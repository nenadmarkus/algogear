# Fast Fourier Transform

Implementation of the [radix-2 FFT algorithm](https://en.wikipedia.org/wiki/Cooley–Tukey_FFT_algorithm).

The file `fft.c` contains the function

	void fft(float x[], float X[], unsigned int log2n)

where

* `x` is the input array containing `2*n` floats representing `n` complex numbers (`x[2*i+0]` is the real part of the `i`th number and `x[2*i+1]` is its imaginary part);
* `X` is the output array containing `n` complex numbers repersenting the frequency spectrum of `x` (`X` has the same structure as `x`: `X[2*i+0]` `X[2*i+1]` are the real and imaginary parts of the `i`th number);
* `log2n` is the base-2 logarithm of `n`, i.e., the array size (notice that this restricts the computation only to the arrays where `n` is the power of two).

## Testing

The Python script `test.py` compiles `fft.c` as a shared library, generates a random complex array with 1024 elements and computes its FFT (the implementation in `fft.c` is accessed via `ctypes`).
The result is compared to the output of the function `numpy.fft.fft`.

## Inverse FFT

You can implement IFFT via the provided FFT primitive as explained in <https://www.dsprelated.com/showarticle/800.php>.