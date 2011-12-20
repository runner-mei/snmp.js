
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

#include <memory>
#include "stream_adapter.h"
#include "session.h"


#define TO_STREAM(t) CONTAINING_RECORD(t,Stream, t_);

Stream::Stream() {
    session_ = NULL;

    t_.f_open     = NULL;//netsnmp_udpbase_recv;
    t_.f_recv     = &Recv;//netsnmp_udpbase_recv;
    t_.f_send     = &Send;//netsnmp_udpbase_send;
    t_.f_close    = &Close;
    t_.f_accept   = NULL;
    t_.f_fmtaddr  = NULL;//netsnmp_udp_fmtaddr;
    t_.f_setup_session  = &SetupSession;
    t_.f_copy     = NULL;//netsnmp_udp_fmtaddr;
    t_.f_config   = NULL;//netsnmp_udp_fmtaddr;

    t_.flags      = 0;
#ifdef FOR_STANDARDS_COMPLIANCE_OR_FUTURE_USE
    t_.tmStateRef = NULL;
#endif
    t_.base_transport = NULL;
    t_.identifier = NULL;

    t_.sock       = 0;
}

//bool Stream::init(struct sockaddr_storage* addr, int local) {
//	v8::HandleScope scope;
//	v8::Handle<v8::Value> args1[1];
//	v8::Handle<v8::Value> args2[2];
//
//	// get process from global scope.
//	v8::Local<v8::Object> global = v8::Context::GetCurrent()->Global();
//	v8::Local<v8::Object> process = global->Get(process_symbol)->ToObject();
//	v8::Local<v8::Object> require = global->Get(require_symbol)->ToObject();
//
//	// dgram = require('dgram');
//	args1[0] = dgram_symbol;
//	v8::Local<v8::Object> dgram = require->CallAsFunction(global, 1, args1)->ToObject();
//
//	// sock = dgram.createSocket('udp4');
//	args2[0] = (addr->ss_family == AF_INET)?udp4_symbol:udp6_symbol;
//	args2[1] = this->on_message_callback;
//	v8::Local<v8::Value> sock = v8::Local<v8::Function>::Cast(dgram->Get(createSocket_symbol))->Call(dgram, 2, args1);
//	if(sock->IsNull() || sock->IsUndefined()) {
//		return false;
//	}
//
//	stream = v8::Persistent<v8::Object>::New(sock->ToObject());
//
//	// sock.bind(port)
//	args2[0] = from_uint16(ntohs((addr->ss_family == AF_INET)?((struct sockaddr_in*)addr)->sin_port:((struct sockaddr_in6*)addr)->sin6_port));
//	args2[1] = on_binding_callback;
//	v8::Local<v8::Function>::Cast(stream->Get(bind_symbol)->ToObject())->Call(sock->ToObject(), 2, args2);
//}


Stream* Stream::ToStream(netsnmp_transport *t) {
    return TO_STREAM(t);
}

int Stream::SetupSession(netsnmp_transport *t, snmp_session *s) {
    Stream* stream = TO_STREAM(t);
    Session* session = Session::ToSession(s);
    session->on_open(stream);
    stream->session_ = s;
    return SNMPERR_SUCCESS;
}

int Stream::Recv(netsnmp_transport* t, void *buf, int size,
                 void **opaque, int *olength) {

    Stream* stream = TO_STREAM(t);
    Session* session = Session::ToSession(stream->session_);
    // TODO
    struct sockaddr *from;
    socklen_t       fromlen = sizeof(struct sockaddr);

    netsnmp_indexed_addr_pair* addr_pair = (netsnmp_indexed_addr_pair *) malloc(sizeof(netsnmp_indexed_addr_pair));
    memset(addr_pair, 0, sizeof(netsnmp_indexed_addr_pair));
    from = &addr_pair->remote_addr.sa;

    int rc = recvfrom(t->sock, (char*)buf, size, NETSNMP_DONTWAIT, from, &fromlen);
    if (rc < 0) {
        DEBUGMSGTL(("netsnmp_udp", "recvfrom fd %d err %d (\"%s\")\n",
                    t->sock, errno, strerror(errno)));
    }

    *opaque = (void *)addr_pair;
    *olength = sizeof(netsnmp_indexed_addr_pair);
    return rc;
}

int Stream::Send(netsnmp_transport *t, void *buf, int size, void **opaque, int *olength) {

    Stream* stream = TO_STREAM(t);
    Session* session = Session::ToSession(stream->session_);
    // TODO
    int rc = -1;
    netsnmp_indexed_addr_pair *addr_pair = NULL;
    struct sockaddr *to = NULL;

    if (opaque != NULL && *opaque != NULL &&
            ((*olength == sizeof(netsnmp_indexed_addr_pair) ||
              (*olength == sizeof(struct sockaddr_in))))) {

        addr_pair = (netsnmp_indexed_addr_pair *) (*opaque);
    } else if (t != NULL && t->data != NULL &&
               t->data_length == sizeof(netsnmp_indexed_addr_pair)) {
        addr_pair = (netsnmp_indexed_addr_pair *) (t->data);
    }

    to = &addr_pair->remote_addr.sa;

    if (to != NULL && t != NULL && t->sock >= 0) {
        char *str = netsnmp_udp_fmtaddr(NULL, (void *) addr_pair,
                                        sizeof(netsnmp_indexed_addr_pair));
        DEBUGMSGTL(("netsnmp_udp", "send %d bytes from %p to %s on fd %d\n",
                    size, buf, str, t->sock));
        free(str);
        while (rc < 0) {
            //rc = sendto(t->sock, buf, size, 0, to, sizeof(struct sockaddr));
            if (rc < 0 && errno != EINTR) {
                DEBUGMSGTL(("netsnmp_udp", "sendto error, rc %d (errno %d)\n",
                            rc, errno));
                break;
            }
        }
    }
    return rc;
}

int Stream::Close(netsnmp_transport *t) {
    Stream* stream = TO_STREAM(t);
    Session* session = Session::ToSession(stream->session_);
    session->on_close(stream);
    delete stream;
    return SNMPERR_SUCCESS;
}