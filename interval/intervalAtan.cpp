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
#include <functional>
#include <random>

#include "check.hh"
#include "interval_algebra.hh"
#include "interval_def.hh"

namespace itv {
//------------------------------------------------------------------------------------------
// Interval Atan
// Calculates the principal value of the arc tangent of x; that is the value whose tangent is x.
// The return value is in the range [-pi/2, pi/2].

// interval Atan(const interval& x) const;
// void testAtan() const;

interval interval_algebra::Atan(const interval& x) const
{
    if (x.isEmpty()) return x;

    double v = 0; // value at which the min slope is attained, here the bound of highest absolute value
    int sign = 1; // whether we compute the difference between f(v) and f(v+ε) or f(v-ε)
    if (std::abs(x.hi()) < std::abs(x.lo()))
        v = x.lo();
    else
    {
        v = x.hi();
        sign = -1; // if we compute the difference at the higer bound we have to take the FP point right before
    }

    int precision = exactPrecisionUnary(atan, v, sign*pow(2, x.lsb()));

    return {atan(x.lo()), atan(x.hi()), precision};
}

void interval_algebra::testAtan() const
{
    analyzeUnaryMethod(10, 1000, "atan", interval(-100, 100, 0), atan, &interval_algebra::Atan);
    analyzeUnaryMethod(10, 1000, "atan", interval(-100, 100, -5), atan, &interval_algebra::Atan);
    analyzeUnaryMethod(10, 1000, "atan", interval(-100, 100, -10), atan, &interval_algebra::Atan);
    analyzeUnaryMethod(10, 1000, "atan", interval(-100, 100, -15), atan, &interval_algebra::Atan);
    analyzeUnaryMethod(10, 1000, "atan", interval(-100, 100, -20), atan, &interval_algebra::Atan);
}
}  // namespace itv
