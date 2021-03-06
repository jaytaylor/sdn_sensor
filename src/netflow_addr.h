/*
 * Copyright (c) 2004, 2005 Damien Miller <djm@mindrot.org>
 * Copyright (c) 2014 Matthew Hall <mhall@mhcomputing.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __NETFLOW_ADDR_H__
#define __NETFLOW_ADDR_H__

/* Address handling routines */

#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
#include "netflow_common.h"

struct xaddr {
    sa_family_t    af;
    union {
        struct in_addr        v4;
        struct in6_addr        v6;
        u_int8_t        addr8[16];
        u_int16_t        addr16[8];
        u_int32_t        addr32[4];
    } xa;            /* 128-bit address */
    u_int32_t    scope_id;    /* iface scope id for v6 */
#define v4    xa.v4
#define v6    xa.v6
#define addr8    xa.addr8
#define addr16    xa.addr16
#define addr32    xa.addr32
};

/* BEGIN PROTOTYPES */

int addr_unicast_masklen(int af);
int addr_masklen_valid(int af, int masklen);
int addr_xaddr_to_sa(const struct xaddr* xa, struct sockaddr* sa, socklen_t* len, u_int16_t port);
int addr_sa_to_xaddr(struct sockaddr* sa, socklen_t slen, struct xaddr* xa);
int addr_frame_to_xaddr(ss_frame_t* fbuf, struct xaddr* xa);
int addr_invert(struct xaddr* n);
int addr_netmask(int af, int l, struct xaddr* n);
int addr_hostmask(int af, int l, struct xaddr* n);
int addr_and(struct xaddr* dst, const struct xaddr* a, const struct xaddr* b);
int addr_or(struct xaddr* dst, const struct xaddr* a, const struct xaddr* b);
int addr_cmp(const struct xaddr* a, const struct xaddr* b);
int addr_is_all0s(const struct xaddr* a);
int addr_host_is_all0s(const struct xaddr* a, int masklen);
int addr_host_is_all1s(const struct xaddr* a, int masklen);
int addr_host_to_all0s(struct xaddr* a, int masklen);
int addr_host_to_all1s(struct xaddr* a, int masklen);
int addr_pton(const char* p, struct xaddr* n);
int addr_sa_pton(const char* h, const char* s, struct sockaddr* sa, socklen_t slen);
int addr_ntop(const struct xaddr* n, char* p, socklen_t len);
int addr_sa_ntop(const struct sockaddr* sa, socklen_t slen, char* h, socklen_t hlen, char* p, socklen_t plen);
int addr_pton_cidr(const char* p, struct xaddr* n, int* l);
int addr_netmatch(const struct xaddr* host, const struct xaddr* net, int masklen);
const char* addr_ntop_buf(const struct xaddr* a);

/* END PROTOTYPES */

#endif /* __NETFLOW_ADDR_H__ */
