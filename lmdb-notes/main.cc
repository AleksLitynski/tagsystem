

#include <node.h>
#include "lmdb/libraries/liblmdb/lmdb.h"

namespace main {

    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;

    void Method(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
    }

    void init(Local<Object> exports) {
        NODE_SET_METHOD(exports, "hello", Method);
    }

    NODE_MODULE(main, init)

    /*
     * document db
     *  id -> doc
     * forward index db
     *  id -> [tags]
     * inverted index db
     *  tag -> base 8 id tree -> id
     *
     * insert doc (doc -> id) 
     * add tag to id (tag, id -> void) 
     * remove tag from id (id -> void)
     * remove all for id (id -> void)
     *
     * itterate tag union ([tags] -> [ids]) (stream or itterable or something)
     *
     */
}
