#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include "payload.h"
#include "getinfo.h"

#define DEF_BOSS_HOST   "campal-api.everlink.net"
#define DEF_BOSS_PORT   80

#define TTY_BUFFSIZE    127
#define IDLEN           16
#define HN_BUFFSIZE     127


char id[IDLEN + 1];
char hostname[HN_BUFFSIZE + 1];
int sock_fd;
struct sockaddr_in peer_addr;
void socket_read(void);
void socket_write(void);

int main(int argc, char* argv[]) {
    int option;
    int p_code = 0;
    struct sockaddr_in stun_addr;
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

    ret = info_by_id(hostname, s_port, id, EL_SENDER_ROLE_TOWER, &i);
    if (ret != 0)
    {
        printf("get info error %d\n", ret);
        exit(2);
    }

    printf("scan.. %s %d %s %d %s %d %s %d\n", i.sip, i.spo, i.cip, i.cpo, i.nip, i.npo, i.hip, i.hpo);
    if (0 == inet_pton(AF_INET, i.nip, &peer_addr.sin_addr))
    {
      perror("error");
    }
    peer_addr.sin_port = htons(i.npo);
    peer_addr.sin_family = AF_INET;
    printf("TARGET %s:%d\n", inet_ntoa(peer_addr.sin_addr), peer_addr.sin_port);

    if((sock_fd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket()");
        exit(2);
    }

    /* hi server, please get the island and ask knock me */
    payload.p.s.addr = peer_addr;
    payload.sender_role = EL_SENDER_ROLE_TOWER;

    if (0 == inet_pton(AF_INET, i.sip, &stun_addr.sin_addr))
    {
      perror("error");
    }
    stun_addr.sin_family = AF_INET;
    stun_addr.sin_port = htons(i.spo);
    if(sendto(sock_fd, (void*)&payload, sizeof(payload), 0, (struct sockaddr*)&stun_addr, sizeof(struct sockaddr)) < 0) {
        perror("sendto()");
    }

    /* I would like to know the island first, so that I can get from island 
     * this is need, or will not accept the HI headbeat ACK
     * TODO need some delay before knock or retry knocks ?? */
    if(sendto(sock_fd, "knock", 6, 0, (struct sockaddr*)&peer_addr, sizeof(struct sockaddr)) < 0) {
        perror("sendto()");
    }

    printf("KEY IN SOMETHING TO TEST..\n");

    switch(fork()) {
        case -1:
            perror("fork()");
            exit(EXIT_FAILURE);
        case 0:
            socket_write();
            break;
        default:
            socket_read();
            break;
    }

    return (EXIT_SUCCESS);
}

void socket_write(void) {
    char* buff = malloc(TTY_BUFFSIZE);

    printf(">");
    fflush(stdout);
    while(fgets(buff, TTY_BUFFSIZE, stdin)) {
        if(sendto(sock_fd, buff, strlen(buff), 0, (struct sockaddr*)&peer_addr, sizeof(struct sockaddr)) < 0) {
            perror("sendto()");
        }
        memset(buff, 0, TTY_BUFFSIZE);
        printf(">");
        fflush(stdout);
    }
    free(buff);
}

void socket_read(void) {
    char* buff = malloc(TTY_BUFFSIZE);

    while(1) {
        memset(buff, 0, TTY_BUFFSIZE);
        if(recv(sock_fd, buff, TTY_BUFFSIZE, 0) < 0) {
            perror("recv()");
            exit(1);
        }
        printf("[%s]\n>", buff);
        fflush(stdout);
    }
    free(buff);
}

