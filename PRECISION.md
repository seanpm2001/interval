This document describes how we compute the output precision of a function over an interval with certain input precision.

# General principle

We consider a function $f$ that we study over the interval $[lo;hi]$, with $lo < hi$. The fixpoint format for the input is given, we denote its lsb $l$ and $ε = 2^l$ the gap between two consecutive numbers. The goal is to determine the minimum of $|f(x) - f(y)|$ with $x$, $y$ ranging over $[lo;hi]$, which will inform us of the minimum number of digits needed to distinguish between all the images of $f$ over $[lo;hi]$.

This minimum number of digits needed to distinguish between two numbers $x$ and $y$ is linked to the notion of their Least Common Bit (LCB), that is the position of the first bit at which they differ.
If two non-negative numbers $x$ and $y$ have LCB $-k$, then $|x-y|\le 2^{-k}$ and $log_2|x-y| \le LCB(x, y)$: thus, $log_2|x-y|$ is a sound approximation of their LCB.
This is particularly useful in situations where we do not have direct access to the digits of the numbers, for example when studying them in a general abstract setting.

In the case of monotonous functions, this minimum will be attained for two consecutive arguments: $|f(x) - f(x±ε)|$. This $x$ is determined as the point where function $f$ has lowest slope over $[lo;hi]$.

When a function is not monotonous, it can happen that two non-consecutive fixpoint arguments have images closer than any two consecutive fixpoints. The usual functions subjected to this phenomenon are the periodic trigonometric functions $sin$, $cos$ and $tan$. We study modified versions of these functions to get around this difficulty and get back to the case where the minimum is between two consecutive numbers.

The overall goal is to find the proper $x$ and $±ε$ for each usual function $f$. After that, the target lsb is given by $prec(f, x, ±ε) = ⌊log₂(|f(x±ε) - f(x)|)⌋$.

# Typology of functions

## Binary operator

### Add

$min(l_x, l_y)$ is exact: attained

**TODO** Attained for whom?

### Mul

$l_x + l_y$: not sure if always attained but is a sound over-approximation.

### Div

We express it as the composition of multiplication and the inverse.

## Convex functions

The derivative is increasing, so the lowest slope is attained at the low end of the interval.

We thus compute $prec(f, lo, +ε)$.

Functions: $exp$, inverse on $]0; +\infty[$

## Concave functions

The derivative is decreasing, so the lowest slope is attained at the high end of the interval.

We compute $prec(f, hi, -ε)$, with $-ε$ is in order to evaluate $f$ at a point that is in the interval.

Functions: $log$, $log_{10}$, $acosh$, $\sqrt{\;}$, inverse on $]-\infty; 0[$

## The lowest slope is attained at one point (typically zero)

The derivative attains a global minimum at a point $x0$. If $x0$ is included in the interval $[lo;hi]$, we compute $prec(f, x0, ±ε)$. Otherwise, if $x0 < lo$, the minimum derivative is at $lo$ and we compute $prec(f, lo, +ε)$, and if $x0 > hi$, the minimum derivative is at $hi$ and we compute $prec(f, hi, -ε)$.

Functions: $acos$, $asin$, $atanh$, $cosh$, $sinh$ ($x_0=0$ for all).

## Even function with derivative decreasing towards zero at ±∞

The minimum of the derivative over $[lo;hi]$ is attained at whichever has the highest absolute value, since the derivative of an even function is odd.

We thus compute $prec(f, hi, -ε)$ if $|hi| > |lo|$ and $prec(f, lo, +ε)$ otherwise.

Functions: $asinh$, $atan$, $tanh$

## Trigonometric functions

Lowest slope is attained periodically at multiples or half-multiples of $π$. However, those are irrational numbers and thus not representable, so this lowest slope is never *quite* attained. The smallest gap is theoretically attained for two consecutive representable numbers framing a multiple (or half-multiple) of $π$. Identifying the pair of numbers attaining this minimum is a difficult problem. 

Thus, we instead chose to study $cosPi : x \mapsto cos(π\*x)$, $sinPi : x \mapsto sin(π\*x)$ and $tanPi : x \mapsto tan(π\*x)$, where the lowest slopes are attained at integer or half-integer numbers, which have the advantage of being representable in a fixpoint format. Besides, this form corresponds to the one that is typically used in DSP applications.

The input interval $[lo; hi]$ is normalised to $[lo'; hi']$ such that $0 ≤ lo' ≤ 2$ and $hi' - lo' = hi - lo$. We then test the translated interval for an integer or half-integer $k$, depending on the function considered. In that case, we compute $prec(f, k, ±ε)$. 
If there is no integer in the translated interval, we compute $prec(f, lo', +ε)$ if $lo'$ is closer to its closest integer than $hi'$ and $prec(f, hi', -ε)$ otherwise.

This approach can cause some rounding issues in the implementation: there can be $x$ and $y$ such that $cos(π\*x)$ and $cos(π\*y)$ are mathematically equal but for which the roundings of $π\*x$ and $π\*y$ cause their images through cos to be slightly different, resulting in a measured lsb much lower than what is actually needed.

Functions: $cos$, $tan$, $sin$

## Binary functions

These are functions such as $atan2$ (computing the angle between a vector $(x, y)$ and the x-axis) and $pow$ (computing $x^y$). 
These are typically tackled by expressing them as the composition of a binary operator and of an unary function that has already been studied.

### atan2

Since $atan2(y,x)$ represents the angle of $(x,y)$, the location of the maximum and minimum value it takes over a product of intervals $[\underline{x};\overline{x}]\times [\underline{y};\overline{y}]$ depends on the quadrant(s) which this domain intersects.

This function presents a discontinuity on the negative part of the x-axis, where the angle leaps from $\pi$ to $-\pi$.

If the domain over which we study $atan2$ intersects this discontinuity, we separate it between positive and negative values of $y$, so that $atan2$ is continuous over each of these sub-domains.

Then, when studying a domain over which $atan2$ is continuous, the maximum and the minimum are determined in function of the quadrant(s) intersected by the domain, considered in clockwise order for the maximum and counter-clockwise for the minimum.

* If the domain intersects $\{(x, y) | x \le 0, y\ge0\}$, then the maximum of $atan2$ over the domain is attained at $(\underline{x}, \underline{y})$.
* The next quadrant to consider is $\{(x, y) | x \ge 0, y\ge0\}$, where the maximum would lie at $(\underline{x},\overline{y})$.
* If there is an intersection with $\{(x, y) | x \ge 0, y< 0\}$ but not the previous quadrants, look for the maximum at $(\overline{x}, \overline{y})$.
* Finally, if the domain is entirely contained in the last quadrant $\{(x, y) | x > 0, y<0\}$, the maximum will be at $(\overline{x}, \underline{y})$.

The location of the minimum is determined with a similar method.

### pow

$x^y = exp(y\cdot ln(x))$: since $exp$, $ln$ and multiplication have already be studied, we can thus compute the output interval and the LSB by chaining these well-known operations.

# Avoiding absorption

In the case of $pow$, we chain the computations of LSBs associated to $ln$, multiplication and $exp$.
These can quickly get very fine precisions, and result in computations of the form $f(x + \epsilon) - f(x)$ where $\epsilon$ is so small compared to $x$ that in machine representation $x + \epsilon = x$, resulting in an infinite computed precision.

We thus need to rewrite precision computations so that these forms do not appear.
We need to rewrite them in the case of $log$ and $exp$.

## Logarithm

Precision computation for base $a$ logarithm studied over interval $[\underline{x};\overline{x}]\_l$ looks like this in its basic form:
$\lfloor log_2(log\_a(\overline{x}) - log\_a(\overline{x} - 2^l))\rfloor$

In cases where $l$ is negative enough (and thus $2^l$ very small), the representation of $\overline{x} - 2^l$ is identical to that of $\overline{x}$, and the computation yields a precision of $-\infty$.

To avoid such a phenomenon, we rewrite the computation as follows:
$log_a(\overline{x} - 2^l) = log_a(\overline{x}\cdot(1 - \frac{2^l}{\overline{x}})) = log_a(\overline{x}) + log_a(1 - \frac{2^l}{\overline{x}})$, thus the precision becomes $\lfloor log_2(- log_a(1 - \frac{2^l}{\overline{x}}))\rfloor$.

When $2^l/\overline{x}$ is so small that $x + 2^l$ gets rounded to $x$ (we will call that phenomenon absorption), it is reasonable to replace it by its first-order series expansion: $log_a(1 - \frac{2^l}{\overline{x}}) \approx -log_a(e)\cdot \frac{2^l}{\overline{x}}$.

The final form of the precision is then $\lfloor log_2(log_a(e)\cdot \frac{2^l}{\overline{x}})\rfloor = \lfloor log_2(log_a(e)) + l - log_2(\overline{x})\rfloor$

## Exponential

$exp(\underline{x} + 2^l) - exp(\underline{x}) = exp(\underline{x})\cdot (exp(2^l) - 1)$

If $2^l$ is very small, $exp(2^l) \approx 1 + 2^l$ and thus the former expression becomes $exp(\underline{x})\cdot 2^l$.

Reinjected into the precision, this becomes $\lfloor log_2(exp(\underline{x})) + l\rfloor$.

## Integer power

Let's study $x \mapsto x^n$ for $n \in \mathbb{N}$.

The absolute lowest value of the slope is attained at zero, so if the input interval contains zero, the precision is $\lfloor log_2((2^l)^n - 0)\rfloor = l\cdot n$.

If zero is not in the interval, the precision is attained at $\tilde x$, the boundary of the interval at the lowest absolute value. 

Let's pose $\delta = (\tilde x+2^l)^n - \tilde x^n = \tilde x^n\cdot\left((1 + \frac{2^l}{\tilde x})^n - 1\right)$, with the precision being $\lfloor log\_2(\delta)\rfloor$. 
If $2^l$ is very small, $(1+ \frac{2^l}{\tilde x})^n \approx 1 + n\cdot \frac{2^l}{\tilde x}$, and thus the precision is $\lfloor log\_2(\tilde x^n \cdot n \cdot \frac{2^l}{\tilde x})\rfloor = \lfloor (n-1)\cdot log\_2(\tilde x) + log\_2(n) + l\rfloor$.

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
