#ifndef __GETINFO_H
#define __GETINFO_H

#define IN
#define OUT

struct hostinfo {
    int spo;                  // port of stun server
    int cpo;                  // port of client/internal, copy from payload
    int npo;                  // port of nat/external
    int hpo;                  // port of heartbeat server 
    char sip[IP_BUFFSIZE];    // ip of stun server
    char cip[IP_BUFFSIZE];    // ip of client/internal, copy from payload
    char nip[IP_BUFFSIZE];    // ip of nat/external
    char hip[IP_BUFFSIZE];    // ip of heartbeat server
};

int info_by_id(IN const char *hostname, IN int s_port, IN const char *id, IN int role, OUT struct hostinfo *i);

#endif

