
#include <syslog.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
//#include <netpacket/packet.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

#include "nettest.h"
#include "interface.h"

static void set_promisc(const char *ifname, bool enable) {
    struct packet_mreq mreq = {0};
    int sfd;
    int action;

    if ((sfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        syslog(LOG_ERR, "unable to open socket: %s", strerror(errno));
        return;
    }

    mreq.mr_ifindex = if_nametoindex(ifname);
    mreq.mr_type = PACKET_MR_PROMISC;

    if (mreq.mr_ifindex == 0) {
        syslog(LOG_ERR, "unable to get interface index: %s", strerror(errno));
        return;
    }

    if (enable)
        action = PACKET_ADD_MEMBERSHIP;
    else
        action = PACKET_DROP_MEMBERSHIP;

    if (setsockopt(sfd, SOL_PACKET, action, &mreq, sizeof(mreq)) != 0) {
        syslog(LOG_ERR, "unable to enter promiscouous mode: %s", strerror(errno));
        return;
    }

    close(sfd);
}

static int open_raw_socket(char *name) {
    int sock_raw = socket(AF_INET , SOCK_RAW , IPPROTO_TCP);
    
    const int len = strnlen(name, IFNAMSIZ);
    if (len == IFNAMSIZ) {
        syslog(LOG_ERR, "Too long iface name");
        close(sock_raw);
        return -1;
    }
    setsockopt(sock_raw, SOL_SOCKET, SO_BINDTODEVICE, name, len);
    return sock_raw;   
}

if_handle* init_input_interface(rule_t *cfg) {
    set_promisc(cfg->in, true);
    int sock_raw = open_raw_socket(cfg->in);
    if(sock_raw < 0)
        return NULL;
    if_handle* hndl = malloc(sizeof(if_handle));
    if(!hndl)
        return NULL;

    hndl->socket = sock_raw;    
    return hndl;
}

if_handle* init_output_interface(rule_t *cfg) {
    int sock_raw = open_raw_socket(cfg->out);
    if(sock_raw < 0)
        return NULL;
    if_handle* hndl = malloc(sizeof(if_handle));
    if(!hndl)
        return NULL;

    hndl->socket = sock_raw;
    return hndl;

}


void close_interface(if_handle *cfg) {
    close(cfg->socket);
    free(cfg);
}

