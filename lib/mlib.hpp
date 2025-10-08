#pragma once

#include <vector>

typedef struct {
    int N;
    double Pi;
    double deviation;
} Result;

extern std::vector<Result> result;
#define REF_TSK2 0.142857 // 1/7

void gen(); //tsk1
void gen2(); //tsk2
