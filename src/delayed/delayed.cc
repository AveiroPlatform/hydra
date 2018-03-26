#include "../rcib.h"
#include "delayed.h"
//constructor
DelayedHelper::DelayedHelper() {
}

//static
DelayedHelper* DelayedHelper::GetInstance() {
  static DelayedHelper This;
  return &This;
}

void DelayedHelper::DelayByMil(rcib::async_req * req) {
  CommonPross(req);
}

void DelayedHelper::DelayBySec(rcib::async_req * req) {
  CommonPross(req);
}

void DelayedHelper::DelayByMin(rcib::async_req * req) {
  CommonPross(req);
}

void DelayedHelper::DelayByHour(rcib::async_req * req) {
  CommonPross(req);
}

void DelayedHelper::CommonPross(rcib::async_req * req) {
  rcib::RcibHelper::DoNopAsync(req);
  rcib::RcibHelper::GetInstance()->Uv_Send(req, NULL);
}
