# LU decomposition with partial pivoting

Implementation of the [LU decomposition](https://en.wikipedia.org/wiki/LU_decomposition).

The file `lup.c` contains the function

	int do_lup_factorization(float A[], float LU[], unsigned int p[], unsigned int n, float tolerance)

which performs an LU decomposition of the matrix `A` of size `n`x`n`.
The result is stored in `LU` and `p`.
If the matrix is singular (up to tolerance `tolerance`), the funciton returns a `0`.
In the case everything is OK, a `1` is returned.

The LU-decomposed matrix can be used to

* solve a linear system (`solve_linear_system`);
* invert the matrix (`invert_matrix`);
* compute its determinant (`compute_determinant`).

## Testing

The Python script `test.py` compiles `lup.c` as a shared library, generates a random matrix `A` and finds `x` such that `Ax=b` for `b` filled with ones (of course, this works if `A` is not singluar).

The error is reported in the following way: `numpy.max(numpy.abs(b - numpy.dot(A, x)))`.

## Acknowledgements

<https://en.wikipedia.org/wiki/LU_decomposition>