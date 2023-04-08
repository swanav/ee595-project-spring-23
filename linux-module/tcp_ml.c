/* Kernel Module for TCP ML Congestion Control Algorithm  */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <net/tcp.h>

#define MODULE_TAG "tcp_ml"

#define LOG_KERN(level, fmt, ...) \
    printk(level MODULE_TAG ": " fmt "\n", ##__VA_ARGS__)

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
    LOG_KERN(KERN_INFO, "Handling congestion window event %d", tp->snd_cwnd);

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
    int ret = tcp_register_congestion_control(&tcp_ml);
    if (ret)
        return ret;
    LOG_KERN(KERN_INFO, "Registering tcp_ml");
    return 0;
}

/* Clean up the module */
static void __exit tcp_ml_unregister(void) {
    LOG_KERN(KERN_INFO, "Unregistering tcp_ml");
    tcp_unregister_congestion_control(&tcp_ml);
}

module_init(tcp_ml_register);
module_exit(tcp_ml_unregister);

MODULE_AUTHOR("Rutuparna K Patkar, Swanav Swaroop");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Implementation of the TCP ML Congestion Control Algorithm developed at USC for EE595 - Software Design and Optimization");
MODULE_VERSION("1.0");