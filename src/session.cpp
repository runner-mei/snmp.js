#ifndef __session__cpp__
#define __session__cpp__ 1

#include "snmp.h"
#include "pdu.cpp"
#include <memory>



class Callable {

private:
    v8::Persistent<v8::Object> cb_;
    v8::Persistent<v8::Object> pdu_;
public:
    Callable() {
    }

    virtual ~Callable() {
    }


    static int OnEvent(int code, netsnmp_session *sess, int reqid,
                       netsnmp_pdu *pdu, void *ctxt) {
        v8::HandleScope scope;
        std::auto_ptr<Callable> wrap(reinterpret_cast<Callable*>(ctxt));
        assert(0 != wrap.get());

        v8::TryCatch try_catch;
        v8::Handle<v8::Value> args[] = {
            from_int32(code), wrap->pdu_.IsEmpty()?Pdu::fromPdu(pdu):wrap->pdu_
        };

        // get process from global scope.
        v8::Local<v8::Object> global = v8::Context::GetCurrent()->Global();
        v8::Local<v8::Object> process = global->Get(process_symbol)->ToObject();


        wrap->cb_->CallAsFunction(process, 2, args);
        if (try_catch.HasCaught()) {
            node::FatalException(try_catch);
        }
    }

};


class Session : node::ObjectWrap {
private:
    netsnmp_session arguments_;
    void* session_;

    Session() : session_(0) {
        snmp_sess_init(&arguments_);
    }

    void open() {
        if (NULL == session_) {
            session_ = snmp_sess_open(&arguments_);
        }
    }

    void close() {
        if (NULL != session_) {
            snmp_sess_close(session_);
            session_ = NULL;
        }
    }

public:

    virtual ~Session() {
        close();
    }
	
	
    static void Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;


        v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(New);
        t->SetClassName(Session_symbol);
        t->InstanceTemplate()->SetInternalFieldCount(1);


        SNMP_SET_ACCESSOR(t, version);
        SNMP_SET_ACCESSOR(t, retries);
        SNMP_SET_ACCESSOR(t, timeout);
        SNMP_SET_ACCESSOR(t, flags);
        SNMP_SET_ACCESSOR(t, peername);
        SNMP_SET_ACCESSOR(t, remote_port);
        SNMP_SET_ACCESSOR(t, localname);
        SNMP_SET_ACCESSOR(t, local_port);

        SNMP_SET_ACCESSOR(t, sessid);
        SNMP_SET_ACCESSOR(t, community);
        SNMP_SET_ACCESSOR(t, rcvMsgMaxSize);
        SNMP_SET_ACCESSOR(t, sndMsgMaxSize);

        SNMP_SET_ACCESSOR(t, isAuthoritative);
        SNMP_SET_ACCESSOR(t, contextEngineID);
        SNMP_SET_ACCESSOR(t, engineBoots);
        SNMP_SET_ACCESSOR(t, engineTime);
        SNMP_SET_ACCESSOR(t, contextName);
        SNMP_SET_ACCESSOR(t, securityEngineID);
        SNMP_SET_ACCESSOR(t, securityName);
        SNMP_SET_ACCESSOR(t, securityAuthProto);
        SNMP_SET_ACCESSOR(t, securityAuthKey);
        SNMP_SET_ACCESSOR(t, securityAuthLocalKey);
        SNMP_SET_ACCESSOR(t, securityPrivProto);
        SNMP_SET_ACCESSOR(t, securityPrivKey);
        SNMP_SET_ACCESSOR(t, securityPrivLocalKey);
        SNMP_SET_ACCESSOR(t, securityModel);
        SNMP_SET_ACCESSOR(t, securityLevel);
        SNMP_SET_ACCESSOR(t, paramName);


        NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
        target->Set(Session_symbol, t->GetFunction());
    }

    static v8::Handle<v8::Value> New(const v8::Arguments& args) {
        v8::HandleScope scope;
        if(1 < args.Length()) {
            return ThrowError("Must pass a arguments object to constructor.");
        }

        Session* session = new Session();
        session->Wrap(args.This());
        if (args.Length() > 0) {
            if(!args[0]->IsObject()) {
                return ThrowTypeError("Arguments type must be an object");
            }

            v8::Handle<v8::Object> obj = args[0]->ToObject();
            session->arguments_.version = to_int32(obj, "version", session->arguments_.version);
        }
        return args.This();
    }

    SNMP_ACCESSOR_DEFINE(Session, arguments_. , long,  version);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , int32,  retries);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , long,  timeout);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , ulong,  flags);
    SNMP_ACCESSOR_DEFINE_STRING_LENLESS(Session, arguments_. , peername);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , ushort,  remote_port);
    SNMP_ACCESSOR_DEFINE_STRING_LENLESS(Session, arguments_. , localname);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , ushort,  local_port);

    SNMP_ACCESSOR_DEFINE(Session, arguments_. , long,  sessid);
    SNMP_ACCESSOR_DEFINE_USTRING(Session, arguments_. , community, community_len);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , size_t,  rcvMsgMaxSize);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , size_t,  sndMsgMaxSize);

    SNMP_ACCESSOR_DEFINE(Session, arguments_. , uchar,  isAuthoritative);
    SNMP_ACCESSOR_DEFINE_USTRING(Session, arguments_. , contextEngineID, contextEngineIDLen);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , uint32,  engineBoots);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , uint32,  engineTime);
    SNMP_ACCESSOR_DEFINE_STRING(Session, arguments_. , contextName, contextNameLen);
    SNMP_ACCESSOR_DEFINE_USTRING(Session, arguments_. , securityEngineID, securityEngineIDLen);
    SNMP_ACCESSOR_DEFINE_STRING(Session, arguments_. , securityName, securityNameLen);
    SNMP_ACCESSOR_DEFINE_OID(Session, arguments_. , securityAuthProto, securityAuthProtoLen);
    SNMP_ACCESSOR_DEFINE_FIXED_USTRING(Session, arguments_. , securityAuthKey, securityAuthKeyLen, USM_AUTH_KU_LEN);
    SNMP_ACCESSOR_DEFINE_USTRING(Session, arguments_. , securityAuthLocalKey, securityAuthLocalKeyLen);
    SNMP_ACCESSOR_DEFINE_OID(Session, arguments_. , securityPrivProto, securityPrivProtoLen);
    SNMP_ACCESSOR_DEFINE_FIXED_USTRING(Session, arguments_. , securityPrivKey, securityPrivKeyLen, USM_PRIV_KU_LEN);
    SNMP_ACCESSOR_DEFINE_USTRING(Session, arguments_. , securityPrivLocalKey, securityPrivLocalKeyLen);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , int32,  securityModel);
    SNMP_ACCESSOR_DEFINE(Session, arguments_. , int32,  securityLevel);
    SNMP_ACCESSOR_DEFINE_STRING_LENLESS(Session, arguments_. , paramName);



    static v8::Handle<v8::Value> Open(const v8::Arguments& args) {
        v8::HandleScope scope;
        UNWRAP(Session, wrap, args.This());
        if(0 != wrap->session_) {
            return ThrowError("Session already opened.");
        }
        wrap->session_ = snmp_sess_open(&wrap->arguments_);
        if(0 == wrap->session_) {
            return ThrowError(snmp_api_errstring(wrap->arguments_.s_snmp_errno));

        }
        return v8::Undefined();
    }

    static v8::Handle<v8::Value> sendNativePdu(const v8::Arguments& args) {
        v8::HandleScope scope;
        UNWRAP(Session, wrap, args.This());
        if(0 != wrap->session_) {
            return ThrowError("Session already opened.");
        }

        if(2 != args.Length()) {
            return ThrowError("Must pass pdu and cb arguments to constructor.");
        }

        v8::Handle<v8::Object> cb = args[1]->ToObject();
        if (!cb->IsCallable()) {
            return ThrowError("Must pass pdu and cb arguments to constructor.");
        }

        UNWRAP(Pdu, pdu, args[0]->ToObject());

        std::auto_ptr<Callable> callable(new Callable());

        if(0 != snmp_sess_async_send(wrap->session_, pdu->native(),
                                     Callable::OnEvent, callable.get())) {
            return ThrowError(snmp_api_errstring(wrap->arguments_.s_snmp_errno));
        }
        callable.release();
        return v8::Undefined();
    }

    static v8::Handle<v8::Value> Close(const v8::Arguments& args) {
        v8::HandleScope scope;
        UNWRAP(Session, wrap, args.This());
        wrap->close();
        return v8::Undefined();
    }
};

#endif //   __session__cpp__
