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
// Interval Acosh
// interval Acosh(const interval& x);
// void testAcosh();

static const interval domain(1, HUGE_VAL);

interval interval_algebra::Acosh(const interval& x)
{
    interval i = intersection(domain, x);
    if (i.isEmpty()) {
        return i;
    }

    // the min slope is attained at the highest bound of the interval
    // we thus compute the gap between f(hi) and f(hi-ε), to remain in the interval
    int precision = exactPrecisionUnary(acosh, x.hi(), -pow(2, x.lsb()));

    if (precision == INT_MIN or taylor_lsb)
        precision = floor(x.lsb() - (double)log2(x.hi()*x.hi() - 1)/2);

    return {acosh(i.lo()), acosh(i.hi()), precision};
}

void interval_algebra::testAcosh()
{
    analyzeUnaryMethod(10, 1000, "acosh", interval(950, 1000, 0), acosh, &interval_algebra::Acosh);
    analyzeUnaryMethod(10, 1000, "acosh", interval(950, 1000, -5), acosh, &interval_algebra::Acosh);
    analyzeUnaryMethod(10, 1000, "acosh", interval(950, 1000, -10), acosh, &interval_algebra::Acosh);
    analyzeUnaryMethod(10, 1000, "acosh", interval(950, 1000, -15), acosh, &interval_algebra::Acosh);
    analyzeUnaryMethod(10, 1000, "acosh", interval(950, 1000, -20), acosh, &interval_algebra::Acosh);

    /* analyzeUnaryMethod(10, 1000, "acosh", interval(0, 10, 0), acosh, &interval_algebra::Acosh);
    analyzeUnaryMethod(10, 1000, "acosh", interval(0, 10, -5), acosh, &interval_algebra::Acosh);
    analyzeUnaryMethod(10, 1000, "acosh", interval(0, 10, -10), acosh, &interval_algebra::Acosh);
    analyzeUnaryMethod(10, 1000, "acosh", interval(0, 10, -15), acosh, &interval_algebra::Acosh);
    analyzeUnaryMethod(10, 1000, "acosh", interval(0, 10, -20), acosh, &interval_algebra::Acosh);*/
}
}  // namespace itv
