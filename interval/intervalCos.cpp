/* Copyright 2023 Yann ORLAREY
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <algorithm>
#include <cmath>
#include <functional>
#include <random>

#include "check.hh"
#include "interval_algebra.hh"
#include "interval_def.hh"

namespace itv {
//------------------------------------------------------------------------------------------
// Interval Cos
// interval Cos(const interval& x) const;
// void testCos() const;

static double cosPi(double x)
{
    return cos(x*M_PI);
}

interval interval_algebra::Cos(const interval& x) const
{
    double TWOPI = 2 * M_PI;
    double epsilon = pow(2, x.lsb());

    int precision = floor(-1 + 2*x.lsb() + 2*log2(M_PI)); // in the case where there is an integer in the interval
    int truncated_precision = std::max(precision, -24);  

    if (x.isEmpty()) return {};
    if (x.size() >= 2) return {-1, 1, precision};

    // normalize input interval between 0..2 (corresponding to 0..2PI)
    double l = fmod(x.lo(), 2);
    if (l < 0) l += 2;
    interval i(l, l + x.size(), x.lsb());

    // compute the default boundaries
    double a  = cosPi(i.lo());
    double b  = cosPi(i.hi());
    double lo = std::min(a, b);
    double hi = std::max(a, b);

    // check if integers are included
    if (i.has(0) || i.has(2)) hi = 1;
    if (i.has(1) || i.has(3)) lo = -1;

    if (i.hi() < 1 or (i.lo() > 1 and i.hi() < 2)) // if there are no integers in i, i.e i is included in ]0;1[ or ]1;2[
    {
        if (ceil(x.hi()) - x.hi() < x.lo() - floor(x.lo())) // if the lowest slope is attained for the higher bound
            precision = floor(log2(M_PI) + x.lsb() + log2(abs(sin(M_PI*(x.hi() - epsilon/2))))); 
        else // ... for the lower bound
            precision = floor(log2(M_PI) + x.lsb() + log2(abs(sin(M_PI*(x.lo() + epsilon/2)))));
    }
  
    return {lo, hi, precision};
}

void interval_algebra::testCos() const
{
    analyzeUnaryMethod(10, 40000, "cos", interval(1, 2, -3), cosPi, &interval_algebra::Cos);
    analyzeUnaryMethod(10, 40000, "cos", interval(1, 2, -5), cosPi, &interval_algebra::Cos);
    analyzeUnaryMethod(10, 40000, "cos", interval(1, 2, -10), cosPi, &interval_algebra::Cos);
    analyzeUnaryMethod(10, 40000, "cos", interval(1, 2, -15), cosPi, &interval_algebra::Cos);    
    analyzeUnaryMethod(10, 40000, "cos", interval(1, 2, -20), cosPi, &interval_algebra::Cos);
    analyzeUnaryMethod(10, 40000, "cos", interval(1, 2, -24), cosPi, &interval_algebra::Cos);    
}
}  // namespace itv
