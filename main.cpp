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
#include <iostream>
#include <sstream>
#include <string>

#include "interval/check.hh"
#include "interval/interval_algebra.hh"
#include "interval/interval_def.hh"

using namespace itv;

int main()
{
    // test interval representation
    /*check("interval()", interval());
    check("interval(0,100)", interval(100.0, 0.0));
    check("interval(0)", interval(0, 0));
    check("interval(-10,0)", interval(0, -10));

    // test union intersection

    check("test2", intersection(interval(0, 100), interval(-10, 0)), interval(0));
    check("test3", intersection(interval(10, 100), interval(-10, 0)), interval());
    check("test4", reunion(interval(0, 100), interval(-100, 50)), interval(-100, 100));
    check("test5", reunion(interval(0, 100), interval(10, 500)), interval(0, 500));

    // test predicates
    interval a(1, 100), b(10, 20), c(-10, 0), n;

    std::cout << a << " == " << b << " = " << (a == b) << std::endl;
    std::cout << a << " <= " << b << " = " << (a <= b) << std::endl;
    std::cout << a << " < " << b << " = " << (a < b) << std::endl;

    std::cout << std::endl;

    std::cout << a << " != " << b << " = " << (a != b) << std::endl;
    std::cout << a << " > " << b << " = " << (a > b) << std::endl;
    std::cout << a << " >= " << b << " = " << (a >= b) << std::endl;

    std::cout << std::endl;

    std::cout << a << " == " << a << " = " << (a == a) << std::endl;
    std::cout << a << " <= " << a << " = " << (a <= a) << std::endl;
    std::cout << a << " < " << a << " = " << (a < a) << std::endl;

    std::cout << std::endl;

    std::cout << a << " != " << a << " = " << (a != a) << std::endl;
    std::cout << a << " > " << a << " = " << (a > a) << std::endl;
    std::cout << a << " >= " << a << " = " << (a >= a) << std::endl;

    std::cout << std::endl;

    std::cout << a << " != " << n << " = " << (a != n) << std::endl;
    std::cout << a << " > " << n << " = " << (a > n) << std::endl;
    std::cout << a << " >= " << n << " = " << (a >= n) << std::endl;

    std::cout << std::endl;

    std::cout << "must be true " << (reunion(a, n) == a) << std::endl;
    std::cout << "must be true " << (intersection(a, n) == n) << std::endl;

    std::cout << std::endl;*/

    interval_algebra A;
    //A.testAll();
    /* A.testExp();
    A.testLog();
    A.testAcos(); 
    A.testAsin();
    A.testAcosh();
    A.testAsinh();*/
    A.testAtanh();
    /*A.testCosh();
    A.testLog10();
    A.testSin();
    A.testCos();
    A.testSinh();
    A.testSqrt();
    A.testTan();
    A.testTanh();*/
    
    /*{
        double u = 0.0;
        double v = nextafter(u, -HUGE_VAL);
        double w = nextafter(v, -HUGE_VAL);

        if ((u != v) && (v != w) && (u > v) && (v > w)) {
            std::cout << "Order OK" << std::endl;
        } else {
            std::cout << "Order NOT OK" << std::endl;
        }
    }

    analyzemod(interval(9), interval(10));
    analyzemod(interval(8, 9), interval(10));
    analyzemod(interval(8, 9), interval(1, 10));
    analyzemod(interval(8, 11), interval(10));
    analyzemod(interval(9), interval(9, 10));
    analyzemod(interval(-9), interval(9, 10));
    analyzemod(interval(0, 9), interval(9, 10));
    analyzemod(interval(-9, 0), interval(9, 10));
    analyzemod(interval(0, 100), interval(1));
    analyzemod(interval(-100, 100), interval(1));

    analyzeUnaryFunction(10, 1000, "rint", interval(-10000, 10000), rint);
    analyzeUnaryFunction(10, 1000, "floor", interval(-10000, 10000), floor);
    analyzeUnaryFunction(10, 1000, "ceil", interval(-10000, 10000), ceil);*/
}