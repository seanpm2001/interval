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
// Interval Sqrt
// interval Sqrt(const interval& x) const;
// void testSqrt() const;

interval interval_algebra::Sqrt(const interval& x) const
{
    interval xp = intersection(x, Positive());
    if (xp.isEmpty()) {
        return xp;
    }
    return {sqrt(xp.lo()), sqrt(xp.hi())};
}

void interval_algebra::testSqrt() const
{
    analyzeUnaryMethod(10, 1000, "sqrt", interval(0, 10), sqrt, &interval_algebra::Sqrt);
}
}  // namespace itv
