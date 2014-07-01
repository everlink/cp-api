#ifndef __PAYLOAD_H
#define __PAYLOAD_H

#if 0

# !! WARNING !!
# !! WARNING !! For now, we only support little endian, if your business needs BIG endian, please send mail to sales <noel@everlink.net>
# !! WARNING !!

#include <endian.h>
#ifdef __BYTE_ORDER
#  if __BYTE_ORDER != __LITTLE_ENDIAN
#    error "Endian not support!"
#  endif
#else
#  error "Endian determination failed!"
#endif  /* #ifdef __BYTE_ORDER */

#endif

#define EL_SIGNAL_KNOCK         0x01
#define EL_SENDER_ROLE_ISLAND   0x04
#define EL_SENDER_ROLE_TOWER    0x08
#define EL_SENDER_ROLE_SERVER   0x10

#define IP_BUFFSIZE     63
#define HTTP_BUFFSIZE   1023


struct payload
{
    char payload_version;
    char signal;
    char sender_role;
    char reserved;
    union
    {
        struct
        {
            char id[16];
            int host;
            short port;
        } c;
        struct
        {
            struct sockaddr_in addr;
        } s;
    } p;
};

#endif  /* __PAYLOAD */

