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
#include <cassert>
#include <functional>
#include <random>

#include "check.hh"
#include "interval_algebra.hh"
#include "interval_def.hh"

namespace itv {
//------------------------------------------------------------------------------------------
// Interval Pow
// interval Pow(const interval& x, const interval& y) const;
// void testPow() const;

static interval ipow(const interval& x, int y)
{
    assert(y >= 0);
    if (y == 0) {
        return interval(1.0);
    }

    if ((y & 1) == 0) {
        // y is even
        double z0 = std::pow(x.lo(), y);
        double z1 = std::pow(x.hi(), y);
        return {0, std::max(z0, z1)};
    }

    // y is odd
    return {std::pow(x.lo(), y), std::pow(x.hi(), y)};
}

interval interval_algebra::fPow(const interval& x, const interval& y) const
{
    assert(x.lo() > 0);
    // x all positive
    return Exp(Mul(y, Log(x)));
}

interval interval_algebra::iPow(const interval& x, const interval& y) const
{
    int      y0 = std::max(0, saturatedIntCast(y.lo()));
    int      y1 = std::max(0, saturatedIntCast(y.hi()));
    interval z  = ipow(x, y0);
    if (y1 > y0) {
        // we have more than one integer exponent
        z = reunion(z, ipow(x, y0 + 1));
        z = reunion(z, ipow(x, y1 - 1));
        z = reunion(z, ipow(x, y1));
    }
    return z;
}

interval interval_algebra::Pow(const interval& x, const interval& y) const
{
    // if (x.lo() > 0) {
    //     return fPow(x, y);
    // }
    // return iPow(x, y);

    // std::cerr << "Pow(" << x << "," << y << ")" << std::endl;
    interval xp = intersection(x, Positive());
    // std::cerr << "xp=" << xp << std::endl;
    interval xn = intersection(x, StrictNegative());
    // std::cerr << "xn=" << xn << std::endl;
    interval yp = intersection(y, Positive());
    // std::cerr << "yp=" << yp << std::endl;
    interval yn = intersection(y, StrictNegative());
    // std::cerr << "yn=" << yn << std::endl;

    interval r(NAN, NAN);  // the result is empty

    if (!yn.isEmpty()) {
        interval ynn = Neg(yn);
        interval z   = Inv(Pow(x, ynn));
        r            = reunion(r, z);
        // std::cerr << "yn=" << yn << " ynn=" << ynn << " z=" << z << std::endl;
    }

    if (!xp.isEmpty() && !yp.isEmpty()) {
        double a0 = std::pow(xp.lo(), yp.lo());
        double a1 = std::pow(xp.hi(), yp.lo());
        double b0 = std::pow(xp.lo(), yp.hi());
        double b1 = std::pow(xp.hi(), yp.hi());

        double   z0 = std::min(std::min(a0, a1), std::min(b0, b1));
        double   z1 = std::max(std::max(a0, a1), std::max(b0, b1));
        interval z  = interval(z0, z1);
        r           = reunion(r, z);
        // std::cerr << "xp=" << xp << " yp=" << yp << " z=" << z << std::endl;
    }

    if (!xn.isEmpty() && !yp.isEmpty()) {
        interval z = iPow(xn, yp);
        r          = reunion(r, z);
        // std::cerr << "xn=" << xn << " yp=" << yp << " z=" << z << std::endl;
    }
    // std::cerr << "Pow(" << x << "," << y << ") = " << r << std::endl;
    return r;
}

static double myfPow(double x, double y)
{
    return std::pow(x, y);
}

static double myiPow(double x, double y)
{
    return std::pow(x, int(y));
}

void interval_algebra::testPow() const
{
    analyzeBinaryMethod(10, 2000, "Pow", interval(-1, 1), interval(0.1, 6), myfPow, &interval_algebra::Pow);
    analyzeBinaryMethod(10, 2000, "Pow", interval(0, 1), interval(0, 1), myfPow, &interval_algebra::Pow);
    analyzeBinaryMethod(10, 2000, "Pow", interval(0, 10), interval(0, 4), myfPow, &interval_algebra::Pow);
    analyzeBinaryMethod(10, 2000, "Pow", interval(0, 10), interval(-4, 4), myfPow, &interval_algebra::Pow);
    analyzeBinaryMethod(10, 2000, "iPow2", interval(-100, 100), interval(0, 20), myiPow, &interval_algebra::iPow);
    analyzeBinaryMethod(10, 2000, "iPow2", interval(-1, 1), interval(1, 3), myiPow, &interval_algebra::iPow);
    analyzeBinaryMethod(10, 2000, "iPow2", interval(-1, 1), interval(1, 10), myiPow, &interval_algebra::iPow);
    analyzeBinaryMethod(10, 2000, "fPow2", interval(1, 1000), interval(-10, 10), myfPow, &interval_algebra::fPow);
    analyzeBinaryMethod(10, 2000, "fPow2", interval(0.001, 1), interval(-10, 10), myfPow, &interval_algebra::fPow);
    analyzeBinaryMethod(10, 2000, "fPow2", interval(0.001, 10), interval(-20, 20), myfPow, &interval_algebra::fPow);
}
}  // namespace itv
