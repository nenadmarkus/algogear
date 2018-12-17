## Matrix-matrix multiplication

The file `mmmul.c` contains a cache-friendly implementation of matrix-matrix multiplication for dense matrices.

The code is optimized for large matrices.

### Description

Function prototype:

	void mmmul(float* C, const float* A, const float* B, int m, int n, int p, int bsize)

Function arguments/inputs:

* `C`, `A` and `B` are matrices represented as row-major 2D arrays with sizes `m x p`, `m x n` and `n x p`, respectively
* the contents of `C` are replaced with the matrix-matrix product of `A` and `B` after the funciton finishes
* `bsize` is the size of the block used in the internal loops of the function

The parameter `bsize` significantly affects the speed of computation.
A good first choice is to set it to 128.
However, the optimal value should be determined experimentally.

### Testing

The program `test.c` times the matrix-matrix multiplication process for naive implementation (tripple for loop) and `mmmul.c` for various values of `bsize`.

The program can be compiled and run by the following command:

	cc test.c -o test -O3 && ./test

### Acknowledgements

This code was modified from <https://github.com/deuxbot/fast-matrix-multiplication>.