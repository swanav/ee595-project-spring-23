/* Kernel Module for TCP ML Congestion Control Algorithm  */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <net/tcp.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <net/sock.h> 

#define MODULE_TAG "tcp_ml"

#define LOG_KERN(level, fmt, ...) \
    printk(level MODULE_TAG ": " fmt "\n", ##__VA_ARGS__)

#define NETLINK_MY_PROTOCOL 31
struct my_netlink_msg {
    uint32_t cwnd;
    uint32_t rtt;
};

static struct sock *netlink_sock = NULL;

static void my_netlink_rcv_msg(struct sk_buff *skb)
{

    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    //char *msg = "Hello from kernel";
    struct my_netlink_msg *msg_data;
    int res;

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

    //msg_size = strlen(msg);
    msg_size = sizeof(struct my_netlink_msg);

    nlh = (struct nlmsghdr *)skb->data;
    //printk(KERN_INFO "Netlink received msg payload:%s\n", (char *)nlmsg_data(nlh));

    // Print reecived data 
    struct my_netlink_msg *rcv_data = (struct my_netlink_msg *)NLMSG_DATA(nlh);
    printk("Reecived cwnd: %d\n",rcv_data->cwnd);


    pid = nlh->nlmsg_pid; /*pid of sending process */

    skb_out = nlmsg_new(msg_size, 0);
    if (!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    //strncpy(nlmsg_data(nlh), msg, msg_size);

    // Send dummy data
    // msg_data->cwnd = 99;
    // msg_data->rtt = 102;

    msg_data = kzalloc(sizeof(struct my_netlink_msg), GFP_KERNEL);
    if (!msg_data) {
        printk(KERN_ERR "Failed to allocate memory for message data\n");
        return;
    }

    msg_data->cwnd = 99;
    msg_data->rtt = 102;

    memcpy(nlmsg_data(nlh), msg_data, msg_size);

    res = nlmsg_unicast(netlink_sock, skb_out, pid);
    if (res < 0)
        printk(KERN_INFO "Error while sending bak to user\n");

}

static struct netlink_kernel_cfg my_netlink_cfg = {
    .input = my_netlink_rcv_msg,
};

/* initialize private data (optional) */
static void tcp_ml_init(struct sock *sk) {
    struct tcp_sock *tp = tcp_sk(sk);
    LOG_KERN(KERN_INFO, "Initializing tcp_ml %d", tp->snd_cwnd);
    tp->snd_cwnd = 1;
}

/* cleanup private data (optional) */
void tcp_ml_release(struct sock *sk) {
    struct tcp_sock *tp = tcp_sk(sk);
    LOG_KERN(KERN_INFO, "Releasing tcp_ml %d", tp->snd_cwnd);
}

 /* return slow start threshold (required) */
static u32 tcp_ml_ssthresh(struct sock *sk) {
    struct tcp_sock *tp = tcp_sk(sk);
    LOG_KERN(KERN_INFO, "Handling slow start threshold event %d", tp->snd_cwnd);

    return 2;
}

/* do new cwnd calculation (required) */
static void tcp_ml_cong_avoid(struct sock *sk, u32 ack, u32 in_flight)
{
    struct tcp_sock *tp = tcp_sk(sk);
    LOG_KERN(KERN_INFO, "Handling congestion avoidance event %d", tp->snd_cwnd);

    tp->snd_cwnd = 2;
}

/* call before changing ca_state (optional) */
static void tcp_ml_set_ca_state(struct sock *sk, u8 new_state) {}

/* call when cwnd event occurs (optional) */
static void tcp_ml_cwnd_event(struct sock *sk, enum tcp_ca_event event)
{
    struct tcp_sock *tp = tcp_sk(sk);
    LOG_KERN(KERN_INFO, "Handling congestion window event %p %d %d", sk, event, tp->snd_cwnd);

    switch (event) {
    case CA_EVENT_TX_START:
        tp->snd_cwnd = 2;
        break;
    case CA_EVENT_CWND_RESTART:
        tp->snd_cwnd = 2;
        break;
    case CA_EVENT_COMPLETE_CWR:
        tp->snd_cwnd = 2;
        break;
    default:
        break;
    }
}

/* new value of cwnd after loss (optional) */
static u32 tcp_ml_undo_cwnd(struct sock *sk) {
    struct tcp_sock *tp = tcp_sk(sk);
    LOG_KERN(KERN_INFO, "Handling undo congestion window event %d", tp->snd_cwnd);

    return 2;
}

/* hook for packet ACK accounting (optional) */
static void tcp_ml_pkts_acked(struct sock *sk, const struct ack_sample* sample) {
    struct tcp_sock *tp = tcp_sk(sk);
    LOG_KERN(KERN_INFO, "Handling packets acked event: num_acked");
}

/* get info for inet_diag (optional) */
static size_t tcp_ml_get_info(struct sock *sk, u32 ext, int* p, union tcp_cc_info *cc_info) {
    return 0;
}

/* Register the new TCP CC algorithm */
static struct tcp_congestion_ops tcp_ml = {
    .init = tcp_ml_init,
    .release = tcp_ml_release,
    .ssthresh = tcp_ml_ssthresh,
    .cong_avoid = tcp_ml_cong_avoid,
    .set_state = tcp_ml_set_ca_state,
    .cwnd_event = tcp_ml_cwnd_event,
    .undo_cwnd = tcp_ml_undo_cwnd,
    .pkts_acked = tcp_ml_pkts_acked,
    .get_info = tcp_ml_get_info,
    .name = "tcp_ml",
    .owner = THIS_MODULE,
};

/* Initialize the module */
static int __init tcp_ml_register(void) {
    /* Register the TCP CC algorithm */
    int ret = tcp_register_congestion_control(&tcp_ml);
    if (ret)
        return ret;
    LOG_KERN(KERN_INFO, "Registering tcp_ml");

    // Create Netlink socket
    netlink_sock = netlink_kernel_create(&init_net, NETLINK_MY_PROTOCOL, &my_netlink_cfg);
    if (!netlink_sock) {
        printk(KERN_ERR "Error creating Netlink socket\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "Created Netlink socket\n");


    return 0;
}

/* Clean up the module */
static void __exit tcp_ml_unregister(void) {
    netlink_kernel_release(netlink_sock);
    printk(KERN_INFO "Released Netlink socket\n");

    /* Unregister the TCP CC algorithm */
    LOG_KERN(KERN_INFO, "Unregistering tcp_ml");
    tcp_unregister_congestion_control(&tcp_ml);
}

module_init(tcp_ml_register);
module_exit(tcp_ml_unregister);

MODULE_AUTHOR("Rutuparna K Patkar, Swanav Swaroop");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Implementation of the TCP ML Congestion Control Algorithm developed at USC for EE595 - Software Design and Optimization");
MODULE_VERSION("1.0");
