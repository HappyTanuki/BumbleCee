#ifndef BUMBLEBEE_INCLUDE_CORE_BUMBLEBEE_H_
#define BUMBLEBEE_INCLUDE_CORE_BUMBLEBEE_H_

#include "precomp.h"

namespace bumblebee {

class BumbleBee {
 public:
  BumbleBee();
  ~BumbleBee();

 private:
  dpp::cluster cluster;
};

}  // namespace bumblebee

#endif