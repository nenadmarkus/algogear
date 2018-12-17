#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "mmmul.c"

float getticks()
{
	struct timespec ts;

	if(clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
		return -1.0f;

	return ts.tv_sec + 1e-9f*ts.tv_nsec;
}

void mmmul_naive(float* C, const float* A, const float* B, int m, int n, int p)
{
	int i, j, k;

	for(i = 0; i < m; i++)
		for(j = 0; j < p; j++)
			for(k = 0; k < n; k++)
				C[p*i+j] += A[n*i+k] * B[p*k+j];
}

float* generate_random_matrix(int m, int n)
{
	int i;
	float* tmp = malloc(m*n*sizeof(float));

	for(i=0; i<m*n; ++i)
		tmp[i] = rand() % 1024;

	return tmp;
}

int main()
{
	float t;
	int i, m=1048, n=1048, p=1048;

	float* A = generate_random_matrix(m, n);
	float* B = generate_random_matrix(n, p);
	float* C = generate_random_matrix(m, p);

	//
	t = getticks();
	mmmul_naive(C, A, B, m, n, p);
	printf("* elapsed time for mmmul_naive: %f [ms]\n", 1000.0f*(getticks() - t));

	//
	int bsizes[] = {2, 4, 8, 16, 32, 64, 128, 256};

	for(i=0; i<8; ++i)
	{
		t = getticks();
		mmmul(C, A, B, m, n, p, bsizes[i]);
		printf("* elapsed time for mmmul[bsize=%d]: %f [ms]\n", bsizes[i], 1000.0f*(getticks() - t));
	}

	return 0;
}