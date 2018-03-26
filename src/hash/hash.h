#ifndef RCIB_HASH_
#define RCIB_HASH_

class HashData {
public:
  HashData();
  ~HashData() {
  }

  char * _p;
  size_t _plen;
  char *_k;
  size_t _klen;
};

class HashRe : public rcib::Param {
public:
  typedef void(*Clean)(void *, base::WeakPtr<base::Thread>& thread);
  explicit HashRe(Clean f, base::WeakPtr<base::Thread> thr) :
    _data(nullptr),
    _len(0),
    _encoding(node::HEX) {
    _fclean = f;
    _thr = thr;
    if (thr.get()) {
      thr->IncComputational();
    }
  }

  virtual ~HashRe() {
    assert(_fclean);
    (*(_fclean))(_data, _thr);
  }
  uint8_t * _data;
  ssize_t _len;
  node::encoding _encoding;
  base::WeakPtr<base::Thread> _thr;
  Clean _fclean;
};

class HashHelper {
public:
  explicit HashHelper();
  //static
  static HashHelper* GetInstance();
  //static
  static void HashClean(void *data, base::WeakPtr<base::Thread>& thread);
  // sha
  void SHA(int type /*256|384|512*/, const HashData &data, rcib::async_req * req);
};

#endif
