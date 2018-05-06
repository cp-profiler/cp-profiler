#pragma once

#include <iostream>
#include <vector>
#include <QString>
#include <QDebug>
#include <sstream>

namespace cpprofiler {

std::ostream& debug(std::string str = "");

std::ostream& operator<<(std::ostream& os, const QString& str);

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    if (vec.size() == 0) {
        return os << "[]";
    }

    os << "[";
    for (auto i = 0u; i < vec.size() - 1; ++i) {
        os << vec[i] << ",";
    }
    return os << vec.back() << "]";
}

inline std::string format(const char* temp) {
    return temp;
}

template<typename T, typename ... Types>
std::string format(const char* temp, T value, Types ... args) {
    std::ostringstream oss;
    for (;*temp != '\0'; ++temp) {

        if (*temp == '{') {
            oss << value;
            oss << format(temp+2, args...);
            break;
        }
        oss << *temp;
    }

    return oss.str();
}

template<typename ...Types>
void print(const char* temp, Types ... args) {
    std::cerr << format(temp, args...) << std::endl;
}


}