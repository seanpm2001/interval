#pragma once

#include <cmath>

/** 
 * truncates x at the precision induced by lsb
 * */
double truncate(double x, int lsb)
{
    double epsilon = pow(2, lsb);
    double res = epsilon*(double)floor(x/epsilon);
    return res;
}