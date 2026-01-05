#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
#include "Time.h"

class Employee {
    std::string id;
    std::string name;
    double hourlyWage;
private:
    Time weeklyWTime;
    Time dailyWorkTime[5];
    double weeklyWage;
    double tax;
    double taxRate;
    double netWage;
    double control;
public:
    Employee(std::string id, std::string n, double h, double t);
    void setDailyWorkTime(int day, int h, int m);
    void salary();
    double calculateAnnualSalary();
    double getWeeklyWage();
    double getTax();
    double getNetWage();
    double getTotalHours();
    std::string output();
};

#endif