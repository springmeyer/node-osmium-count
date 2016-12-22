
/*

Port of https://github.com/osmcode/libosmium/blob/master/examples/osmium_count.cpp

*/

#include "count.hpp"

#include <cstdint>  // for std::uint64_t
#include <cstdlib>  // for std::exit
#include <iostream> // for std::cout, std::cerr

// Allow any format of input files (XML, PBF, ...)
#include <osmium/io/pbf_input.hpp>

// We want to use the handler interface
#include <osmium/handler.hpp>

// Utility class gives us access to memory usage information
#include <osmium/util/memory.hpp>

// For osmium::apply()
#include <osmium/visitor.hpp>


// Handler derive from the osmium::handler::Handler base class. Usually you
// overwrite functions node(), way(), and relation(). Other functions are
// available, too. Read the API documentation for details.
struct CountHandler : public osmium::handler::Handler {

    std::uint64_t nodes     = 0;
    std::uint64_t ways      = 0;
    std::uint64_t relations = 0;

    // This callback is called by osmium::apply for each node in the data.
    void node(const osmium::Node&) noexcept {
        ++nodes;
    }

    // This callback is called by osmium::apply for each way in the data.
    void way(const osmium::Way&) noexcept {
        ++ways;
    }

    // This callback is called by osmium::apply for each relation in the data.
    void relation(const osmium::Relation&) noexcept {
        ++relations;
    }

}; // struct CountHandler

inline void CallbackError(std::string message, v8::Local<v8::Function> callback) {
    v8::Local<v8::Value> argv[1] = { Nan::Error(message.c_str()) };
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callback, 1, argv);
}

/**
 * Count the number of items in an osm.pbf file
 *
 * @name count
 * @param {String} path to input pbf to read
 * @param {Function} callback - called when work is done, passed `err` or `null` as first arg, and an object as second arg with counts of osm features
 * @example
 *
 * var osmium = require('osmium_count');
 * osmium.count("input.osm.pbf", function(err, results) {
 *   if (err) throw err;
 *   console.log(results);
 * });
 *
 */
class AsyncBaton {
public:
    uv_work_t request; // required
    // callback function type (will stay alive until you say it can be destroyed)
    Nan::Persistent<v8::Function> cb;
    std::string error_name;
    std::string input_pbf;
    CountHandler handler;
};

NAN_METHOD(count) {
    // CALLBACK: ensure callback is a function
    v8::Local<v8::Value> callback_val = info[info.Length()-1];
    if (!callback_val->IsFunction()) {
        Nan::ThrowError("last argument must be a callback function");
        return;
    }
    v8::Local<v8::Function> callback = callback_val.As<v8::Function>();

    // check first argument, should be a path as string
    v8::Local<v8::Value> input_val = info[0];
    if (input_val->IsNull() ||
        input_val->IsUndefined() ||
        !input_val->IsString()) {
        CallbackError("first arg must be a String", callback);
        return;
    }

    // set up the baton to pass into our threadpool
    AsyncBaton* baton = new AsyncBaton();
    baton->input_pbf = *v8::String::Utf8Value(input_val->ToString());
    baton->request.data = baton;
    baton->cb.Reset(callback);
    uv_queue_work(uv_default_loop(), &baton->request, AsyncCount, (uv_after_work_cb)AfterCount);
    return;
}

void AsyncCount(uv_work_t* req) {
    AsyncBaton* baton = static_cast<AsyncBaton*>(req->data);
    try {

        osmium::io::File input_file{baton->input_pbf};
        osmium::io::Reader reader{input_file};

        // Create an instance of our own CountHandler and push the data from the
        // input file through it.
        osmium::apply(reader, baton->handler);

        // You do not have to close the Reader explicitly, but because the
        // destructor can't throw, you will not see any errors otherwise.
        reader.close();
    } catch (std::exception const& ex) {
        baton->error_name = ex.what();
    }
}

void AfterCount(uv_work_t* req) {
    Nan::HandleScope scope;

    AsyncBaton* baton = static_cast<AsyncBaton*>(req->data);

    if (!baton->error_name.empty()) {
        v8::Local<v8::Value> argv[1] = { Nan::Error(baton->error_name.c_str()) };
        Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(baton->cb), 1, argv);
    } else { // no errors, lets return data
        v8::Local<v8::Object> results = Nan::New<v8::Object>();
        results->Set(Nan::New("ways").ToLocalChecked(), Nan::New<v8::Number>(baton->handler.ways));
        results->Set(Nan::New("nodes").ToLocalChecked(), Nan::New<v8::Number>(baton->handler.nodes));
        results->Set(Nan::New("relations").ToLocalChecked(), Nan::New<v8::Number>(baton->handler.relations));
        v8::Local<v8::Value> argv[2] = { Nan::Null(),
                                         results
                                       };
        Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(baton->cb), 2, argv);
    }

    // Release, mark as garbage collectible
    baton->cb.Reset();
    delete baton;
}
