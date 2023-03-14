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
// Interval Atan2
// interval Atan2(const interval& x, const interval& y) const;
// void testAtan2() const;

// atan2(y, x) = 2*atan(y / (sqrt(x^2 + y^2) + x))

// be careful with the order of the arguments:
// atan2 is typically called as atan2(y,x) (where (x,y) are the cartesian coordinates of the point we wish to retrieve the angle of)
interval interval_algebra::Atan2(const interval& y, const interval& x) const 
{
    double lo = -M_PI;
    double hi = M_PI;
    int precision = INT_MIN;

    if (x.lo() <= 0 and y.hasZero()) // if we intersect the Ox- axis
        return {lo, hi, precision};

    if (y.lo() >= 0) // the domain XxY is entirely included in the higher half of the plane
    {
        if (x.lo() <= 0) // we intersect the quadrant in which atan2 takes the highest values
            hi = atan2(y.lo(), x.lo());
        else 
            hi = atan2(y.hi(), x.lo());
    } else {
        if (x.hi() >= 0)
        {
            if (y.hi() >=0 )
                hi = atan2(y.hi(), x.lo());
            else 
                hi = atan2(y.hi(), x.hi());
        } else {
            hi = atan2(y.lo(), x.hi());   
        }
    }

    if (y.hi() <= 0) // the domain XxY is entirely included in the lower half of the plane
    {
        if (x.lo() <= 0)
            lo = atan2(y.hi(), x.lo());
        else 
            lo = atan2(y.lo(), x.lo());
    } else {
        if (x.hi() >= 0)
        {
            if (y.lo() >= 0)
                lo = atan2(y.lo(), x.hi());
            else 
                lo = atan2(y.lo(), x.lo());
        } else {
            lo = atan2(y.hi(), x.hi());
        }
    }
    
    return {lo, hi, precision};
}

void interval_algebra::testAtan2() const
{
    // std::cout << "Atan2 not implemented" << std::endl;
    analyzeBinaryMethod(10, 2000, "atan2", interval(1, 2, -24), interval(1, 2, -24), atan2, &interval_algebra::Atan2);
    analyzeBinaryMethod(10, 2000, "atan2", interval(-1, 2, -24), interval(1, 2, -24), atan2, &interval_algebra::Atan2);
    analyzeBinaryMethod(10, 2000, "atan2", interval(-2, -1, -24), interval(1, 2, -24), atan2, &interval_algebra::Atan2);
    analyzeBinaryMethod(10, 2000, "atan2", interval(-2, -1, -24), interval(-1, 2, -24), atan2, &interval_algebra::Atan2);
    analyzeBinaryMethod(10, 2000, "atan2", interval(-2, -1, -24), interval(-2, -1, -24), atan2, &interval_algebra::Atan2);

    analyzeBinaryMethod(10, 2000, "atan2", interval(-1, 2, -24), interval(-1, 2, -24), atan2, &interval_algebra::Atan2);
    analyzeBinaryMethod(10, 2000, "atan2", interval(-1, 2, -24), interval(-2, -1, -24), atan2, &interval_algebra::Atan2);
}
}  // namespace itv