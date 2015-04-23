#include "command.h"

std::map<std::string, int> Command::operations_ = {
    {"SELECT", 0},
    {"INSERT", 1},
    {"DELETE", 2},
    {"EXIT", 3}
};

std::string Command::String() {
    std::stringstream ss;
    ss << op_code_ << x1_ << x2_ << y1_ << y2_ << value_;
    return ss.str();
}

Command Command::Parse(std::string s) {
  Command ans;
  ans.op_code_ = -1;
  return ans;
}
