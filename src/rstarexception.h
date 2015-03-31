#ifndef RSTARTREE_RSTAREXCEPTION_H_
#define RSTARTREE_RSTAREXCEPTION_H_

#include <exception>
#include <string>

class RStarException : public std::exception {
  std::string msg_;
 public:
  const char * what() const throw () {
    return msg_.c_str();
  }
  RStarException(const std::string &msg) : msg_(msg) {
  }
};

#endif //RSTARTREE_RSTAREXCEPTION_H_
