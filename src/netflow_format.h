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

#ifndef __NETFLOW_FORMAT_H__
#define __NETFLOW_FORMAT_H__

/* On-disk storage format */

#include "ioc.h"
#include "netflow_common.h"
#include "netflow_addr.h"

/* On-disk address formats for v4 and v6 addresses */
struct store_addr6 {
    u_int8_t    d[16];
} __packed;
struct store_addr4 {
    u_int8_t    d[4];
} __packed;

#define STORE_VER_MIN_MASK    ((1 << 5) - 1)
#define STORE_VER_MAJ_MASK    ((1 << 3) - 1)
#define STORE_MKVER(maj,min)    (((maj & STORE_VER_MAJ_MASK) << 5) | \
                  (min & STORE_VER_MIN_MASK))
#define STORE_VER_GET_MAJ(ver)    ((ver >> 5) & STORE_VER_MAJ_MASK)
#define STORE_VER_GET_MIN(ver)    (ver & STORE_VER_MIN_MASK)

#define STORE_VER_MAJOR        3
#define STORE_VER_MINOR        0
#define STORE_VERSION        STORE_MKVER(STORE_VER_MAJOR, STORE_VER_MINOR)

/* Start of flow record - present for every flow */
struct store_flow {
    u_int8_t        version;
    u_int8_t        len_words; /* len in 4 byte words not inc hdr */
    u_int16_t        reserved;
    u_int32_t        fields;
} __packed;

/*
 * Optional flow fields, specify what is stored for the flow
 * NB - the flow records appear in this order on disk
 */
#define STORE_FIELD_TAG            (1U)
#define STORE_FIELD_RECV_TIME        (1U<<1)
#define STORE_FIELD_PROTO_FLAGS_TOS    (1U<<2)
#define STORE_FIELD_AGENT_ADDR4        (1U<<3)
#define STORE_FIELD_AGENT_ADDR6        (1U<<4)
#define STORE_FIELD_SRC_ADDR4        (1U<<5)
#define STORE_FIELD_SRC_ADDR6        (1U<<6)
#define STORE_FIELD_DST_ADDR4        (1U<<7)
#define STORE_FIELD_DST_ADDR6        (1U<<8)
#define STORE_FIELD_GATEWAY_ADDR4    (1U<<9)
#define STORE_FIELD_GATEWAY_ADDR6    (1U<<10)
#define STORE_FIELD_SRCDST_PORT        (1U<<11)
#define STORE_FIELD_PACKETS        (1U<<12)
#define STORE_FIELD_OCTETS        (1U<<13)
#define STORE_FIELD_IF_INDICES        (1U<<14)
#define STORE_FIELD_AGENT_INFO        (1U<<15)
#define STORE_FIELD_FLOW_TIMES        (1U<<16)
#define STORE_FIELD_AS_INFO        (1U<<17)
#define STORE_FIELD_FLOW_ENGINE_INFO    (1U<<18)
/* ... more one day */

#define STORE_FIELD_CRC32        (1U<<30)
#define STORE_FIELD_RESERVED        (1U<<31) /* For extension header */
#define STORE_FIELD_ALL            (((1U<<19)-1)|STORE_FIELD_CRC32)

/* Useful combinations */
#define STORE_FIELD_AGENT_ADDR        (STORE_FIELD_AGENT_ADDR4|\
                     STORE_FIELD_AGENT_ADDR6)
#define STORE_FIELD_SRC_ADDR        (STORE_FIELD_SRC_ADDR4|\
                     STORE_FIELD_SRC_ADDR6)
#define STORE_FIELD_DST_ADDR        (STORE_FIELD_DST_ADDR4|\
                     STORE_FIELD_DST_ADDR6)
#define STORE_FIELD_SRCDST_ADDR        (STORE_FIELD_SRC_ADDR|\
                     STORE_FIELD_DST_ADDR)
#define STORE_FIELD_GATEWAY_ADDR    (STORE_FIELD_GATEWAY_ADDR4|\
                     STORE_FIELD_GATEWAY_ADDR6)

#define STORE_DISPLAY_ALL        STORE_FIELD_ALL
#define STORE_DISPLAY_BRIEF        (STORE_FIELD_TAG|\
                     STORE_FIELD_RECV_TIME|\
                     STORE_FIELD_PROTO_FLAGS_TOS|\
                     STORE_FIELD_SRCDST_PORT|\
                     STORE_FIELD_PACKETS|\
                     STORE_FIELD_OCTETS|\
                     STORE_FIELD_SRCDST_ADDR|\
                     STORE_FIELD_AGENT_ADDR4|\
                     STORE_FIELD_AGENT_ADDR6)

/*
 * Optional flow records
 * NB. suffixes must match the corresponding STORE_FIELD_ define (see store.c)
 */

/* Optional flow field - present if STORE_FIELD_TAG */
struct store_flow_TAG {
    u_int32_t        tag; /* set by filter */
} __packed;

/* Optional flow field - present if STORE_FIELD_RECV_TIME */
struct store_flow_RECV_TIME {
    u_int32_t        recv_sec;
    u_int32_t        recv_usec;
} __packed;

/* Optional flow field - present if STORE_FIELD_PROTO_FLAGS_TOS */
struct store_flow_PROTO_FLAGS_TOS {
    u_int8_t        tcp_flags;
    u_int8_t        protocol;
    u_int8_t        tos;
    u_int8_t        pad;
} __packed;

/* Optional flow field - present if STORE_FIELD_AGENT_ADDR */
struct store_flow_AGENT_ADDR4 {
    struct store_addr4    flow_agent_addr;
} __packed;
struct store_flow_AGENT_ADDR6 {
    struct store_addr6    flow_agent_addr;
} __packed;

/* Optional flow field - present if STORE_FIELD_SRC_ADDR4 */
struct store_flow_SRC_ADDR4 {
    struct store_addr4    src_addr;
} __packed;

/* Optional flow field - present if STORE_FIELD_DST_ADDR4 */
struct store_flow_DST_ADDR4 {
    struct store_addr4    dst_addr;
} __packed;

/* Optional flow field - present if STORE_FIELD_SRC_ADDR6 */
struct store_flow_SRC_ADDR6 {
    struct store_addr6    src_addr;
} __packed;

/* Optional flow field - present if STORE_FIELD_DST_ADDR6 */
struct store_flow_DST_ADDR6 {
    struct store_addr6    dst_addr;
} __packed;

/* Optional flow field - present if STORE_FIELD_GATEWAY_ADDR */
struct store_flow_GATEWAY_ADDR4 {
    struct store_addr4    gateway_addr;
} __packed;
struct store_flow_GATEWAY_ADDR6 {
    struct store_addr6    gateway_addr;
} __packed;

/* Optional flow field - present if STORE_FIELD_SRCDST_PORT */
struct store_flow_SRCDST_PORT {
    u_int16_t        src_port;
    u_int16_t        dst_port;
} __packed;

/* Optional flow field - present if STORE_FIELD_PACKETS */
struct store_flow_PACKETS {
    u_int64_t        flow_packets;
} __packed;

/* Optional flow field - present if STORE_FIELD_OCTETS */
struct store_flow_OCTETS {
    u_int64_t        flow_octets;
} __packed;

/* Optional flow field - present if STORE_FIELD_IF_INDICES */
struct store_flow_IF_INDICES {
    u_int32_t        if_index_in;
    u_int32_t        if_index_out;
} __packed;

/* Optional flow field - present if STORE_FIELD_AGENT_INFO */
struct store_flow_AGENT_INFO {
    u_int32_t        sys_uptime_ms;
    u_int32_t        time_sec;
    u_int32_t        time_nanosec;
    u_int16_t        netflow_version;
    u_int16_t        pad;
} __packed;

/* Optional flow field - present if STORE_FIELD_FLOW_TIMES */
struct store_flow_FLOW_TIMES {
    u_int32_t        flow_start;
    u_int32_t        flow_finish;
} __packed;

/* Optional flow field - present if STORE_FIELD_AS_INFO */
struct store_flow_AS_INFO {
    u_int32_t        src_as;
    u_int32_t        dst_as;
    u_int8_t        src_mask;
    u_int8_t        dst_mask;
    u_int16_t        pad;
} __packed;

/* Optional flow field - present if STORE_FIELD_FLOW_ENGINE_INFO */
struct store_flow_FLOW_ENGINE_INFO {
    u_int16_t        engine_type;
    u_int16_t        engine_id;
    u_int32_t        flow_sequence;
    u_int32_t        source_id;
} __packed;

/* Optional flow field - present if STORE_FIELD_CRC32 */
struct store_flow_CRC32 {
    u_int32_t        crc32;
} __packed;

/* A abstract flow record (all fields included) */
struct store_flow_complete {
    struct store_flow            hdr;
    struct store_flow_TAG            tag;
    struct store_flow_RECV_TIME        recv_time;
    struct store_flow_PROTO_FLAGS_TOS    pft;
    struct xaddr                agent_addr;
    struct xaddr                src_addr;
    struct xaddr                dst_addr;
    struct xaddr                gateway_addr;
    struct store_flow_SRCDST_PORT        ports;
    struct store_flow_PACKETS        packets;
    struct store_flow_OCTETS        octets;
    struct store_flow_IF_INDICES        ifndx;
    struct store_flow_AGENT_INFO        ainfo;
    struct store_flow_FLOW_TIMES        ftimes;
    struct store_flow_AS_INFO        asinf;
    struct store_flow_FLOW_ENGINE_INFO    finf;
    struct store_flow_CRC32            crc32;
} __packed;

typedef struct ss_ioc_entry_s ss_ioc_entry_t;

/* BEGIN PROTOTYPES */

const char* iso_time(time_t t, int utc_flag);
const char* interval_time(time_t t);
u_int64_t netflow_swp_ntoh64(u_int64_t v);
u_int32_t netflow_swp_ntoh32(u_int32_t v);
u_int16_t netflow_swp_ntoh16(u_int16_t v);
u_int64_t netflow_swp_hton64(u_int64_t v);
u_int32_t netflow_swp_hton32(u_int32_t v);
u_int16_t netflow_swp_hton16(u_int16_t v);
u_int64_t netflow_swp_fake64(u_int64_t v);
u_int32_t netflow_swp_fake32(u_int32_t v);
u_int16_t netflow_swp_fake16(u_int16_t v);
uint8_t* ss_metadata_prepare_netflow(const char* source, nn_queue_t* nn_queue, struct store_flow_complete* flow, ss_ioc_entry_t* ioc_entry);
void netflow_format_flow(struct store_flow_complete* flow, char* buf, size_t len, int utc_flag, u_int32_t display_mask, int hostorder);
u_int64_t netflow_ntohll(u_int64_t v);
u_int64_t netflow_htonll(u_int64_t v);

/* END PROTOTYPES */

#endif /* __NETFLOW_FORMAT_H__ */
