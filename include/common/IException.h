#ifndef IEXCEPTION_H_
#define IEXCEPTION_H_

#include <stdexcept>
#include <memory>
#include <string>
#include <list>

#include <common/shd_ptr_mod.h>

namespace Tools {

  class IException : public std::runtime_error, public Tools::SharedPointerMod<IException> {
  private:
    using List = std::list<std::string>;

    List m_childs = {};

  public:
    static std::runtime_error createBackTrack(const std::string& file, const int& line,
                                              const std::string& func, const std::exception& child,
                                              const std::string& msg = "");

    IException(const std::string& msg = "");

    IException(const IException&) = default;
    IException(IException&&) = default;
    IException& operator=(const IException&) = default;
    IException& operator=(IException&&) = default;

    ~IException() = default;

    static Ptr create(const std::string& msg = "");

    bool isChildsEmpty() const noexcept;
    void add(const std::exception& child);
    void add(const std::string& child);

    void checkSelf();

    std::string toString() const;
  };

  #define IExceptionBacktrackMsg(child, msg) IException::createBackTrack(__FILE__, __LINE__, __func__, child, msg)
  #define IExceptionBacktrack(child) IException::createBackTrack(__FILE__, __LINE__, __func__, child)

}

#endif  // IEXCEPTION_H_
