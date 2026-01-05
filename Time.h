#ifndef TIME_H
#define TIME_H

#include <iostream>

class Time {
    int hour;
    int min;
public:
    Time(int h, int m);
    Time();
    Time operator+(const Time& t) const;
    friend std::ostream& operator<<(std::ostream& os, const Time& t);
    double toDecimal();
};

#endif