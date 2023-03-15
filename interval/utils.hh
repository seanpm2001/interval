#pragma once

#include <cmath>

/**
 * @brief Computes the minimum of four doubles
 */
static double min4(double a, double b, double c, double d)
{
    return std::min(std::min(a, b), std::min(c, d));
}

/**
 * @brief Computes the maximum of four doubles
 */
static double max4(double a, double b, double c, double d)
{
    return std::max(std::max(a, b), std::max(c, d));
}
