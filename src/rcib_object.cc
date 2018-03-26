#include "rcib_object.h"
#include "ed25519/ed25519.h"
#include "hash/hash.h"

#define ONE ((char*)1)

namespace rcib {
  extern bool bterminating_;
}

extern base::LazyInstance<rcib::ArrayBufferAllocator> array_buffer_allocator_;
extern base::LazyInstance<rcib::furOfThread> furThread_;

namespace rcib {

  void* ArrayBufferAllocator::Allocate(size_t length) {
    void* data = AllocateUninitialized(length);
    return data == nullptr ? data : memset(data, 0, length);
  }
  void* ArrayBufferAllocator::AllocateUninitialized(size_t length) { return malloc(length); }
  void ArrayBufferAllocator::Free(void* data, size_t) { free(data); }

  // constructor
  RcibHelper::RcibHelper()
    :handle_(nullptr) {
  }
  //static
  RcibHelper* RcibHelper::GetInstance() {
    static RcibHelper This;
    return &This;
  }

  static void RunCallBack(async_req * req) {
    v8::Isolate* isolate = req->isolate;
    v8::HandleScope scope(isolate);
    // there is always at least one argument. "error"
    int argc = 1;

    v8::Local<v8::Value> argv[2];

    if (-1 == req->result || !req->out) {
      argv[0] = node::UVException(-1, req->error.c_str());
    } else if (ONE == req->out) {
      argv[0] = v8::Null(isolate);
      req->out = nullptr;
    } else {
      switch (req->w_t) {
      case TYPE_ED25519: {
          argv[0] = v8::Null(isolate);
          argc = 2;
          Ed25519Re *hre = reinterpret_cast<Ed25519Re *>(req->out);
          if (hre->_type == Ed25519Re::SIGN) {
            argv[1] = node::Encode(isolate, reinterpret_cast<char *>(hre->data), 64, node::encoding::BUFFER);
          } else {
            argv[1] = v8::Boolean::New(isolate, req->result);
          }
          delete hre;
          req->out = nullptr;  // should be set null
        }
        break;
      case TYPE_SHA: {
          argv[0] = v8::Null(isolate);
          argc = 2;
          HashRe *hre = reinterpret_cast<HashRe *>(req->out);
          argv[1] = node::Encode(isolate, reinterpret_cast<char *>(hre->_data), hre->_len, node::encoding::BUFFER);
          delete hre;
          req->out = nullptr;
        }
        break;
        default: {
          argv[0] = v8::Null(isolate);
        }
        break;
      }
    }

    v8::TryCatch try_catch(isolate);
    try_catch.SetVerbose(false);

    v8::Local<v8::Function> callback =
      v8::Local<v8::Function>::New(isolate, req->callback);
    callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);

    // cleanup
    req->callback.Reset();
    if (req->out) {
      free(req->out);
      req->out = nullptr;
    }

    delete req;

    if (try_catch.HasCaught()) {
      node::FatalException(isolate, try_catch);
    }
  }
  //static
  void RcibHelper::AfterAsync(uv_async_t* h) {
    RcibHelper::GetInstance()->PickFinished();

    async_req * req = nullptr;
    while (true) {
      req = RcibHelper::GetInstance()->Pop();
      if (!req)
        break;
      RunCallBack(req);
    }//end while
  }
  //static
  void RcibHelper::DoNopAsync(async_req* req) {
    req->result = 1;
    req->out = ONE;
  }
  //static
  void RcibHelper::EMark(async_req* req, std::string message) {
    req->out = nullptr;
    req->result = -1;
    req->error = message;
  }
  //static
  void RcibHelper::EMark2(async_req* req, std::string message) {
    if (req->out) {
      Param *hre = reinterpret_cast<Param *>(req->out);
      delete hre;
      req->out = nullptr;
    }
    req->result = -1;
    req->error = message;
  }
  //static
  void RcibHelper::init_async_req(async_req *req) {
    if (!req) {
      return;
    }
    req->finished = false;
    req->out = nullptr;
    req->result = 0;
    req->w_t = TYPE_START;
    RcibHelper::GetInstance()->Push(req);
  }

  void RcibHelper::Init() {
    array_buffer_allocator_.Get();
    //init this
    handle_ = new async_t_handle;
    uv_async_init(uv_default_loop(), (uv_async_t*)handle_, RcibHelper::AfterAsync);
  }

  void RcibHelper::Terminate() {
    bterminating_ = true;
  }

  void RcibHelper::Uv_Send(async_req* req, uv_async_t* h) {
    h = h ? h : (uv_async_t*)handle_;
    req->finished = true;
    uv_async_send(h);
  }

  //this function runs in main
  void RcibHelper::PickFinished() {
    std::list<async_req *> tmp_q;
    async_req * item = nullptr;
    while (!pending_queue_.empty()){
      item = pending_queue_.front();
      pending_queue_.pop_front();
      if (!item)
        continue;
      if (item->finished) {
        working_queue_.push_back(item);
      } else {
        tmp_q.push_back(item);
      }
    }// end while
    pending_queue_.swap(tmp_q);
    DCHECK_EQ(tmp_q.size(), 0);
  }// end func

} // end rcib
