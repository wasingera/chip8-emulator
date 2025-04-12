#ifndef LOG_H
#define LOG_H

#include <iostream>

enum Log {
    ERROR,
    WARNING,
    INFO,
    DEBUG
};

std::ostream& operator<<(std::ostream& os, const Log log_type);

#endif
