#include <math.h>

unsigned int mirror_bits(unsigned int in, unsigned int nbits)
{
	unsigned int out, i;

	out = 0;

	for(i = 0; i < nbits; ++i)
	{
		out = (out << 1) | (in & 1);

		in >>= 1;
	}

	return out;
}

void fft(float x[], float X[], unsigned int log2n)
{
	unsigned int i, n;

	/*
		* compute the signal length
	*/

	for(i=log2n, n=1; i>0; --i)
		n = 2*n;
	
	/*
		* mirror bits: X[ b2b1b0 ] = x[ b0b1b2 ];
	*/

	for(i=0; i<n; ++i)
	{
		X[2*mirror_bits(i, log2n) + 0] = x[2*i + 0];
		X[2*mirror_bits(i, log2n) + 1] = x[2*i + 1];
	}

	/*

	*/

	for(i=n; i >= 2; i = i/2)
	{
		// get the length of FFTs that'll be performed in this stage
		unsigned int len = 2*n/i, j, k;

		// perform i/2 FFTs
		for(j = 0; j < i/2; ++j)
		{
			// even indexed parts
			float* even = &X[2*j*len];

			// odd indexed parts
			float* odd = &X[2*(j*len + len/2)];

			// do the actual "mini" FFT
			for(k = 0; k < len/2; ++k)
			{
				// get twiddle factor, exp(-j*2pi*k/len)
				float w[2] = {
					cosf(-2.0*M_PI*k/(float)len),
					sinf(-2.0*M_PI*k/(float)len)
				};

				// tmp1 = odd[k]*w;
				float tmp1[2] = {
					odd[2*k+0]*w[0] - odd[2*k+1]*w[1],
					odd[2*k+0]*w[1] + odd[2*k+1]*w[0]
				};

				// tmp2 = even[k];
				float tmp2[2] = {
					even[2*k + 0],
					even[2*k + 1]
				};

				// even[k] = tmp2 + tmp1;
				even[2*k + 0] = tmp2[0] + tmp1[0];
				even[2*k + 1] = tmp2[1] + tmp1[1];

				// odd[k] = tmp2 - tmp1;
				odd[2*k + 0] = tmp2[0] - tmp1[0];
				odd[2*k + 1] = tmp2[1] - tmp1[1];
			}
		}
	}
}