//
// Created by alculquicondor on 3/29/15.
//

#ifndef RSTARTREE_RSTAREXCEPTION_H
#define RSTARTREE_RSTAREXCEPTION_H

#include <exception>

class RStarException : public std::exception {
    char const *msg;
public:
    const char * what() const throw () {
        return msg;
    }
    RStarException(char const *msg) : msg(msg) {
    }
};

#endif //RSTARTREE_RSTAREXCEPTION_H
