#ifndef SHD_PTR_MOD_H_
#define SHD_PTR_MOD_H_

#include <memory>

namespace Tools {

template<typename T> 
class SharedPointerMod {
 public:
  using Ptr = std::shared_ptr<T>;
  using CnstPtr = std::shared_ptr<const T>;
};

}

#endif  // SHD_PTR_MOD_H_
