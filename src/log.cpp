#include "log.h"
#include <string>

std::ostream& operator<<(std::ostream& os, const Log log_type)
{
    std::string out = "\033[1m\033[";
    switch (log_type) {
        case ERROR:
            out += "31m[ERROR]   ";
        break;
        case WARNING:
            out += "33m[WARNING] ";
        break;
        case INFO:
            out += "34m[INFO]    ";
        break;
        case DEBUG:
            out += "32m[DEBUG]   ";
        break;
    }

    out += "\033[0m";

    os << out;

    return os;
}
