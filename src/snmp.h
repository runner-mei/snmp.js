#ifndef _snmp_js_h
#define _snmp_js_h  1


#ifndef _WIN32_WINNT
# define _WIN32_WINNT   0x0502
#endif


#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <net-snmp/net-snmp-config.h>


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>

#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <node_version.h>

#include <net-snmp/utilities.h>
#include <net-snmp/net-snmp-includes.h>


#define ThrowError(x) v8::ThrowException(v8::Exception::Error(v8::String::New(x)))

#define ThrowRangeError(x) v8::ThrowException(v8::Exception::RangeError(v8::String::New(x)))
#define ThrowReferenceError(x) v8::ThrowException(v8::Exception::ReferenceError(v8::String::New(x)))
#define ThrowSyntaxError(x) v8::ThrowException(v8::Exception::SyntaxError(v8::String::New(x)))
#define ThrowTypeError(x) v8::ThrowException(v8::Exception::TypeError(v8::String::New(x)))


inline v8::Handle<v8::Value> newIntArray(int v1) {
	v8::Handle<v8::Array> result = v8::Array::New(1);
	result->Set(0, v8::Integer::New(v1));
	return result;
}

inline v8::Handle<v8::Value> newIntArray(int v1, int v2) {
	v8::Handle<v8::Array> result = v8::Array::New(2);
	result->Set(0, v8::Integer::New(v1));
	result->Set(1, v8::Integer::New(v2));
	return result;
}

inline v8::Handle<v8::Value> newIntArray(int v1, int v2, int v3) {
	v8::Handle<v8::Array> result = v8::Array::New(3);
	result->Set(0, v8::Integer::New(v1));
	result->Set(1, v8::Integer::New(v2));
	result->Set(2, v8::Integer::New(v3));
	return result;
}

inline v8::Handle<v8::Value> newIntArray(int v1, int v2
										, int v3, int v4) {
	v8::Handle<v8::Array> result = v8::Array::New(4);
	result->Set(0, v8::Integer::New(v1));
	result->Set(1, v8::Integer::New(v2));
	result->Set(2, v8::Integer::New(v3));
	result->Set(3, v8::Integer::New(v4));
	return result;
}

inline v8::Handle<v8::Value> newIntArray(int v1, int v2
								, int v3, int v4, int v5) {
	v8::Handle<v8::Array> result = v8::Array::New(5);
	result->Set(0, v8::Integer::New(v1));
	result->Set(1, v8::Integer::New(v2));
	result->Set(2, v8::Integer::New(v3));
	result->Set(3, v8::Integer::New(v4));
	result->Set(4, v8::Integer::New(v5));
	return result;
}

inline v8::Handle<v8::Value> newIntArray(int v1, int v2
						, int v3, int v4, int v5, int v6) {
	v8::Handle<v8::Array> result = v8::Array::New(6);
	result->Set(0, v8::Integer::New(v1));
	result->Set(1, v8::Integer::New(v2));
	result->Set(2, v8::Integer::New(v3));
	result->Set(3, v8::Integer::New(v4));
	result->Set(4, v8::Integer::New(v5));
	result->Set(5, v8::Integer::New(v6));
	return result;
}

inline v8::Handle<v8::Value> newIntArray(int v1, int v2
				, int v3, int v4, int v5, int v6, int v7) {
	v8::Handle<v8::Array> result = v8::Array::New(7);
	result->Set(0, v8::Integer::New(v1));
	result->Set(1, v8::Integer::New(v2));
	result->Set(2, v8::Integer::New(v3));
	result->Set(3, v8::Integer::New(v4));
	result->Set(4, v8::Integer::New(v5));
	result->Set(5, v8::Integer::New(v6));
	result->Set(6, v8::Integer::New(v7));
	return result;
}

inline v8::Handle<v8::Value> from_uint64(uint64_t value) {
	if(value > 0)
		return v8::Integer::NewFromUnsigned(value);
	return v8::Integer::New(value);
}

inline v8::Handle<v8::Value> from_int64(int64_t value) {
    if(value > 0)
        return v8::Integer::NewFromUnsigned(value);
    return v8::Integer::New(value);
}

inline v8::Handle<v8::Value> from_ulong(u_long value) {
    return v8::Integer::NewFromUnsigned(value);
}

inline v8::Handle<v8::Value> from_long(long value) {
    return v8::Integer::New(value);
}

inline v8::Handle<v8::Value> from_uint32(uint32_t value) {
    return v8::Integer::NewFromUnsigned(value);
}

inline v8::Handle<v8::Value> from_int32(int32_t value) {
    return v8::Integer::New(value);
}

inline v8::Handle<v8::Value> from_uchar(u_char* value, size_t len) {
    return v8::String::New((const char*)value, len);
}

inline v8::Handle<v8::Value> from_oid(oid* name, size_t len) {
	v8::Handle<v8::Array> ret = v8::Array::New(len);
    for(size_t i = 0; i < len; ++ i) {
		ret->Set(i, v8::Int32::New(name[i]));
	}
	return ret;
}

inline int to_int32(v8::Handle<v8::Object>& obj, const char* key
											, int defaultValue) {
    v8::Handle<v8::Value> result = obj->Get(v8::String::New(key));
	if(!result->IsNumber()){
		return defaultValue;
	}
	return result->Int32Value();
}

inline int to_uint32(v8::Handle<v8::Object>& obj, const char* key
										, uint32_t defaultValue) {
    v8::Handle<v8::Value> result = obj->Get(v8::String::New(key));
	if(!result->IsNumber()){
		return defaultValue;
	}
	return result->Uint32Value();
}

inline int64_t to_int64(v8::Handle<v8::Object>& obj, const char* key
											, int64_t defaultValue) {
    v8::Handle<v8::Value> result = obj->Get(v8::String::New(key));
	if(!result->IsNumber()){
		return defaultValue;
	}
	return result->IntegerValue();
}

inline uint64_t to_uint64(v8::Handle<v8::Object>& obj, const char* key
											, uint64_t defaultValue) {
    v8::Handle<v8::Value> result = obj->Get(v8::String::New(key));
	if(!result->IsNumber()){
		return defaultValue;
	}
	return result->IntegerValue();
}

inline oid* to_oid(v8::Handle<v8::Value>& s, oid* out, size_t* len) {
	if(s->IsString()){
		v8::String::Utf8Value u8(s->ToString());
		oid* buf = out;
		while(NULL == snmp_parse_oid(*u8, buf, len)){
		    if(buf != out) {
		       free(buf);
		    }

			if((*len) > MAX_OID_LEN) {
				return 0;
			}

			(*len) *= 2;
			buf = (oid*)malloc(sizeof(oid) * *len);
		}
		return buf;
	} else if(s->IsArray()) {
		v8::Handle<v8::Array> ints = v8::Handle<v8::Array>::Cast(s);
		if(ints->Length() > *len) {
			out = (oid*)malloc(sizeof(oid) * ints->Length() + 1);
		}
		
		(*len) = ints->Length();

		for(int i =0; i < (*len); ++ i) {
			out[i] = ints->Get(i)->Int32Value();
		}
		return out;
	} else {
		return 0;
	}
}

#define UNWRAP(type, wrap, ref) type* wrap = ObjectWrap::Unwrap<type>(ref)


#define SNMP_DEFINE_CONSTANT_VALUE(target, constant_name, constant_value)         \
  (target)->Set(v8::String::NewSymbol(#constant_name),                            \
                constant_value,                                                   \
                static_cast<v8::PropertyAttribute>(                               \
                    v8::ReadOnly|v8::DontDelete))


#define SNMP_DEFINE_SYMBOL(name) static v8::Persistent<v8::String> name##_symbol;


#define SNMP_SET_ACCESSOR(target, name)                                           \
    name##_symbol = NODE_PSYMBOL(#name);                                          \
	target->PrototypeTemplate()->SetAccessor(name##_symbol                        \
	                                            , Get_##name, Set_##name);        \


#define SNMP_SET_READONLY_ACCESSOR(target, name)                                  \
    name##_symbol = NODE_PSYMBOL(#name);                                          \
	target->PrototypeTemplate()->SetAccessor(name##_symbol, Get_##name, 0);       \
	

#define SNMP_ACCESSOR_DEFINE_GET(this_type, value_type,  name)                    \
	static v8::Handle<v8::Value> Get_##name(v8::Local<v8::String> propertyName    \
                                        , const v8::AccessorInfo& info) {         \
		v8::HandleScope scope;                                                    \
		UNWRAP(this_type, wrap, info.This());                                     \
		return scope.Close(v8::value_type::New(wrap->native_->name));             \
	}


#define SNMP_ACCESSOR_DEFINE_SET(this_type, value_type,  name)                    \
    static void Set_##name(v8::Local<v8::String> propertyName                     \
           , v8::Local<v8::Value> value, const v8::AccessorInfo& info) {          \
		v8::HandleScope scope;                                                    \
		UNWRAP(this_type, wrap, info.This());                                     \
        wrap->native_->name = value->value_type##Value();                         \
	}


#define SNMP_ACCESSOR_DEFINE_GET_OID(this_type, name, len)                        \
	static v8::Handle<v8::Value> Get_##name(v8::Local<v8::String> propertyName    \
                                        , const v8::AccessorInfo& info) {         \
		v8::HandleScope scope;                                                    \
		UNWRAP(this_type, wrap, info.This());                                     \
		v8::Handle<v8::Array> ret = v8::Array::New(wrap->native_->len);           \
        for(size_t i = 0; i < wrap->native_->len; ++ i) {                         \
			ret->Set(i, v8::Int32::New(wrap->native_->name[i]));                  \
		}                                                                         \
		return scope.Close(ret);                                                  \
	}


#define SNMP_ACCESSOR_DEFINE_SET_OID(this_type, name, len)                        \
    static void Set_##name(v8::Local<v8::String> propertyName                     \
           , v8::Local<v8::Value> value, const v8::AccessorInfo& info) {          \
		v8::HandleScope scope;                                                    \
		UNWRAP(this_type, wrap, info.This());                                     \
		oid* new_value = wrap->native_->name;                                     \
        size_t new_len = wrap->native_->len;                                      \
		new_value = to_oid(value, new_value, &new_len);                           \
		if(new_value != wrap->native_->name) free(wrap->native_->name);           \
        wrap->native_->name = new_value;                                          \
        wrap->native_->len = new_len;                                             \
	}


#define SNMP_ACCESSOR_DEFINE_GET_STRING(char_type, this_type, name, len)          \
	static v8::Handle<v8::Value> Get_##name(v8::Local<v8::String> propertyName    \
                                           , const v8::AccessorInfo& info) {      \
		v8::HandleScope scope;                                                    \
		UNWRAP(this_type, wrap, info.This());                                     \
		return scope.Close(v8::String::New((const char*)wrap->native_->name       \
		                                               , wrap->native_->len));    \
	}


#define SNMP_ACCESSOR_DEFINE_SET_STRING(char_type, this_type, name, len)          \
    static void Set_##name(v8::Local<v8::String> propertyName                     \
                  , v8::Local<v8::Value> value, const v8::AccessorInfo& info) {   \
		v8::HandleScope scope;                                                    \
		UNWRAP(this_type, wrap, info.This());                                     \
		v8::String::Utf8Value u8(value->ToString());                              \
		if(0 != wrap->native_->name) free(wrap->native_->name);                   \
        wrap->native_->name = (char_type*)strdup(*u8);                            \
        wrap->native_->len = u8.length();                                         \
	}


#define SNMP_ACCESSOR_DEFINE_GET_USTRING(this_type, name, len)                    \
	static v8::Handle<v8::Value> Get_##name(v8::Local<v8::String> propertyName    \
                                              , const v8::AccessorInfo& info) {   \
		v8::HandleScope scope;                                                    \
		UNWRAP(Pdu, wrap, info.This());                                           \
		return node::Buffer::New((char*)wrap->native_->name                       \
		              , sizeof(u_char)*wrap->native_->len)->handle_;              \
	}

#define SNMP_ACCESSOR_DEFINE_SET_USTRING(this_type, name, len)                    \
    static void Set_##name(v8::Local<v8::String> propertyName                     \
                   , v8::Local<v8::Value> value, const v8::AccessorInfo& info) {  \
		v8::HandleScope scope;                                                    \
		UNWRAP(Pdu, wrap, info.This());                                           \
		UNWRAP(node::Buffer, buffer, value->ToObject());                          \
		                                                                          \
		if(0 == wrap->native_->name                                               \
		  || wrap->native_->len < node::Buffer::Length(buffer)) {                 \
		    free(wrap->native_->name);                                            \
            wrap->native_->name = (u_char*)malloc(node::Buffer::Length(buffer) + 4);  \
		}                                                                         \
                                                                                  \
		memcpy(wrap->native_->name, node::Buffer::Data(buffer)                    \
		           , node::Buffer::Length(buffer));                               \
        wrap->native_->len = node::Buffer::Length(buffer);                        \
	}


#define SNMP_ACCESSOR_DEFINE(this_type, value_type,  name)                        \
  SNMP_ACCESSOR_DEFINE_SET(this_type, value_type,  name)                          \
  SNMP_ACCESSOR_DEFINE_GET(this_type, value_type,  name)      


#define SNMP_ACCESSOR_DEFINE_OID(this_type, name, len)                            \
  SNMP_ACCESSOR_DEFINE_SET_OID(this_type, name, len)                              \
  SNMP_ACCESSOR_DEFINE_GET_OID(this_type, name, len)      
	

#define SNMP_ACCESSOR_DEFINE_USTRING(this_type, name, len)                        \
  SNMP_ACCESSOR_DEFINE_SET_USTRING(this_type, name, len)                          \
  SNMP_ACCESSOR_DEFINE_GET_USTRING(this_type, name, len)      


#define SNMP_ACCESSOR_DEFINE_STRING(this_type, name, len)                         \
  SNMP_ACCESSOR_DEFINE_SET_STRING(char, this_type, name, len)                     \
  SNMP_ACCESSOR_DEFINE_GET_STRING(char, this_type, name, len)        


#endif // _snmp_js_h