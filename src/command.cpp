#include "command.h"

std::map<std::string, int> Command::operations_ = {
    {"select", 0},
    {"insert", 1},
    {"delete", 2},
    {"exit", 3}
};

std::string Command::String() {
  std::stringstream ss;
  ss << op_code_;
  if (op_code_ != 3 and op_code_ != -1)
    ss << ' ' << x1_ << ' ' << x2_ <<
        ' ' << y1_ << ' ' << y2_ <<
        ' ' << value_;
  return ss.str();
}

Command Command::parse_result_;

#include <iostream>
Command Command::Parse(const std::string &s) {
  size_t pos = 0;
  parse_result_.op_code_ = -1;

  std::string command_name = GetName(s, pos);
  std::map<std::string, int>::iterator op_it = operations_.find(command_name);
  if (op_it != operations_.end())
    parse_result_.op_code_ = op_it->second;
  if (parse_result_.op_code_ == -1)
    return parse_result_;

  if (parse_result_.op_code_ == 3) { // EXIT
    MoveSpaces(s, pos);
    if (pos != s.size())
      parse_result_.op_code_ = -1;
    return parse_result_;
  }

  if (not GetChar(s, pos, '(')) {
    parse_result_.op_code_ = -1;
    return parse_result_;
  }

  parse_result_.x1_ = GetDouble(s, pos);
  if (isnan(parse_result_.x1_) or not GetChar(s, pos, ',')) {
    parse_result_.op_code_ = -1;
    return parse_result_;
  }

  parse_result_.x2_ = GetDouble(s, pos);
  if (isnan(parse_result_.x2_) or parse_result_.x2_ < parse_result_.x1_ or
      not GetChar(s, pos, ',')) {
    parse_result_.op_code_ = -1;
    return parse_result_;
  }

  parse_result_.y1_ = GetDouble(s, pos);
  if (isnan(parse_result_.y1_) or not GetChar(s, pos, ',')) {
    parse_result_.op_code_ = -1;
    return parse_result_;
  }

  parse_result_.y2_ = GetDouble(s, pos);
  if (isnan(parse_result_.y2_) or parse_result_.y2_ < parse_result_.y1_ or
      not GetChar(s, pos, ')')) {
    parse_result_.op_code_ = -1;
    return parse_result_;
  }

  if (parse_result_.op_code_ == 0) { // SELECT
    MoveSpaces(s, pos);
    if (pos != s.size())
      parse_result_.op_code_ = -1;
    return parse_result_;
  }

  long tmp = GetValue(s, pos);
  MoveSpaces(s, pos);
  if (tmp == -1 or pos != s.size())
    parse_result_.op_code_ = -1;
  else
    parse_result_.value_ = tmp;
  return parse_result_;
}

std::string Command::GetName(const std::string &s, size_t &pos) {
  MoveSpaces(s, pos);
  if (pos == s.size())
    return "";
  size_t npos = pos;
  while (npos < s.size() and isalnum(s[npos]))
    ++npos;
  std::string res = s.substr(pos, npos - pos);
  pos = npos;
  for (char &c : res)
    c = tolower(c);
  return res;
}

bool Command::GetChar(const std::string &s, size_t &pos, char c) {
  MoveSpaces(s, pos);
  if (pos == s.size() or s[pos] != c)
    return false;
  ++pos;
  return true;
}

double Command::GetDouble(const std::string &s, size_t &pos) {
  MoveSpaces(s, pos);
  char *cpos;
  double ans = strtod(s.c_str() + pos, &cpos);
  size_t npos = cpos - s.c_str();
  if (npos == pos)
    return nan("");
  pos = npos;
  return ans;
}

long Command::GetValue(const std::string &s, size_t &pos) {
  MoveSpaces(s, pos);
  char *cpos;
  long ans = strtoul(s.c_str() + pos, &cpos, 10);
  size_t npos = cpos - s.c_str();
  if (npos == pos)
    return -1;
  pos = npos;
  return ans;
}

void Command::MoveSpaces(const std::string &s, size_t &pos) {
  while (pos < s.size() and isspace(s[pos]))
    ++pos;
}
