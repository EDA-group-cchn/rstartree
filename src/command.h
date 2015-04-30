#ifndef RSTARTREE_COMMAND_H_
#define RSTARTREE_COMMAND_H_

#include <map>
#include <string>
#include <sstream>
#include <cmath>

class Command {
  int op_code_;
  double x1_, x2_, y1_, y2_;
  size_t value_;

  static std::map<std::string, int> operations_;
  static Command parse_result_;

  static std::string GetName(const std::string &s, size_t &pos);
  static bool GetChar(const std::string &s, size_t &pos, char c);
  static double GetDouble(const std::string &s, size_t &pos);
  static long GetValue(const std::string &s, size_t &pos);
  static void MoveSpaces(const std::string &s, size_t &pos);

 public:
  static Command Parse(const std::string &s);

  std::string String();

  int OpCode() {
    return op_code_;
  }
};


#endif //RSTARTREE_COMMAND_H_
