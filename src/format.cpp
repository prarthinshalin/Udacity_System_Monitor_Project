#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
    std::string hours, mins, secs;
    long h, m, s;
    h = seconds/3600;
    m = (seconds%3600)/60;
    s = (seconds%3600)%60;

    if(h < 10) { hours = "0" + std::to_string(h); }
    else { hours = std::to_string(h); }

    if(m < 10) { mins = "0" + std::to_string(m); }
    else { mins = std::to_string(m); }

    if(s < 10) { secs = "0" + std::to_string(s); }
    else { secs = std::to_string(s); }

    return (hours + ":" + mins + ":" + secs);
}