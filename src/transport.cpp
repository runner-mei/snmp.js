
/*
 * Copyright (c) 2011 runner.mei
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "snmp.h"
#include "stream_adapter.h"



static netsnmp_tdomain meiDomain;
oid             meiDomain_oid[] = { 1, 3, 6, 1, 6, 1, 112 };


static netsnmp_tdomain mei6Domain;
oid             mei6Domain_oid[] = { 1, 3, 6, 1, 6, 1, 113 };




void init4(netsnmp_transport* t, struct sockaddr_in* addr, int local_port) {
    t->local = (u_char *) malloc(6);
    memcpy(t->local, (u_char *) & (addr->sin_addr.s_addr), 4);
    t->local[4] = (htons(addr->sin_port) & 0xff00) >> 8;
    t->local[5] = (htons(addr->sin_port) & 0x00ff) >> 0;
    t->local_length = 6;

    t->remote = (u_char *)malloc(6);
    memcpy(t->remote, (u_char *) & (addr->sin_addr.s_addr), 4);
    t->remote[4] = (htons(addr->sin_port) & 0xff00) >> 8;
    t->remote[5] = (htons(addr->sin_port) & 0x00ff) >> 0;
    t->remote_length = 6;


    t->data = malloc(sizeof(netsnmp_indexed_addr_pair));
    t->data_length = sizeof(netsnmp_indexed_addr_pair);
    memset(t->data, 0, sizeof(netsnmp_indexed_addr_pair));

    netsnmp_indexed_addr_pair* pair = (netsnmp_indexed_addr_pair*) t->data;
    memcpy(&pair->remote_addr, addr, sizeof(struct sockaddr_in));

    pair->local_addr.sin.sin_family = addr->sin_family;
    pair->local_addr.sin.sin_port = local_port;
}

void init6(netsnmp_transport* t, struct sockaddr_in6* addr, int local_port) {

    t->local = (unsigned char*)malloc(18);
    memcpy(t->local, addr->sin6_addr.s6_addr, 16);
    t->local[16] = (addr->sin6_port & 0xff00) >> 8;
    t->local[17] = (addr->sin6_port & 0x00ff) >> 0;
    t->local_length = 18;


    t->remote = (unsigned char*)malloc(18);
    memcpy(t->remote, addr->sin6_addr.s6_addr, 16);
    t->remote[16] = (addr->sin6_port & 0xff00) >> 8;
    t->remote[17] = (addr->sin6_port & 0x00ff) >> 0;
    t->remote_length = 18;


    t->data = malloc(sizeof(netsnmp_indexed_addr_pair));
    t->data_length = sizeof(netsnmp_indexed_addr_pair);
    memset(t->data, 0, sizeof(netsnmp_indexed_addr_pair));

    netsnmp_indexed_addr_pair* pair = (netsnmp_indexed_addr_pair*) t->data;
    memcpy(&pair->remote_addr, addr, sizeof(struct sockaddr_in6));

    pair->local_addr.sin6.sin6_family = addr->sin6_family;
    pair->local_addr.sin6.sin6_port = local_port;
}

netsnmp_transport *mei_udp_transport(netsnmp_tdomain* domain, struct sockaddr_storage* addr, int local) {
    Stream* stream = new Stream();
    if(NULL == stream) {
        return NULL;
    }

    netsnmp_transport *t = stream->transport();

    /*
     * Set Domain
     */
    t->domain = domain->name;
    t->domain_length = domain->name_length;

    if(AF_INET == addr->ss_family) {
        init4(t, (struct sockaddr_in*)addr, local);
    } else {
        init6(t, (struct sockaddr_in6*)addr, local);
    }

    /*
     * 16-bit length field, 8 byte UDP header, 20 byte IPv4 header
     */

    t->msgMaxSize = 0xffff - 8 - 20;

    return t;
}


netsnmp_transport * mei_udp_create_tstring(const char *str, int local,
        const char *default_target) {
    struct sockaddr_in addr;

    if (netsnmp_sockaddr_in2(&addr, str, default_target)) {
        return mei_udp_transport(&meiDomain, (struct sockaddr_storage*)&addr, local);
    } else {
        return NULL;
    }
}


netsnmp_transport * mei_udp_create_ostring(const u_char * o, size_t o_len, int local) {
    struct sockaddr_in addr;

    if (o_len == 6) {
        unsigned short porttmp = (o[4] << 8) + o[5];
        addr.sin_family = AF_INET;
        memcpy((u_char *) & (addr.sin_addr.s_addr), o, 4);
        addr.sin_port = htons(porttmp);
        return mei_udp_transport(&meiDomain, (struct sockaddr_storage*)&addr, local);
    }
    return NULL;
}


void transport_udp_ctor(void) {
    meiDomain.name = meiDomain_oid;
    meiDomain.name_length = sizeof(meiDomain_oid)/sizeof(oid);
    meiDomain.prefix = (const char**)calloc(2, sizeof(char *));
    meiDomain.prefix[0] = "mei";

    meiDomain.f_create_from_tstring     = NULL;
    meiDomain.f_create_from_tstring_new = mei_udp_create_tstring;
    meiDomain.f_create_from_ostring     = mei_udp_create_ostring;

    netsnmp_tdomain_register(&meiDomain);
}


netsnmp_transport * mei_udp6_create_tstring(const char *str, int local,
        const char *default_target) {
    struct sockaddr_in6 addr;

    if (netsnmp_sockaddr_in6_2(&addr, str, default_target)) {
        return mei_udp_transport(&mei6Domain, (struct sockaddr_storage*)&addr, local);
    } else {
        return NULL;
    }
}

netsnmp_transport* mei_udp6_create_ostring(const u_char * o, size_t o_len, int local) {
    struct sockaddr_in6 addr;

    if (o_len == 18) {
        memset((u_char *) & addr, 0, sizeof(struct sockaddr_in6));
        addr.sin6_family = AF_INET6;
        memcpy((u_char *) & (addr.sin6_addr.s6_addr), o, 16);
        addr.sin6_port = (o[16] << 8) + o[17];
        return mei_udp_transport(&mei6Domain, (struct sockaddr_storage*)&addr, local);
    }
    return NULL;
}

void transport_udp6_ctor(void) {
    mei6Domain.name = mei6Domain_oid;
    mei6Domain.name_length = sizeof(mei6Domain_oid)/sizeof(oid);
    mei6Domain.prefix = (const char**)calloc(2, sizeof(char *));
    mei6Domain.prefix[0] = "mei6";

    mei6Domain.f_create_from_tstring     = NULL;
    mei6Domain.f_create_from_tstring_new = mei_udp6_create_tstring;
    mei6Domain.f_create_from_ostring     = mei_udp6_create_ostring;

    netsnmp_tdomain_register(&mei6Domain);
}