/*

*/

#ifndef ABS
#define ABS(x) ((x)>=0?(x):(-(x)))
#endif

/*
	performes an LU decompostion with partial pivoting
*/

int do_lup_factorization(float A[], float LU[], unsigned int p[], unsigned int n, float tolerance)
{
	unsigned int i, j, k;

	if(A != LU)
		for(i=0; i<n; ++i)
			for(j=0; j<n; ++j)
				LU[i*n + j] = A[i*n + j];

	for(i=0; i<n; ++i)
		p[i] = i;

	for(i=0; i<n-1; ++i)
	{
		unsigned int ind_max, tmp_int;

		ind_max = i;
		for(j=i+1; j<n; ++j)
			if(ABS(LU[p[j]*n + i]) > ABS(LU[p[ind_max]*n + i]))
				ind_max = j;

		tmp_int = p[i];
		p[i] = p[ind_max];
		p[ind_max] = tmp_int;

		if(ABS(LU[p[i]*n + i]) < tolerance)
			return 0; // matrix is singular (up to tolerance)

		for(j=i+1; j<n; ++j)
		{
			LU[p[j]*n + i] = LU[p[j]*n + i]/LU[p[i]*n + i];

			for(k=i+1; k<n; ++k)
				LU[p[j]*n + k] = LU[p[j]*n + k] - LU[p[i]*n + k]*LU[p[j]*n + i];
		}
	}

	return 1;
}

/*
	solves Ax=b (A is decomposed using the function do_lup_factorization)
*/

void solve_linear_system(float LU[], unsigned int p[], float x[], float b[], unsigned int n)
{
	int i, j;

	// forward substitution
	for(i=0; i<n; ++i)
	{
		x[i] = b[p[i]];

		for(j=0; j<i; ++j)
			x[i] = x[i] - LU[ p[i]*n + j ]*x[j];
	}

	// backward substitution
	for(i=n-1; i>=0; --i)
	{
		for(j=i+1; j<n; ++j)
			x[i] = x[i] - LU[p[i]*n + j]*x[j];

		x[i] = x[i]/LU[p[i]*n + i];
	}
}

/*
	inverts an LU decomposed matrix
*/

void invert_matrix(float LU[], unsigned int p[], float INV[], unsigned int n)
{
	unsigned int i, j;

	float tmpvec[n];
	float tmpval;

	for(i=0; i<n; ++i)
		tmpvec[i] = 0.0;

	for(i=0; i<n; ++i)
	{
		tmpvec[i] = 1.0;
		solve_linear_system(LU, p, &INV[i*n], tmpvec, n);
		tmpvec[i] = 0.0;
	}

	// transpose
	for(i=0; i<n; ++i)
		for(j=i+1; j<n; ++j)
		{
			tmpval = INV[i*n + j];
			INV[i*n + j] = INV[j*n + i];
			INV[j*n + i] = tmpval;
		}
}

/*
	LU decomposed matrix determinant computation
*/

float compute_determinant(float LU[], unsigned int p[], unsigned int n)
{
	unsigned int i, j;
	float determinant;

	determinant = 1.0;
	for(i=0; i<n; ++i)
		determinant *= LU[p[i]*n + i];

	j = 0;
	for(i=0; i<n; ++i)
		if(p[i] != i)
			++j;

	if(j && (j-1)%2==1)
		determinant = -determinant;

	return determinant;
}