
#include <IException.h>

namespace Tools {

  std::runtime_error IException::createBackTrack(const std::string& file, const int& line,
                                                 const std::string& func, const std::exception& child,
                                                 const std::string& msg) {
    const std::string pathSignature = "/src/";

    std::string tmp = child.what();

    const std::string search = "\n  ";
    const std::string replace = "\n    ";
    std::string::size_type n = 0;
    while ((n = tmp.find(search, n)) != std::string::npos) {
      tmp.replace( n, search.length(), replace);
      n += replace.length();
    }

    return std::runtime_error(
      file.substr(file.find(pathSignature) + pathSignature.length()) + ":" +
      std::to_string(line) + " in " + func + (msg.empty() ? "" : " - " + msg) +
      "\n  " + tmp
    );
  }

  IException::IException(const std::string& msg) : runtime_error(msg) {};

  IException::Ptr IException::create(const std::string& msg) {
    return IException::Ptr(new IException(msg));
  }

  bool IException::isChildsEmpty() const noexcept {
    return m_childs.empty();
  }

  void IException::add(const std::exception& child) {
    m_childs.emplace_back(child.what());
  }

  void IException::add(const std::string& child) {
    m_childs.emplace_back(child);
  }

  void IException::checkSelf() {
    if (isChildsEmpty()) {
      return;
    }

    throw std::runtime_error(toString());
  }

  std::string IException::toString() const {
    std::string msg = what();
    for (const auto& e : m_childs) {
      msg += std::string("\n  ") + e;
    }
    return msg;
  }

}