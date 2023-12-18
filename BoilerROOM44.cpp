    #include <iostream>
    #include <string>
    #include <conio.h>
    #include <iomanip>
    using namespace std;
    class Time {
	int hour;
	int min;
    public:
	Time(int h, int m) {
	hour = h;
	min = m;
	}
	Time() = default;
	Time sum(Time t) {
	int totalMin = min + t.min;
	int carry = totalMin / 60;
	double sumMin = totalMin % 60;
	double totalHour = hour + t.hour + carry;
	return Time(totalHour, sumMin);
	}
	double toDecimal() {
	return hour + (double)min / 60;
	}
    };
    class Employee {
	string name;
	double hourlyWage;
	double WeeklyWage;
	double WeeklyWorkTime;
    private:
	Time weeklyWTime;
	Time dailyWorkTime[5];
	double weeklyWage;
	double control;
    public:
	Employee(string n, double h)
	{
	name = n;
	hourlyWage = h;

	}
	void salary() {
	double totalWage = 0;
	double totalWorkTime = 0;
	for (int i = 0; i < 5; i++) {
	totalWorkTime += dailyWorkTime[i].toDecimal();
	}
	totalWage = totalWorkTime * hourlyWage;
	weeklyWage = totalWage;
	control = totalWorkTime;
	weeklyWTime = Time(totalWorkTime, 0);
	
	}
	string output() {
	salary();
	string outputStr = "";
	outputStr += "Name: " + name + "\n";
	outputStr += "Hourly Wage: " + to_string(hourlyWage) + "\n";
	outputStr += "Weekly Wage: " + to_string(weeklyWage) + "\n";
	outputStr += "Weekly Work Time: " + to_string(weeklyWTime.toDecimal()) + "\n";
    return outputStr;
	}
    };
    int main(){
		string name1;
		double hourlyWage1;
		double WeeklyWage1;
		double WeeklyWorkTime1;
		int hour;
		int min;
		cout << "Name: ";
		cin >> name1;
		cout << "\nHourly Wage: ";
		cin >> hourlyWage1;
		cout << " WeeklyWage :";
		cin >> WeeklyWage1;
		cout << "\n WeeklyWorkTime :";
		cin >> WeeklyWorkTime1;

	for (int i = 1; i < 6; i++) {
	cout << "\nHours " << i << " day (0-23): ";
	cin >> hour;
	cout << "Minutes " << i << " day (0-59): ";
	cin >> min;
	}
	Time t1(hour, min);
	Employee e1 = Employee(name1, hourlyWage1);
	cout << e1.output();
	e1.salary();
    }