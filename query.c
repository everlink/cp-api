
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

/* customer id, enquire with sales@everlink.net */
const char *cstm_id =   "t0001";                                // for DEMO and pilot production

int info_by_id(IN const char *hostname, IN int s_port, IN const char *id, IN int role, OUT struct hostinfo *i)
{
    static char req[HTTP_BUFFSIZE + 1];
    static char respbuf[HTTP_BUFFSIZE + 1];
    struct sockaddr_in boss_addr;
    struct hostent *hptr;
    int sock_tcp = 0;
    size_t n;
    char* ptr = NULL;

    // Get the ip address of BOSS server (server should be campal-api.everlink.net:80)
    // TODO IPV6 not handled
    if( (hptr = gethostbyname(hostname) ) == NULL )
    {
        printf("Unknown host:%s\n", hostname);
        return -1;
    }

    boss_addr.sin_addr = *(struct in_addr*)hptr->h_addr_list[0];
    boss_addr.sin_port = htons(s_port);
    boss_addr.sin_family = AF_INET;
    printf("STUN BOSS -> %s:%d\n", inet_ntoa(boss_addr.sin_addr), s_port);

    // create a STREAM/TCP socket for HTTP request
    if((sock_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("tcp socket()");
        return -2;
    }

    // connect to BOSS server
    if(connect(sock_tcp, (struct sockaddr*)&boss_addr, sizeof(boss_addr)) < 0)
    {
        perror("tcp connect()");
        close(sock_tcp);
        return -3;
    }

    // Form the HTTP request
    snprintf(req, HTTP_BUFFSIZE,
        "GET /?cid=%s&id=%s&role=%d&fmt=plain HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "\r\n",
        cstm_id, id, role, hostname, s_port);
    printf("HTTP REQUEST:\n%s", req);            // LOG

    // Send out HTTP request and read the response
    if (write(sock_tcp, req, strlen(req))>= 0) 
    {
        while ((n = read(sock_tcp, respbuf, HTTP_BUFFSIZE)) > 0) 
        {
            respbuf[n] = '\0';
            printf("\nRESP:\n%s\n", respbuf);    // LOG
            // Moving to response body
            ptr = strstr(respbuf, "\r\n\r\n");
            printf("\nPAYLOAD:\n%s\n", ptr + 4);  // LOG
            break;
        }
    }

    close(sock_tcp);

    // Scan the result from response body
    if (!ptr || 8 != sscanf(ptr + 4, "%s %d %s %d %s %d %s %d",
          i->sip, &i->spo, i->cip, &i->cpo, i->nip, &i->npo, i->hip, &i->hpo))
    {
      printf("Invalid ID.\n");
      return -4;
    }

    return 0;
}

