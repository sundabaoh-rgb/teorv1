#pragma once

#include <vector>
typedef struct {
    int N;
    double Pi;
    double deviation;
} Result;

extern std::vector<Result> result;

void ppp();
void gen();
