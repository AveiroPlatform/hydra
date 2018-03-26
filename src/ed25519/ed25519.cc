#include "../rcib.h"
#include "ed25519.h"

//constructor
Ed25519Data::Ed25519Data() {
  _msg = nullptr;
  _privateKey = nullptr;
  _seed = nullptr;
  _mlen = 0;
}

// constructor
Ed25519Helper::Ed25519Helper() {
}
//static
Ed25519Helper* Ed25519Helper::GetInstance() {
  static Ed25519Helper This;
  return &This;
}

void Ed25519Helper::Sign(const Ed25519Data &data, rcib::async_req * req) {
  unsigned char publicKeyData[32];
  unsigned char privateKeyData[64];
  unsigned char * privateKey;
  Ed25519Re *hre = reinterpret_cast<Ed25519Re *>(req->out);
  if (data._seed) {
    for (int i = 0; i < 32; ++i) {
      privateKeyData[i] = data._seed[i];
    }
    crypto_sign_keypair(publicKeyData, privateKeyData);
    privateKey = privateKeyData;
  } else {
    privateKey = data._privateKey;
  }
  unsigned long long sigLen = data._mlen + 64;
  unsigned char *signatureMessageData = (unsigned char*)malloc(sigLen);
  crypto_sign(signatureMessageData, &sigLen, data._msg, data._mlen, privateKey);
  for (int i = 0; i < 64; ++i) {
    hre->data[i] = signatureMessageData[i];
  }
  free(signatureMessageData);
  req->result = 64;
  rcib::RcibHelper::GetInstance()->Uv_Send(req, NULL);
}

void Ed25519Helper::Verify(const Ed25519Data& data, rcib::async_req * req) {
  bool relt = (crypto_sign_verify(data._seed, data._msg, data._mlen, data._privateKey) == 0);
  req->result = relt ? 1 : 0;
  rcib::RcibHelper::GetInstance()->Uv_Send(req, NULL);
}
