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
// Interval Sin
// interval Sin(const interval& x) const;
// void testSin() const;

static double sinPi(double x)
{
    return sin(x*M_PI);
}

interval interval_algebra::Sin(const interval& x) const
{
    double TWOPI = 2*M_PI;
    double epsilon = pow(2, x.lsb());

    int precision = floor(-1 + 2*x.lsb() + 2*log2(M_PI)); // in the case where there is an integer in the interval
    int truncated_precision = std::max(precision, -24);  
    
    if (x.size() >= 2) return {-1, 1};

    // normalize input interval between 0..2
    double l = fmod(x.lo(), 2);
    if (l < 0) l += 2;
    interval i(l, l + x.size(), x.lsb());

    // compute the default boundaries
    double a  = sinPi(i.lo());
    double b  = sinPi(i.hi());
    double lo = std::min(a, b);
    double hi = std::max(a, b);

    // check if integers are included
    if (i.has(0.5) || i.has(2.5)) hi = 1;
    if (i.has(1.5) || i.has(3.5)) lo = -1;

    // precision if we don't hit the half integers
    if (i.hi() < 0.5)
        precision = floor(log2(M_PI) + x.lsb() + log2(abs(cos(M_PI*(x.hi() - epsilon/2)))));
    else if (i.lo() > 0.5 and i.hi() < 1.5)
        if (i.lo() - 0.5 < 1.5 - i.hi())
            precision = floor(log2(M_PI) + x.lsb() + log2(abs(cos(M_PI*(x.lo() + epsilon/2)))));
        else
            precision = floor(log2(M_PI) + x.lsb() + log2(abs(cos(M_PI*(x.hi() - epsilon/2)))));
    else if (i.lo() > 1.5 and i.hi() < 2)
        precision = floor(log2(M_PI) + x.lsb() + log2(abs(cos(M_PI*(x.lo() + epsilon/2)))));
        
    return {lo, hi, precision};
}

void interval_algebra::testSin() const
{
    // analyzeUnaryMethod(5, 20000, "sin", interval(-1, 1, -3), sinPi, &interval_algebra::Sin);
    analyzeUnaryMethod(10, 40000, "sin", interval(1, 2, -3), sinPi, &interval_algebra::Sin);
    analyzeUnaryMethod(10, 40000, "sin", interval(1, 2, -5), sinPi, &interval_algebra::Sin);
    analyzeUnaryMethod(10, 40000, "sin", interval(1, 2, -10), sinPi, &interval_algebra::Sin);
    analyzeUnaryMethod(10, 40000, "sin", interval(1, 2, -15), sinPi, &interval_algebra::Sin);    
    analyzeUnaryMethod(10, 40000, "sin", interval(1, 2, -20), sinPi, &interval_algebra::Sin);
    analyzeUnaryMethod(10, 40000, "sin", interval(1, 2, -24), sinPi, &interval_algebra::Sin); 
}
}  // namespace itv
