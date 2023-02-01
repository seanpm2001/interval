This document describes how we compute the output precision of a function over an interval with certain input precision.

# General principle

We consider a function `f` that we study over the interval `[lo;hi]`, with `lo<hi`. The fixpoint format for the input is given, we denote its lsb `l` and `ε = 2^l` the gap between two consecutive numbers. The goal is to determine the minimum of `|f(x) - f(y)|` with `x`, `y` ranging over `[lo;hi]`, which will inform us of the minimum number of digits needed to distinguish between all the images of `f` over `[lo;hi]`.

In the case of monotonous functions, this minimum will be attained for two consecutive arguments: `|f(x) - f(x±ε)|`. This `x` is determined as the point where function `f` has lowest slope over `[lo;hi]`.

When a function is not monotonous, it can happen that two non-consecutive fixpoint arguments have images closer than any two consecutive fixpoints. The usual functions subjected to this phenomenon are the periodic trigonometric functions `sin`, `cos` and `tan`. We study modified versions of these functions to get around this difficulty and get back to the case where the minimum is between two consecutive numbers.

The overall goad is to find the proper `x` and `±ε` for each usual function `f`. After that, the target lsb is given by `prec(f, x, ±ε) = ⌊log₂(|f(x±ε) - f(x)|)⌋`.

# Typology of functions

## Convex functions

The derivative is increasing, so the lowest slope is attained at the low end of the interval.

We thus compute `prec(f, lo, +ε)`.

Functions: `exp`

## Concave functions

The derivative is decreasing, so the lowest slope is attained at the high end of the interval.

We compute `prec(f, hi, -ε)`, with `-ε` is in order to evaluate `f` at a point that is in the interval.

Functions: `log`, `log10`, `acosh`, `sqrt`

## The lowest slope is attained at one point (typically zero)

The derivative attains a global minimum at a point `x0`. If `x0` is included in the interval `[lo;hi]`, we compute `prec(f, x0, ±ε)`. Otherwise, if `x0 < lo`, the minimum derivative is at `lo` and we compute `prec(f, lo, +ε)`, and if `x0 > hi`, the minimum derivative is at `hi` and we compute `prec(f, hi, -ε)`.

Functions: `acos`, `asin`, `atanh`, `cosh`, `sinh` (`x0=0` for all).

## Even function with derivative decreasing towards zero at ±∞

The minimum of the derivative over `[lo;hi]` is attained at whichever has the highest absolute value, since the derivative of an even function is odd.

We thus compute `prec(f, hi, -ε)` if `|hi| > |lo|` and `prec(f, lo, +ε)` otherwise.

Functions: `asinh`, `atan`, `tanh`

## Trigonometric functions

Lowest slope is attained periodically at multiples or half-multiples of π. However, those are irrational numbers and thus not representable, so this lowest slope is never *quite* attained. The smallest gap is theoretically attained for two consecutive representable numbers framing a multiple (or half-multiple) of π. Identifying the pair of numbers attaining this minimum is a difficult problem. 

Thus, we instead chose to study `cosPi : x -> cos(π*x)`, `sinPi : x -> sin(π*x)` and `tanPi : x -> tan(π*x)`, where the lowest slopes are attained at integer or half-integer numbers, which have the advantage of being representable in a fixpoint format. Besides, this form corresponds to the one that is typically used in DSP applications.

The input interval `[lo; hi]` is normalised to `[lo'; hi']` such that `0 ≤ lo' ≤ 2` and `hi' - lo' = hi - lo`. We then test the translated interval for an integer or half-integer `k`, depending on the function considered. In that case, we compute `prec(f, k, ±ε)`. 
If there is no integer in the translated interval, we compute `prec(f, lo', +ε)` if `lo'` is closer to its closest integer than `hi'` and `prec(f, hi', -ε)` otherwise.

This approach can cause some rounding issues in the implementation: there can be `x` and `y` such that `cos(π*x)` and `cos(π*y)` are mathematically equal but for which the roundings of `π*x` and `π*y` cause their images through cos to be slightly different, resulting in a measured lsb much lower than what is actually needed.

Functions: `cos`, `tan`, `sin`
