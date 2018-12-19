#include <stdint.h>

/*
	multiply with carry PRNG
*/

uint32_t mwcrand_r(uint64_t* state)
{
	uint32_t* m;

	//
	m = (uint32_t*)state;

	// bad state?
	if(m[0] == 0)
		m[0] = 0xAAAA;

	if(m[1] == 0)
		m[1] = 0xBBBB;

	// mutate state
	m[0] = 36969 * (m[0] & 65535) + (m[0] >> 16);
	m[1] = 18000 * (m[1] & 65535) + (m[1] >> 16);

	// output
	return (m[0] << 16) + m[1];
}

uint64_t prngglobal = 0x12345678000fffffLL;

void smwcrand(uint32_t seed)
{
	prngglobal = 0x12345678000fffffLL*seed;
}

uint32_t mwcrand()
{
	return mwcrand_r(&prngglobal);
}

/*
	ud3 code (learning + runtime)
*/

float get_wmse(int find, float thresh, float targets[], int tdim, float features[], int fdim, double ws[], int inds[], int ninds)
{
	int i, j;

	double wsum0, wsum1, wse0, wse1;
	double wtsum0[tdim], wtsum1[tdim];
	double wtmean0[tdim], wtmean1[tdim];

	//
	wsum0 = wsum1 = 0.0;

	for(i=0; i<tdim; ++i)
	{
		wtsum0[i] = 0.0;
		wtsum1[i] = 0.0;
	}

	for(i=0; i<ninds; ++i)
	{
		double w;

		if(ws)
			w = ws[inds[i]];
		else
			w = 1.0;

		if( features[inds[i]*fdim + find] < thresh )
		{
			wsum1 += w;

			for(j=0; j<tdim; ++j)
				wtsum1[j] += w*targets[inds[i]*tdim + j];
		}
		else
		{
			wsum0 += w;

			for(j=0; j<tdim; ++j)
				wtsum0[j] += w*targets[inds[i]*tdim + j];
		}
	}

	//
	for(i=0; i<tdim; ++i)
	{
		wtmean0[i] = wtsum0[i]/wsum0;
		wtmean1[i] = wtsum1[i]/wsum1;
	}

	wse0 = wse1 = 0.0;

	for(i=0; i<ninds; ++i)
	{
		double se = 0.0, w;

		if(ws)
			w = ws[inds[i]];
		else
			w = 1.0;

		if( features[inds[i]*fdim + find] < thresh )
		{
			for(j=0; j<tdim; ++j)
				se += (targets[inds[i]*tdim + j]-wtmean1[j])*(targets[inds[i]*tdim + j]-wtmean1[j]);

			wse1 += w*se;
		}
		else
		{
			for(j=0; j<tdim; ++j)
				se += (targets[inds[i]*tdim + j]-wtmean0[j])*(targets[inds[i]*tdim + j]-wtmean0[j]);

			wse0 += w*se;
		}
	}

	//
	return (float)( (wse0 + wse1)/(wsum0 + wsum1) );
}

int split_training_data(int find, float thresh, float features[], int fdim, int inds[], int ninds)
{
	int stop;
	int i, j;

	int n0;

	//
	stop = 0;

	i = 0;
	j = ninds - 1;

	while(!stop)
	{
		while( features[inds[i]*fdim + find] >= thresh )
		{
			if( i==j )
				break;
			else
				++i;
		}

		while( features[inds[j]*fdim + find] <  thresh )
		{
			if( i==j )
				break;
			else
				--j;
		}

		//
		if( i==j )
			stop = 1;
		else
		{
			// swap
			inds[i] = inds[i] ^ inds[j];
			inds[j] = inds[i] ^ inds[j];
			inds[i] = inds[i] ^ inds[j];
		}
	}

	//
	n0 = 0;

	for(i=0; i<ninds; ++i)
		if( features[inds[i]*fdim + find] >= thresh )
			++n0;

	//
	return n0;
}

int grow_subtree(int32_t finds[], float threshs[], float preds[], int nodeidx, int d, int maxd, float targets[], int tdim, float features[], int fdim, double ws[], int inds[], int ninds, int nrands)
{
	int i, n0, fis[2048];
	float ths[2048], errs[2048];

	//
	if(d == maxd)
	{
		int j, predidx;
		double taccum[tdim], wsum;

		//
		if(!preds)
			return 1;

		//
		predidx = nodeidx - ((1<<maxd)-1);

		// compute output: a simple average
		for(j=0; j<tdim; ++j)
			taccum[j] = 0.0;
		wsum = 0.0;

		for(i=0; i<ninds; ++i)
		{
			double w;

			if(ws)
				w = ws[inds[i]];
			else
				w = 1.0;

			for(j=0; j<tdim; ++j)
				taccum[j] += w*targets[tdim*inds[i] + j];
 
			wsum += w;
		}

		if(wsum == 0.0)
			for(j=0; j<tdim; ++j)
				preds[tdim*predidx + j] = 0.0f;
		else
			for(j=0; j<tdim; ++j)
				preds[tdim*predidx + j] = (float)( taccum[j]/wsum );

		//
		return 1;
	}
	else if(ninds <= 1)
	{
		//
		finds[nodeidx] = 0;
		threshs[nodeidx] = 0.0f;

		//
		grow_subtree(finds, threshs, preds, 2*nodeidx+1, d+1, maxd, targets, tdim, features, fdim, ws, inds, ninds, nrands);
		grow_subtree(finds, threshs, preds, 2*nodeidx+2, d+1, maxd, targets, tdim, features, fdim, ws, inds, ninds, nrands);

		return 1;
	}

	// generate random splits
	for(i=0; i<nrands; ++i)
	{
		fis[i] = mwcrand()%fdim;
		ths[i] = 0.5f*(features[inds[mwcrand()%ninds]*fdim + fis[i]] + features[inds[mwcrand()%ninds]*fdim + fis[i]]);
	}

	//
	#pragma omp parallel for
	for(i=0; i<nrands; ++i)
		errs[i] = get_wmse(fis[i], ths[i], targets, tdim, features, fdim, ws, inds, ninds);

	//
	float err = errs[0];
	int fi = fis[0];
	float th = ths[0];

	for(i=1; i<nrands; ++i)
		if(err > errs[i])
		{
			err = errs[i];
			fi = fis[i];
			th = ths[i];
		}

	finds[nodeidx] = fi;
	threshs[nodeidx] = th;

	//
	n0 = split_training_data(fi, th, features, fdim, inds, ninds);

	//
	grow_subtree(finds, threshs, preds, 2*nodeidx+1, d+1, maxd, targets, tdim, features, fdim, ws, &inds[0], n0, nrands);
	grow_subtree(finds, threshs, preds, 2*nodeidx+2, d+1, maxd, targets, tdim, features, fdim, ws, &inds[n0], ninds-n0, nrands);

	//
	return 1;
}

int ud3_new(int32_t finds[], float threshs[], float preds[], int d, float targets[], int tdim, float features[], int fdim, double ws[], int inds[], int ninds, int nrands)
{
	int i, retval;

	//
	return grow_subtree(finds, threshs, preds, 0, 0, d, targets, tdim, features, fdim, ws, inds, ninds, nrands);
}

int ud3_get_lut_index(int32_t finds[], float threshs[], int tdepth, float features[])
{
	int i, idx;

	for(i=0, idx=1; i<tdepth; ++i)
		idx = 2*idx + (features[finds[idx-1]] < threshs[idx-1]);

	return idx - (1<<tdepth);
}

float* ud3_run(int32_t finds[], float threshs[], float preds[], int pdim, int tdepth, float features[])
{
	return &preds[pdim*ud3_get_lut_index(finds, threshs, tdepth, features)];
}