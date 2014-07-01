
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>

#include <unistd.h>
#include <netdb.h>

#include <getopt.h>

#include "payload.h"
#include "getinfo.h"

#define DEVICE_CODE "123456789012345678"
#define PORT        9930

#define DEF_BOSS_HOST   "campal-api.everlink.net"
#define DEF_BOSS_PORT   80

#define IDLEN           16
#define HN_BUFFSIZE     127

char id[IDLEN + 1];
char hostname[HN_BUFFSIZE + 1];

int self_ip(void)
{
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    struct in_addr *s = NULL;
    int ret = 0;

    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family == AF_INET) { // check it is IP4
            char mask[INET_ADDRSTRLEN];
            void* mask_ptr = &((struct sockaddr_in*) ifa->ifa_netmask)->sin_addr;
            inet_ntop(AF_INET, mask_ptr, mask, INET_ADDRSTRLEN);
            if (strcmp(mask, "255.0.0.0") != 0) {
                // is a valid IP4 Address
                s = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
                // printf("ip = %x \n", s->s_addr);
                ret = s->s_addr;
            } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
                // is a valid IP6 Address
                // do something
            }
        }
    }
    if (ifAddrStruct != NULL) freeifaddrs(ifAddrStruct);
    return ret;
}

int main(int argc, char* argv[])
{
    struct sockaddr_in si_other;
    unsigned int server_host;
    short server_port;
    socklen_t slen=sizeof(si_other);
    char buf[256];
    int s;

    // The server's endpoint
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;

//

    int option;
    int p_code = 0;
    struct payload payload;
    int s_port = DEF_BOSS_PORT;
    struct hostinfo i;
    int ret = -1;

    strncpy(hostname, DEF_BOSS_HOST, HN_BUFFSIZE);

    while((option = getopt(argc, argv, "s:c:p:")) != -1) {
        switch(option) {
            case 's':
                strncpy(hostname, optarg, HN_BUFFSIZE);
                break;
            case 'p':
                s_port = atoi(optarg);
                break;
            case 'c':
                strncpy(id, optarg, IDLEN);
                p_code = 1;
                break;
        }
    }

    if(!p_code)
    {
        printf("Wrong parameters. Usage %s -c <PAIR_CODE> \n", argv[0]);
        exit(1);
    }

    ret = info_by_id(hostname, s_port, id, EL_SENDER_ROLE_ISLAND, &i);
    if (ret != 0)
    {
        printf("get info error %d\n", ret);
        exit(2);
    }

    printf("scan.. %s %d %s %d %s %d %s %d\n", i.sip, i.spo, i.cip, i.cpo, i.nip, i.npo, i.hip, i.hpo);

    inet_pton(AF_INET, i.sip, &si_other.sin_addr);
    si_other.sin_port = htons(i.spo);
    strncpy(payload.p.c.id, id, 16);

    if(!p_code) {
        printf("Wrong parameters. Usage %s -i <SERVER_IP> -p <SERVER_PORT> -c <ID_CODE>\n", argv[0]);
        exit(1);
    }

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    {
        perror("socket");
        exit(1);
    }


    server_host = si_other.sin_addr.s_addr;
    server_port = si_other.sin_port;


    payload.p.c.host = self_ip();
    payload.p.c.port = PORT;
    struct sockaddr_in a;
    a.sin_addr.s_addr = payload.p.c.host;
    printf("conv %s\n", inet_ntoa(a.sin_addr));

    payload.sender_role = EL_SENDER_ROLE_ISLAND;

    if (sendto(s, &payload, sizeof(payload), 0, (struct sockaddr*)(&si_other), slen)==-1)
    {
        perror("sendto");
        exit(1);
    }

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        if (recvfrom(s, &buf, sizeof(buf), 0, (struct sockaddr*)(&si_other), (socklen_t*)&slen)==-1)
        {
            perror("recvfrom");
            exit(1);
        }

        if (server_host == si_other.sin_addr.s_addr && server_port == (short)(si_other.sin_port))
        {
            printf("package from server.\n");
            printf("calling peer..\n");
            memcpy(&payload, buf, sizeof(payload));
            printf("role:0x%x, signal:0x%x\n", payload.sender_role, payload.signal);

            switch(fork()) {
                case -1:
                    perror("fork()");
                    exit(EXIT_FAILURE);
                case 0:
                    /* child process, keep sending headbeat */
                    while (1)
                    {
                      sleep (10);
                      if (sendto(s, "HEARTBEAT", 10, 0, (struct sockaddr*)(&payload.p.s.addr), slen)==-1)
                      {
                          printf("calling peer.. FAILED\n");
                      }
                      sleep (20);
                    }
                    break;
                default:
                    break;
            }

//            if (sendto(s, "HI", 3, 0, (struct sockaddr*)(&payload.p.s.addr), slen)==-1)
//            {
//                printf("calling peer.. FAILED\n");
//            }
        }
        else
        {
            printf("[FROM %s:%d] => %s", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
        }
    }

    // Actually, we never reach this point...
    close(s);
    return 0;
}
