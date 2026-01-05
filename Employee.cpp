#include "Employee.h"
#include <sstream>

using namespace std;

Employee::Employee(string id, string n, double h, double t) {
    this->id = id;
    name = n;
    hourlyWage = h;
    taxRate = t;
}

void Employee::setDailyWorkTime(int day, int h, int m) {
    if (day >= 0 && day < 5) {
        dailyWorkTime[day] = Time(h, m);
    }
}

void Employee::salary() {
    Time totalTime;
    for (int i = 0; i < 5; i++) {
        totalTime = totalTime + dailyWorkTime[i];
    }
    weeklyWTime = totalTime;
    double totalHours = weeklyWTime.toDecimal();
    if (totalHours > 40) {
        weeklyWage = (40 * hourlyWage) + ((totalHours - 40) * hourlyWage * 1.5);
    } else {
        weeklyWage = totalHours * hourlyWage;
    }
    tax = weeklyWage * (taxRate / 100.0);
    netWage = weeklyWage - tax;
    control = totalHours;
}

double Employee::calculateAnnualSalary() {
    return weeklyWage * 52;
}

double Employee::getWeeklyWage() {
    return weeklyWage;
}

double Employee::getTax() {
    return tax;
}

double Employee::getNetWage() {
    return netWage;
}

double Employee::getTotalHours() {
    return weeklyWTime.toDecimal();
}

string Employee::output() {
    salary();
    stringstream ss;
    ss << "Employee ID: " << id << "\n";
    ss << "Name: " << name << "\n";
    ss << "Hourly Wage: " << hourlyWage << "\n";
    ss << "Weekly Wage: " << weeklyWage << "\n";
    ss << "Tax (" << taxRate << "%): " << tax << "\n";
    ss << "Net Wage: " << netWage << "\n";
    ss << "Annual Salary: " << calculateAnnualSalary() << "\n";
    ss << "Weekly Work Time: " << weeklyWTime << "\n";

    double totalHours = weeklyWTime.toDecimal();
    if (totalHours > 40) {
        ss << "Overtime Hours: " << (totalHours - 40) << "\n";
    } else {
        ss << "Overtime Hours: 0\n";
    }
    return ss.str();
}