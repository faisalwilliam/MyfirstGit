#include "Time.h"
#include <iomanip>

using namespace std;

Time::Time(int h, int m) {
    hour = h + (m / 60);
    min = m % 60;
}

Time::Time() : hour(0), min(0) {}

Time Time::operator+(const Time& t) const {
    return Time(hour + t.hour, min + t.min);
}

ostream& operator<<(ostream& os, const Time& t) {
    os << t.hour << ":" << setfill('0') << setw(2) << t.min;
    return os;
}

double Time::toDecimal() {
    return hour + (double)min / 60;
}