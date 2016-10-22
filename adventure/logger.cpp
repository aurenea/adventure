#include <iostream>
#include <fstream>

void Logger::init() {
    Logger::log.open("log.txt", ios::out | ios::app | ios::trunc);
}

void Logger::record(const char* o, const char* c) {
    if (Logger::log.is_open()) {
        Logger::log << "[" << o << "] " << c << "\n";
    }
    cout << "[" << o << "] " << c << "\n";
}

void Logger::close() {
    if (Logger::log.is_open()) { Logger::log.close(); }
}
