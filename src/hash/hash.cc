#include "../rcib.h"
#include "hash.h"
#include "sha/sha.h"

HashData::HashData() :_p(nullptr), _k(nullptr), _plen(-1), _klen(-1) {
}

HashHelper::HashHelper() {
}

//static
HashHelper* HashHelper::GetInstance() {
  static HashHelper This;
  return &This;
}

//static
void HashHelper::HashClean(void *data, base::WeakPtr<base::Thread>& thread) {
  if (data) free(data);
  if (thread.get()) thread->DecComputational();
}

void HashHelper::SHA(int type, const HashData &data, rcib::async_req * req) {
  if (256 == type || 384 == type || 512 == type) {
    const char *p = nullptr;
    size_t plen;
    p = data._p;
    plen = data._plen;
    HashRe *hre = reinterpret_cast<HashRe *>(req->out);
    hre->_len = (type / 8) * sizeof(uint8_t);
    hre->_data = (uint8_t *)malloc(hre->_len);
    if (256 == type) {
      sha256((uint8_t *)p, (unsigned int)plen, hre->_data);
    } else if (512 == type) {
      sha512((uint8_t *)p, (unsigned int)plen, hre->_data);
    } else {
      sha384((uint8_t *)p, (unsigned int)plen, hre->_data);
    }
    req->result = hre->_len;
  } else {
    rcib::RcibHelper::EMark2(req, std::string("type should be 256/384/512"));
  }
  rcib::RcibHelper::GetInstance()->Uv_Send(req, NULL);
}
