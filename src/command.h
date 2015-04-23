#ifndef RSTARTREE_COMMAND_H_
#define RSTARTREE_COMMAND_H_

#include <map>
#include <string>
#include <sstream>

class Command {
  int op_code_;
  double x1_, x2_, y1_, y2_;
  size_t value_;
  static std::map<std::string, int> operations_;

 public:
  static Command Parse(const std::string s);

  std::string String();

  int OpCode() {
    return op_code_;
  }
};


#endif //RSTARTREE_COMMAND_H_
