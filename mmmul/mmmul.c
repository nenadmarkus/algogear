#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

void mmmul(float* C, const float* A, const float* B, int m, int n, int p, int bsize)
{ 
	int i, j, k, ii, jj, kk;

	for(ii = 0; ii < m; ii += bsize)
		for(kk = 0; kk < n; kk += bsize)
			for(jj = 0; jj < p; jj += bsize)
			{
				float block[bsize*bsize];

				for(k=0; k<bsize*bsize; ++k)
					block[k] = 0;

				for(i = ii; i < MIN(m, ii+bsize); i++)
					for(k = kk; k < MIN(n, kk+bsize); k++)
						for(j = jj; j < MIN(p, jj+bsize); j++)
							block[(i-ii)*bsize + (j-jj)] += A[n*i+k] * B[p*k+j];

				for(i = ii; i < MIN(m, ii+bsize); i++)
					for(j = jj; j < MIN(p, jj+bsize); j++)
						C[p*i+j] = block[(i-ii)*bsize + (j-jj)];
			}	
}