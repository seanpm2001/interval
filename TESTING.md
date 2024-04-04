These notes on testing the intervals results are destined to future library contributors who might want to add tests to validate their additions to the library.

# Testing functions

How the various test functions work.

## `check`

# Number of samples

In a number of cases, the number of samples needs to be higher than usual to give valid results.

## Large intervals
If the interval is larger than usual (for example, if we're testing the limits of number formats), the number of samples needed to have a dense covering of the interval is higher.
On these interval, the `analyzeUnaryMethod` and `analyzeBinaryMethod` functions can measure a precision that is coarser than the actual ground-truth precision, because it didn't draw enough samples to land on a minimal gap.
This should not be taken as an indicator that the implemented interval function is incorrect. 
Increasing the number of samples should solve the issue.

## Binary functions
A binary function has two input intervals, and thus the domain to cover with the sampling is quadratically larger than for a unary function with similar input interval.
Thus, the number of samples needed is the square of the number needed to have the same results for a unary function.

