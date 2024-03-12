This document describes how we compute the output precision of a function over an interval with certain input precision.

# General principle

We consider a function $f$ that we study over the interval $[lo;hi]$, with $lo < hi$. The fixpoint format for the input is given, we denote its lsb $l$ and $u = 2^l$ the gap between two consecutive numbers. The goal is to determine the minimum of $|f(x) - f(y)|$ with $x$, $y$ ranging over $[lo;hi]$, which will inform us of the minimum number of digits needed to distinguish between all the images of $f$ over $[lo;hi]$.

This minimum number of digits needed to distinguish between two numbers $x$ and $y$ is linked to the notion of their Least Common Bit (LCB), that is the position of the first bit at which they differ.
If two non-negative numbers $x$ and $y$ have LCB $-k$, then $|x-y|\le 2^{-k}$ and $log_2|x-y| \le LCB(x, y)$: thus, $log_2|x-y|$ is a sound approximation of their LCB.
This is particularly useful in situations where we do not have direct access to the digits of the numbers, for example when studying them in a general abstract setting.

In the case of monotonous functions, this minimum will be attained for two consecutive arguments: $|f(x) - f(x±u)|$. This $x$ is determined as the point where function $f$ has lowest slope over $[lo;hi]$.

When a function is not monotonous, it can happen that two non-consecutive fixpoint arguments have images closer than any two consecutive fixpoints. The usual functions subjected to this phenomenon are the periodic trigonometric functions $sin$, $cos$ and $tan$. For these functions, we relax the constraint and only seek the lowest difference between two consecutive arguments.

The overall goal is to find the proper $x$ and $±u$ for each usual function $f$. After that, the target lsb is given by $l' = prec(f, x, ±u) = ⌊log₂(|f(x±u) - f(x)|)⌋$.

## Possible pitfalls

If the input precision $l$ is too fine, then $f(x+u)$ and $f(x)$ might be so close that they will be computed as the same floating-point number. In that case, the computation of $f(x+u) - f(x)$ will yield zero and the computed output precision $-\infty$. 

When a situation of this type occur, we fall back on another method for precision computation, based on a Taylor expansion of $f$. We approximate $|f(x+u) - f(x)|$ with $u\cdot|f'(x)|$ and convert it into the output precision $l' = l + \log_2(|f'(x)|)$.

## Implementation conventions

The point at which the precision is computed, called $x$ above, is called `v` in the implementation. 
Whether the next point of the computation $x \pm \epsilon$ is the one before or after $x$ is encoded in `sign`. This mostly depends on whether $x$ is the higher or lower bound of the interval: we chose `sign` so that $x \pm u$ remains in the interval.

# Faust primitives

Here we explain the logic behind the implementation of precision inference in each Faust primitive. The interval $[lo; hi]$ is the input interval of the function, possibly restricted to its definition domain.

## Abs

abs is a function that is equal to the identity function on positive values and its opposite on negative values. As such, its output precision should be the same as its input precision.

### Integers

When applied to integers, Abs is implemented using the `std::abs` C++ function. 
This function has undefined behaviour when applied to `INT_MIN`, since `-INT_MIN = INT_MAX + 1` is not representable.
We chose to interpret this fact as the indication that `INT_MIN` is out of bounds for Abs. 
We deal with it the same way we deal with other domain violations: 
we restrict the input interval to legal values and apply Abs to the restricted interval.


**TODO**: Modifications will have to be made to the code generator as well to warn about domain violations.

## Acos 

acos is the reciprocal of the cosine function.

It is defined over the interval $[-1;1]$, so we restrict the input interval by intersecting it with this domain. If this intersection is empty, the output has no valid value and thus the output interval is empty as well.

Its derivative attains a global minimum at point 0 (and diverges towards $\pm \infty$ at the bounds of the interval). If 0 is included in the interval $[lo; hi]$, we compute $prec(f, 0, ±u)$. Otherwise, if $0 < lo$, the minimum derivative is at $lo$ and we compute $prec(f, lo, +u)$, and if $0 > hi$, the minimum derivative is at $hi$ and we compute $prec(f, hi, -u)$.

If the precision computed with this method is not sound (ie, the inferred precision is infinite), then we fall back on a less accurate precision determination method, based on the Taylor expansion of $acos$:
$|acos(x + u) - acos(x)| = |u\cdot\frac{-1}{\sqrt{1 - x^2}}|$.

We take the $\log_2$ of this expression to retrieve the relevant number of bits:
$l' = l - \frac{1}{2}\log_2(1 - x^2)$.

## Acosh

acosh is the reciprocal of the cosh (hyperbolic cosine) function.

It is defined over the domain $[1; + \infty[$: we restrict the input interval by intersecting it with this domain. If the intersection is empty, the output interval is empty as well.

It is a concave function : its derivative is decreasing, so the lowest slope is attained at the high end of the interval. We compute $prec(f, hi, -u)$, with $-u$ is in order to always evaluate $f$ at a point that is in the interval.

If this method computes an infinite precision, we fall back on a Taylor-based method: 
$|acosh(x + u) - acosh(x)| = |u\cdot \frac{1}{x^ -1}|$,
so $l' = l - \frac{1}{2}\log_2(hi^2 - 1)$.

## Add 

Addition is a binary operator, and the above method, designed for unary, real functions, does not apply to it. We have to devise another precision computation technique. 

Let us consider the addition of arguments $x$ and $y$, with respective precisions $l_x$ and $l_y$. Without loss of generality, let's assume that $l_x < l_y$, ie, $x$ is represented with more precision than $y$. In order to be able to distinguish the two sums $x + y$ and $x' + y$, where the operands $x$ and $x'$ only differ by their last bit, bits up to $l_x$ have to be conserved in the output.
Thus, we establish that $l' = min(l_x, l_y)$ is the coarsest output precision that still respects pseudo-injectivity.

### Integers

Wrapping situations due to integer overflow have to be taken care of. In the case where both operands are integers, if their sum is higher than the biggest representable integer or lower than the smallest representable integer, the default behaviour is for the value to "wrap" around and get to the other end of the integer range. This results in the output interval of the operation being `[INT_MIN; INT_MAX]`.

## And 

And is a bitwise boolean operator.

Example: `And(1011, 1100) = 1000`. Only the positions where both operands have bits set to `1` get set to `1` in the result.  

`And` behaves like a mask: one of the arguments will "mask" the other in that only the bits where the first argument has a `1` will be preserved in the other, while the remaining bits will be indiscriminately set to `0`.

Considering that all bits beyond the LSB of an argument have value `0`, all output bits in these positions will have value `0` as well. 
From the point of view of pseudo-injectivity, this means that these bits cannot be used to distinguish outputs, and can be ditched as such. 
As a consequence, the output LSB will coarser or equal than the coarsest input LSB: $l' \ge max(l_x, l_y)$.

In the case where neither of the intervals are singletons, ie when they both containt at least two values, this bound is attained.
Indeed, these intervals are guaranteed to contain values with the last bit set to both `0` and `1` values. 
This means that the "mask of 0s" will not expand prior to the LSB.

*Example:* $[000; 010]_0 \wedge [100; 110]_1 = \{000; 001; 010\} \wedge \{100; 110\} = \{000; 010\} = [000; 010]_1$

If one (or both) of the interval is a singleton, the mask can extend prior to the LSB. 
We detect the position of bit with value `1` and the least weight, and treat it as the "actual" LSB of the singleton-interval. 
The output LSB is the maximum of both input LSBs. 

*Example:* $[000; 110]_0 \wedge [100]_1 = \{000; 001; 010; 011; 100; 101; 110\} \wedge \{100\} = \{000; 100\} = [000; 100]_2$

Even though the input interval are specified with LSBs $0$ and $1$, the actual output interval can be represented with LSB $2$ without loss of information.

**TODO** Explain the bounds of the interval.

## Asin 

asin is the reciprocal of the sine function. It is very similar to the acos function in its analysis.

It is defined over the interval $[-1; 1]$: we restrict its input interval to this domain. Applying the function to an empty interval yields an empty interval.

Its derivative has a minimum in $0$: this is where we compute the precision if $0$ is included in the interval. Otherwise, the boundary of the interval closest to $0$ is used: $lo$ if $0 < lo$, $hi$ if $0 > hi$.

In case of an infinite precision, the Taylor formula is:
$|asin(x\pm u) - asin(x)| = |u \cdot \frac{1}{sqrt{1- x^2}}|$, 
hence $l' = l - \frac{1}{2}\log_2(1- x^2)$.

## Asinh

asinh is the reciprocal of the sinh (hyperbolic sine) function. 

It is defined over the whole $\mathbb{R}$ set.

It is an increasing function, its derivative tends to 0 in both $+\infty$ and $-\infty$. Thus, the precision is computed at the point of the interval of highest magnitude: $hi$ if $|hi| > |lo|$, $lo$ otherwise.

The Taylor fallback is:
$|asinh(x+u) - asinh(x)| = |u \cdot \frac{1}{\sqrt{1 + x^2}}|$,
so $l' = l - \frac{1}{2} \log_2(1 + x^2)$.

## Atan

atan is the reciprocal of the tangent function. 

It is defined over the whole $\mathbb{R}$ set.

It is an increasing function, whose derivative tends to 0 in $+\infty$ and $-\infty$. Thus, its precision is computed at the point of highest magnitude: $hi$ if $|hi| > |lo|$, $lo$ otherwise.

The Taylor fallback is:
$atan(x+u) - atan(x) = |u\cdot \frac{1}{1+x^2}|$, 
so $l' = l - \log_2(1+ x^2)$.

## atan2

$\textrm{atan2}(y,x)$ represents the angle the vector of coordinates $(x,y)$ makes with the $(Ox^-)$ axis in the Cartesian plane. 

It is computed using the following formula:

$$\textrm{atan2}(y, x) =
\begin{align}
\arctan\left(\frac{y}{x}\right) &\text{ if } x > 0, \\
\arctan\left(\frac{y}{x}\right) + \pi &\text{ if } x < 0 \text{ and } y \ge 0, \\
\arctan\left(\frac{y}{x}\right) - \pi &\text{ if } x < 0 \text{ and } y < 0, \\
+\frac{\pi}{2} &\text{if } x = 0 \text{ and } y > 0, \\
-\frac{\pi}{2} &\text{if } x = 0 \text{ and } y < 0, \\
\text{undefined} &\text{ if } x = 0 \text{ and } y = 0.
\end{align}
$$

### Interval

To determine its output interval, we have to compute the maximum and minimum value it takes over the input intervals $[\underline{x};\overline{x}]\times [\underline{y};\overline{y}]$. These values depend on the quadrant(s) with which this domain intersects.

This function presents a discontinuity on the negative part of the x-axis, where the angle leaps from $\pi$ to $-\pi$.

If the domain over which we study $\textrm{atan2}$ intersects this discontinuity, we separate it between positive and negative values of $y$, so that $\textrm{atan2}$ is continuous over each of these sub-domains.

Then, when studying a domain over which $\textrm{atan2}$ is continuous, the maximum and the minimum are determined in function of the quadrant(s) intersected by the domain, considered in clockwise order for the maximum and counter-clockwise for the minimum.

* If the domain intersects $\{(x, y) | x \le 0, y\ge0\}$, then the maximum of $atan2$ over the domain is attained at $(\underline{x}, \underline{y})$.
* The next quadrant to consider is $\{(x, y) | x \ge 0, y\ge0\}$, where the maximum would lie at $(\underline{x},\overline{y})$.
* If there is an intersection with $\{(x, y) | x \ge 0, y< 0\}$ but not the previous quadrants, look for the maximum at $(\overline{x}, \overline{y})$.
* Finally, if the domain is entirely contained in the last quadrant $\{(x, y) | x > 0, y<0\}$, the maximum will be at $(\overline{x}, \underline{y})$.

The location of the minimum is determined with a similar method.

### Precision

Computing the exact minimum precision verifying pseudo-injectivity is difficult for the $\textrm{atan2}$ function, as it has two arguments and is non-linear. We use the formula above to compute a sound precision, by applying the pseudo-injectivity closed formula on function $\arctan$ and an input precision corresponding to the output precision of $\frac{y}{x}$.

We were not able to verify that this solution is optimal.

This is not the most satisfying solution, but since $\textrm{atan2}$ is not a function that tends to be used a lot in Faust programs, the impact should be limited.

## Atanh

atanh is the reciprocal of the tanh (hyperbolic tangent) function.

It is defined over the $]-1;1[$ open interval. It tends towards $\pm \infty$ at the bounds of the interval. To account for the exclusion of $-1$ and $1$ from the definition interval, the `std::nexttoward` function is used to generate bounds as close to the excluded values as the `double` format permits.

An empty input interval gives an empty output interval.

Its derivative attains a minimum at $0$: we compute the precision at the point $x$ of smallest magnitude in the interval ($0$ if it is included).

The Taylor fallback is computed as follows:
$|atanh(x+u) - atanh(x)| = |u \cdot \frac{1}{1 - x^2}|$,
so $l' = l' - \log_2(1 - x^2)$.

## Button

A button is a Faust UI element that takes values $0$ when it is up and $1$ when it is down.
It thus outputs a boolean signal, that only needs $1$ bit to be represented, and by consequence a LSB of $l'=0$.

## Ceil

Ceil is the ceiling function $x \mapsto \lceil x \rceil$, returning the integer right above its argument. 

It returns an empty interval on an empty argument.

Although its result is mathematically an integer, the implementation of the primitive uses `std::ceil`, whose return value is a floating-point number.
We thus chose to set the output precision to $-1$, which might be under-optimal, but will force the interval library and the code generator to treat the result as floating-point.

## Checkbox

Checkbox functions similarly to Button intervals-wise. Its boolean output warrants a precision of $l'=0$.

## Cos

Cos is the cosine trigonometric function.

It is a periodic function of period $2\pi$. 
Its derivative periodically attains $0$ at points of the form $k\cdot\pi$, $k\in\mathbb{Z}$. 
These values are irrationnal numbers, which are not representable using floating-point or fixed-point numbers.
The smallest gap between two consecutive images is attained at one of the multiple of $\pi$ present in the interval.
While it is possible to bound how close to a multiple of $\pi$ a fixed-point number will be, 
it is too costly to compute for this application, and we will consider that the difference computed at the smallest multiple of $\pi$ (i.e. 0) gives sufficient precision.

## Cosh

Cosh is the hyperbolic cosine function. 

It is defined by $cosh(x) = \frac{e^x + e^{-x}}{2}$ over the whole $\mathbb{R}$ set.

The derivative of $cosh$ is $sinh$, which attains its lowest absolute value $0$ in $0$. If $0 \in [lo; hi]$, then the precision is computed at 0, otherwise it is computed at the point of lowest magnitude: $hi$ if $|hi| < |lo|$, $lo$ otherwise.

The Taylor fallback is:
* if $x = 0$, we use the second-order Taylor expansion of $cosh$: $cosh(u) = \frac{u^2}{2} + o(u^2)$, so $l' = 2*l -1$.
* if $x\neq 0$, $cosh(x + u) - cosh(x) = |u \cdot sinh(x)|$ so $l' = l + \log_2(|sinh(x)|)$.

## Delay

Delay is a Faust primitive used to temporally shift a signal. It takes two arguments: the signal that is delayed, and the signal indicating the amount of delay.  

The output signal is composed of the same samples as the input signal, so it should be represented with the same precision. 

## Div

Div is the division operator. 
It is expressed as the composition of multiplication and the inverse. 
Both the bounds and the precision of the output interval are computed from the result of the Mul function.

## Eq

Eq is the boolean equality test between two signals. 
It can return either $0$ or $1$, so no more than 1 bit is needed to represent the result: output precision is $0$.

## Exp 

Exp is the exponential function. 

It is a convex function that is defined on the whole $\mathbb{R}$ set. 
It is equal to its own derivative and the derivative has a minimum in $lo$.

Due to the very high dynamic range of the exponential function, it actually has the potential to push usual floating-point formats to their limits (ie generate overflows and underflows).
To manage that phenomenon, precision computation is implemented differently for the exponential than for other real functions.
**TODO** Give a concrete example as to how a straightforward implementation could fail.

The general formula for the computation of the output precision is $\log_2(e^{x+u} - e^x)$.
Due to the properties of the exponential function, this expression can be rewritten as $\log_2(e^x\cdot(e^u -1)) = x\cdot\log_2(e) + \log_2(e^u - 1)$.
We denote $\delta = e^u - 1$, $p_1 = x \cdot \log_2(e)$ and $p_2 = \log_2(\delta)$ to simplify proofs.

If $u$ is too small, then $e^u$ is computed to be $1$ and $\delta = 0$.
The threshold for such a $u$ is $2\cdot 10^{-16}$ (the difference between `1` and the next representable `double`), corresponding to an input LSB of $l = -52$. 

If $u$ has large enough magnitude, $\delta$ can be computed directly as $e^u-1$ and $p_2$ as $\log_2(\delta)$.

If $\delta$ is computed to be $0$, we replace it by the Taylor approximation of $e^u - 1 \approx u$, and then $p_2 = l$.

## FloatCast

FloatCast is the operator casting values into floats.

The same precision is needed for the output as for the input.

**Question:** what happens if we cast an `int` value in `float` using that rule? 
The precision will remain above $0$ and the output value will be interpreted as `int` because of that, defeating the purpose of a `float` cast.
It would maybe be better to reassign precisions greater than $0$ to $-1$ in order to force the `float` typing.

## FloatNum

FloatNum is the operator constructing a singleton interval from a floating-point number.
The creation of the interval, including its precision, is deferred to the `singleton` function.

## Floor

Floor is the floor function $x \mapsto \lfloor x \rfloor$, returning the integer right above its argument.

It returns an empty interval on an empty argument.

Although its result is mathematically an integer, the implementation of the primitive uses `std::floor`, whose return value is a floating-point number.
We thus chose to set the output precision to $-1$, which might be under-optimal, but will force the interval library and the code generator to treat the result as floating-point.

# Typology of functions

## Binary operator

### Mul

$l_x + l_y$: not sure if always attained but is a sound over-approximation.


## Convex functions

The derivative is increasing, so the lowest slope is attained at the low end of the interval.

We thus compute $prec(f, lo, +u)$.

Functions: $exp$, inverse on $]0; +\infty[$

## Concave functions

The derivative is decreasing, so the lowest slope is attained at the high end of the interval.

We compute $prec(f, hi, -u)$, with $-u$ is in order to evaluate $f$ at a point that is in the interval.

Functions: $log$, $log_{10}$, $acosh$, $\sqrt{\;}$, inverse on $]-\infty; 0[$

## The lowest slope is attained at one point (typically zero)

The derivative attains a global minimum at a point $x0$. If $x0$ is included in the interval $[lo;hi]$, we compute $prec(f, x0, ±u)$. Otherwise, if $x0 < lo$, the minimum derivative is at $lo$ and we compute $prec(f, lo, +u)$, and if $x0 > hi$, the minimum derivative is at $hi$ and we compute $prec(f, hi, -u)$.

Functions: $acos$, $asin$, $atanh$, $cosh$, $sinh$ ($x_0=0$ for all).


## Trigonometric functions

Lowest slope is attained periodically at multiples or half-multiples of $π$. However, those are irrational numbers and thus not representable, so this lowest slope is never *quite* attained. The smallest gap is theoretically attained for two consecutive representable numbers framing a multiple (or half-multiple) of $π$. Identifying the pair of numbers attaining this minimum is a difficult problem. 

Thus, we instead chose to study $cosPi : x \mapsto cos(π\*x)$, $sinPi : x \mapsto sin(π\*x)$ and $tanPi : x \mapsto tan(π\*x)$, where the lowest slopes are attained at integer or half-integer numbers, which have the advantage of being representable in a fixpoint format. Besides, this form corresponds to the one that is typically used in DSP applications.

The input interval $[lo; hi]$ is normalised to $[lo'; hi']$ such that $0 ≤ lo' ≤ 2$ and $hi' - lo' = hi - lo$. We then test the translated interval for an integer or half-integer $k$, depending on the function considered. In that case, we compute $prec(f, k, ±u)$. 
If there is no integer in the translated interval, we compute $prec(f, lo', +u)$ if $lo'$ is closer to its closest integer than $hi'$ and $prec(f, hi', -u)$ otherwise.

This approach can cause some rounding issues in the implementation: there can be $x$ and $y$ such that $cos(π\*x)$ and $cos(π\*y)$ are mathematically equal but for which the roundings of $π\*x$ and $π\*y$ cause their images through cos to be slightly different, resulting in a measured lsb much lower than what is actually needed.

Functions: $cos$, $tan$, $sin$

### Update 23/06

I ended up deciding against this solution because of the complications involved (in particular, the fact that using this correctly would involve having to detect expressions of the form $cos(\pi·x)$), and because distinguishing between images that are not adjacent doesn't seem to be necessary (this will have to be validated by experiments).

## Binary functions

These are functions such as $atan2$ (computing the angle between a vector $(x, y)$ and the x-axis) and $pow$ (computing $x^y$). 
These are typically tackled by expressing them as the composition of a binary operator and of an unary function that has already been studied.


### pow

$x^y = exp(y\cdot ln(x))$: since $exp$, $ln$ and multiplication have already be studied, we can thus compute the output interval and the LSB by chaining these well-known operations.

### Mod, Remainder and Rint

* Mod is implemented using the `std::fmod` C++ function, which is specified as: $fmod(x, y) = x - \lfloor \frac{x}{y} \rfloor \cdot y$. 
Two conditions apply to it: it should be the same sign as $x$, and lesser than $y$ in magnitude.
* Remainder is implemented using the `std::remainder` C++ function, which is specified as 
$remainder(x,y) = x - n\cdot y$ where $n$ is the integer closest to $\frac{x}{y}$ (ties to even).
This value might have sign opposite to $x$, unlike what $fmod$ does.
* Rint is implemented using the `std::rint` C++ function, which rounds the argument to an integer value using the current rounding mode. 

# Avoiding absorption

In the case of $pow$, we chain the computations of LSBs associated to $ln$, multiplication and $exp$.
These can quickly get very fine precisions, and result in computations of the form $f(x + u) - f(x)$ where $u$ is so small compared to $x$ that in machine representation $x + u = x$, resulting in an infinite computed precision.

We thus need to rewrite precision computations so that these forms do not appear.
We need to rewrite them in the case of $log$ and $exp$.

## Logarithm

Precision computation for base $a$ logarithm studied over interval $[\underline{x};\overline{x}]\_l$ looks like this in its basic form:
$\lfloor log_2(log\_a(\overline{x}) - log\_a(\overline{x} - 2^l))\rfloor$

In cases where $l$ is negative enough (and thus $2^l$ very small), the representation of $\overline{x} - 2^l$ is identical to that of $\overline{x}$, and the computation yields a precision of $-\infty$.

To avoid such a phenomenon, we rewrite the computation as follows:
$log_a(\overline{x} - 2^l) = log_a(\overline{x}\cdot(1 - \frac{2^l}{\overline{x}})) = log_a(\overline{x}) + log_a(1 - \frac{2^l}{\overline{x}})$, thus the precision becomes $\lfloor log_2(- log_a(1 - \frac{2^l}{\overline{x}}))\rfloor$.

When $2^l/\overline{x}$ is so small that $x + 2^l$ gets rounded to $x$ (we will call that phenomenon absorption), it is reasonable to replace it by its first-order series expansion: $log_a(1 - \frac{2^l}{\overline{x}}) \approx -log_a(e)\cdot \frac{2^l}{\overline{x}}$.

The final form of the precision is then $\lfloor log_2(log_a(e)\cdot \frac{2^l}{\overline{x}})\rfloor = \lfloor log_2(log_a(e)) + l - log_2(\overline{x})\rfloor$.

## Integer power

Let's study $x \mapsto x^n$ for $n \in \mathbb{N}$.

The absolute lowest value of the slope is attained at zero, so if the input interval contains zero, the precision is $\lfloor log_2((2^l)^n - 0)\rfloor = l\cdot n$.

If zero is not in the interval, the precision is attained at $\tilde x$, the boundary of the interval at the lowest absolute value. 

Let's pose $\delta = (\tilde x+2^l)^n - \tilde x^n = \tilde x^n\cdot\left((1 + \frac{2^l}{\tilde x})^n - 1\right)$, with the precision being $\lfloor log\_2(\delta)\rfloor$. 
If $2^l$ is very small, $(1+ \frac{2^l}{\tilde x})^n \approx 1 + n\cdot \frac{2^l}{\tilde x}$, and thus the precision is $\lfloor log\_2(\tilde x^n \cdot n \cdot \frac{2^l}{\tilde x})\rfloor = \lfloor (n-1)\cdot log\_2(\tilde x) + log\_2(n) + l\rfloor$.

## 12/05 update

In a shocking turn of events, it appears that computing Pow is not the only context in which functions are composed. 
In fact, this happens about everywhere in any slightly context program.
Thus, any function can potentially get an argument with a precision arising from a previous argument computation, which can get low enough that $x+u$ and $x$ have the same representation. 
In consequence, we apply the same principle as above to every primitive real function.

### Arccos

If $u << x$, $$arccos(x+u) - arccos(x) = u \cdot arccos'(x) + o(u) = \frac{u}{\sqrt{1 - x^2}} + o(u)$$

If the input interval contains $0$, at which the slope of the function is the lowest, 
then $l\_{out} = \lfloor log\_2(u)\rfloor = l$.

If it does not, then it is computed at point $x$ plosest to $0$ and the precision is 
$l\_{out} = \lfloor log\_2(\frac{u}{\sqrt{1 - x^2}}) \rfloor = \lfloor l\_{in}- \frac{1}{2} \cdot log\_2(1 - x^2) \rfloor$.

Note that one could prove that if $|x| < \frac{\sqrt{3}}{2}$, then $l\_{out} = l\_{in}$ 
(but since this is not the case for all x we won't use this property in the implementation).

### SinPi

If $u << x$, 
$$sinPi(x + u) - sinPi(x) = u \cdot \pi \cdot sinPi'(x) + \frac{u^2}{2}\cdot \pi^2 \cdot sinPi''(x) + o(u^2) = u \cdot \pi \cdot cosPi(x) - \frac{u^2}{2}\cdot \pi^2 \cdot sinPi(x) + o(u^2)$$.

If the input interval contains a half-integer (for instance $\frac{1}{2}$), 
then precision is computed at that point and becomes $l\_{out} = \lfloor log\_2(\frac{u^2}{2}\cdot \pi^2) \rfloor = 2\cdot(l_in + \lfloor log\_2(\pi)\rfloor) - 1 = 2\cdot l - 1$.

If it doesn't, then precision is computed at point $x$ closest to a half-integer and becomes $l\_{out} = \lfloor log\_2(u\cdot\pi\cdot cosPi(x)) \rfloor = l + \lfloor log\_2(\pi) + log\_2(cosPi(x))\rfloor$

# Backwards precision computation

i.e., given an output precision $l_{out}$, determine an input precision $l_{in}$ such that $l_{in}$ would induce output precision $l_{out}$ in the direct direction.

We suppose that we know at which point $x_0$ to compute the precision in the direct direction if given an input precision.
So, we know that the relationship between $l_{in}$ and $l_{out}$ is as follows:
$l_{out} = ⌊log_2(f(x_0+2^{l_{in}}) - f(x_0))⌋$
(we do away with the absolute value and the $±$ without loss of generality)
This can be translated as a pair of inequalities:
$l_{out} ≤ log_2(f(x+2^{l_{in}}) - f(x)) < l_{out} + 1$
By inverting these inequalities and inverting $f$ (assuming that it has the right properties for that) we get:
$log_2(f^{-1}(2^{l_{out}} + f(x_0)) - x_0) ≤ l_{in} < log_2(f^{-1}(2^{l_{out}+1} + f(x_0)) - x_0)$

The first inequality is the most important: in the direct direction, it ensures that the choice of precision is sound, ie that it won't cause any collision.
In the backwards direction, it means that the input precision shouldn't be chosen lower than this limit, lest we end up with inputs that the chosen output precision won't be able to discriminate.

The other inequality is less crucial: it is only here to ensure that the precision chosen is tight, ie that we don't choose an output precision that is too precise wrt what we need (or, in the reverse sense, that we are not too sloppy with the input precision).

So, in conclusion, the formula of $l_{in}$ in function of $l_{out}$ is:
$l_{in} = ⌈log_2(f^{-1}(2^{l_{out}} + f(x_0)) - x_0)⌉$.
By design, it always fulfills the important inequality, and it may or may not fulfill the other, but we shouldn't be too concerned about this.
