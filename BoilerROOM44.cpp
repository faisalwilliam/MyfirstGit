    
	
	#include <iostream>
    #include <string>
    #include <limits>
    #include <fstream>
    #include <filesystem>
    #include <ctime>
    #include <iomanip>
    #include <sstream>
    #include <vector>
    #include <algorithm>
    #include <cctype>
    #include "Employee.h"
    using namespace std;

    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string RESET = "\033[0m";

    bool isValidFilename(const string& filename) {
        if (filename.empty()) return false;
        const string invalidChars = "/\\:*?\"<>|";
        return filename.find_first_of(invalidChars) == string::npos;
    }

    int main(){
		int choice;
		while (true) {
		cout << "\n1. Create New Employee Report\n2. Read Saved Report\n3. Exit\nSelect option: ";
		cin >> choice;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "Invalid input. Please enter a number." << RESET << "\n";
			continue;
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 3) break;

		if (choice == 2) {
			vector<string> files;
			for (const auto& entry : std::filesystem::directory_iterator(".")) {
				if (entry.is_regular_file() && entry.path().extension() == ".txt") {
					files.push_back(entry.path().filename().string());
				}
			}

			string keyword;
			cout << "Enter keyword to filter files (Press Enter for all): ";
			getline(cin, keyword);

			string lowerKeyword = keyword;
			transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), 
				[](unsigned char c){ return tolower(c); });

			cout << "\n--- Available Files ---\n";
			sort(files.begin(), files.end());
			for (const auto& file : files) {
				string lowerFile = file;
				transform(lowerFile.begin(), lowerFile.end(), lowerFile.begin(), 
					[](unsigned char c){ return tolower(c); });
				if (keyword.empty() || lowerFile.find(lowerKeyword) != string::npos) {
					cout << file << endl;
				}
			}
			cout << "-----------------------\n";
			string filename;
			do {
				cout << "Enter filename: ";
				getline(cin, filename);
				if (!isValidFilename(filename)) {
					cout << RED << "Invalid filename! Please avoid characters like /\\:*?\"<>|" << RESET << "\n";
				}
			} while (!isValidFilename(filename));
			if (std::filesystem::exists(filename)) {
				ifstream inFile(filename);
				if (inFile.is_open()) {
					cout << "\n--- Report Content ---\n";
					cout << inFile.rdbuf();
					inFile.close();
				} else cout << RED << "Unable to open file." << RESET << "\n";
			} else {
				cout << RED << "Error: File '" << filename << "' does not exist." << RESET << "\n";
			}
		}

		else if (choice == 1) {
		string id1;
		string name1;
		double hourlyWage1;
		double taxRate1;
		int hour;
		int min;
		cout << "Employee ID: ";
		getline(cin, id1);
		cout << "Name: ";
		getline(cin, name1);
		do {
			cout << "\nHourly Wage: ";
			cin >> hourlyWage1;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << RED << "Invalid input! Please enter a number." << RESET << "\n";
				hourlyWage1 = -1;
			} else if (hourlyWage1 <= 0) cout << RED << "Invalid input! Please enter a positive wage." << RESET << "\n";
		} while (hourlyWage1 <= 0);

	do {
		cout << "Tax Rate (%): ";
		cin >> taxRate1;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "Invalid input! Please enter a number." << RESET << "\n";
			taxRate1 = -1;
		} else if (taxRate1 < 0) cout << RED << "Invalid input! Please enter a positive tax rate." << RESET << "\n";
	} while (taxRate1 < 0);

	Employee e1(id1, name1, hourlyWage1, taxRate1);
	for (int i = 0; i < 5; i++) {
	do {
		cout << "\nHours " << i + 1 << " day (0-23): ";
		cin >> hour;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "Invalid input! Please enter a number." << RESET << "\n";
			hour = -1;
		} else if (hour < 0 || hour > 23) cout << RED << "Invalid input! Please enter 0-23." << RESET << "\n";
	} while (hour < 0 || hour > 23);

	do {
		cout << "Minutes " << i + 1 << " day (0-59): ";
		cin >> min;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "Invalid input! Please enter a number." << RESET << "\n";
			min = -1;
		} else if (min < 0 || min > 59) cout << RED << "Invalid input! Please enter 0-59." << RESET << "\n";
	} while (min < 0 || min > 59);

	e1.setDailyWorkTime(i, hour, min);
	}
	cout << "\033[2J\033[1;1H";
	string report = e1.output();
	cout << report;

	char saveChoice;
	cout << "\nDo you want to save this report to a file? (y/n): ";
	cin >> saveChoice;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	if (saveChoice == 'y' || saveChoice == 'Y') {
		string filename;
		bool confirmed = false;
		while (!confirmed) {
			do {
				cout << "Enter filename base: ";
				getline(cin, filename);
				if (!isValidFilename(filename)) {
					cout << RED << "Invalid filename! Please avoid characters like /\\:*?\"<>|" << RESET << "\n";
				}
			} while (!isValidFilename(filename));

			time_t t = time(nullptr);
			tm* now = localtime(&t);
			stringstream ss;
			ss << put_time(now, "_%Y-%m-%d");
			filename += ss.str();
			filename += ".txt";

			if (std::filesystem::exists(filename)) {
				char overwrite;
				cout << RED << "Warning: File '" << filename << "' already exists. Overwrite? (y/n): " << RESET;
				cin >> overwrite;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				if (overwrite == 'y' || overwrite == 'Y') confirmed = true;
			} else {
				confirmed = true;
			}
		}

		ofstream outFile(filename);
		if (outFile.is_open()) {
			outFile << report;
			cout << GREEN << "Report saved to " << filename << RESET << endl;
		}
		outFile.close();
	}
		}
		}
    }