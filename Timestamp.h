#pragma once 
#include<iostream>
#include<string>
#include "time.h"

class Timestamp
{
public:
    Timestamp();
    Timestamp(int64_t microSecondSinceEpoch );
    static Timestamp now();
    std::string toString() const;
private:
    int64_t microSecondSinceEpoch_;
    
};
