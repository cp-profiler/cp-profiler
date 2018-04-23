#pragma once

#include <iostream>
#include <vector>
#include <QString>
#include <QDebug>

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

inline void print(const char* temp) {
    std::cerr << temp << std::endl;
}

template<typename T, typename ... Types>
void print(const char* temp, T value, Types ... args) {
    for (;*temp != '\0'; ++temp) {

        if (*temp == '{') {
            std::cerr << value;
            print(temp+2, args...);
            return;
        }
        std::cerr << *temp;
    }
}


}