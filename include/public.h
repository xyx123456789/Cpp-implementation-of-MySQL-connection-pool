#pragma once
#include <iostream>

#define LOG(str) \
    do {\
        std::cout << __TIMESTAMP__ << ":" << __FILE__ << ":" << __LINE__ << ":" << " " \
                  << str << std::endl;\
    } while(0)