#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <iostream>
using namespace std;

class Logger {
private:
    ofstream fout;

public:
    Logger(const string& filename) {
        fout.open(filename.c_str());
        if (!fout.is_open()) {
            throw runtime_error("Cannot open log file");
        }
    }

    ~Logger() {
        if (fout.is_open()) {
            fout.close();
        }
    }

    void write(const string& msg) {
        fout << msg << endl;
    }
};

#endif
