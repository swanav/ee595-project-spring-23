#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdint.h>

struct my_netlink_msg
{
    uint32_t cwnd;
    uint32_t rtt;
};

#define NETLINK_USER 31

struct sockaddr_nl src_addr, dest_addr; /* source and destination addresses */
struct nlmsghdr *nlh = NULL;            /* Netlink header */
struct iovec iov;                       /* IO vector needed for sendmsg */
int sock_fd;                            /* socket file descriptor */
struct msghdr msg;                      /* message header */

int main()
{
    /* Create a socket */
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0)
        return -1;

    /* To prepare binding */
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    /* Bind to the source address */
    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

    /* To prepare destination address */
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;    /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    /* Prepare the message header */
    // nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD)); // 16 + 1024
    // memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    // nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD); // 1032

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct my_netlink_msg)));
    memset(nlh, 0, NLMSG_SPACE(sizeof(struct my_netlink_msg)));
    nlh->nlmsg_len = NLMSG_SPACE(sizeof(struct my_netlink_msg));

    nlh->nlmsg_pid = getpid(); /* self pid */
    nlh->nlmsg_flags = 0;      /* unicast */

    // /* Fill in the netlink message payload */
    // strcpy((char *)NLMSG_DATA(nlh), "Hello"); // 1032 - 16 = 1016

    // /* Fill the netlink message payload */
    // iov.iov_base = (void *)nlh;
    // iov.iov_len = nlh->nlmsg_len;
    // msg.msg_name = (void *)&dest_addr;
    // msg.msg_namelen = sizeof(dest_addr);
    // msg.msg_iov = &iov;
    // msg.msg_iovlen = 1; // Len is 1 because we only have 1 iov

    // printf("Sending message to kernel\n");
    // sendmsg(sock_fd, &msg, 0);
    // printf("Waiting for message from kernel\n");

    while (1)
    {
        /* Read message from kernel */
        recvmsg(sock_fd, &msg, 0);
        // printf("Received message payload: %s\n", NLMSG_DATA(nlh));

        struct my_netlink_msg *rcv_data = (struct my_netlink_msg *)NLMSG_DATA(nlh);
        printf("Received message payload: cwnd=%u, rtt=%u \n", rcv_data->cwnd, rcv_data->rtt);

        /* Fill in the netlink message payload */
        strcpy((char *)NLMSG_DATA(nlh), "Hello"); // 1032 - 16 = 1016

        /* Fill the netlink message payload */
        iov.iov_base = (void *)nlh;
        iov.iov_len = nlh->nlmsg_len;
        msg.msg_name = (void *)&dest_addr;
        msg.msg_namelen = sizeof(dest_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1; // Len is 1 because we only have 1 iov

        struct my_netlink_msg msg_data;
        msg_data.cwnd = 44;
        msg_data.rtt = 55;

        memcpy(NLMSG_DATA(nlh), &msg_data, sizeof(struct my_netlink_msg));
        printf("Sending the updated message to kernel message to kernel\n");
        sendmsg(sock_fd, &msg, 0);
        printf("Waiting for message from kernel\n");
    }

    close(sock_fd);
}