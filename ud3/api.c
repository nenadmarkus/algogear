#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ud3.c"

/*

*/

void* new_tree(float shrinkage, int depth, float targets[], int tdim, float features[], int fdim, int nsamples, double ws[], int nrands)
{
	int i;

	//
	int32_t* tree = malloc((2 + 2*((1<<depth)-1) + tdim*(1<<depth))*sizeof(int32_t));

	tree[0] = tdim;
	tree[1] = depth;

	int32_t* finds = (int32_t*)&tree[2];
	float* threshs = (float*)&tree[2 + (1<<depth)-1];
	float* preds = (float*)&tree[2 + (1<<depth)-1 + (1<<depth)-1];

	//
	int* inds = (int*)malloc(nsamples*sizeof(int));

	for(i=0; i<nsamples; ++i)
		inds[i] = i;

	//
	int retval = ud3_new(finds, threshs, preds, depth, targets, tdim, features, fdim, ws, inds, nsamples, nrands);

	for(i=0; i<(tdim*(1<<depth)); ++i)
		preds[i] = shrinkage*preds[i];

	free(inds);

	if(!retval)
	{
		free(tree);
		tree = 0;
	}

	//
	return tree;
}

void del_tree(void* tree)
{
	free(tree);
}

float* run_tree(void* tree, float features[])
{
	int pdim = ((int32_t*)tree)[0];
	int tdepth = ((int32_t*)tree)[1];

	int32_t* finds = (int32_t*)&((int32_t*)tree)[2];
	float* threshs = (float*)&((int32_t*)tree)[2 + (1<<tdepth)-1];
	float* preds = (float*)&((int32_t*)tree)[2 + (1<<tdepth)-1 + (1<<tdepth)-1];

	return ud3_run(finds, threshs, preds, pdim, tdepth, features);
}

/*

*/

void* load_tree(FILE* file)
{
	int pdim, tdepth;

	!fread(&pdim, sizeof(int32_t), 1, file);
	!fread(&tdepth, sizeof(int32_t), 1, file);

	int32_t* tree = malloc((2 + 2*((1<<tdepth)-1) + pdim*(1<<tdepth))*sizeof(int32_t));
	tree[0] = pdim;
	tree[1] = tdepth;

	!fread(&tree[2], sizeof(int32_t), 2*((1<<tdepth)-1) + pdim*(1<<tdepth), file);

	return tree;
}

int save_tree(void* tree, FILE* file)
{
	int pdim = ((int32_t*)tree)[0];
	int tdepth = ((int32_t*)tree)[1];

	return fwrite(tree, sizeof(int32_t), (2 + 2*((1<<tdepth)-1) + (1<<tdepth)), file);
}

/*

*/

void new_ensemble(float shrinkage, void* trees[], int ntrees, int depth, float targets[], int tdim, float features[], int fdim, double weights[], int nsamples, int nrands)
{
	int i, j, k;

	for(i=0; i<ntrees; ++i)
	{
		trees[i] = new_tree(shrinkage, depth, targets, tdim, features, fdim, nsamples, weights, nrands);

		for(j=0; j<nsamples; ++j)
		{
			float* preds = run_tree(trees[i], &features[j*fdim]);

			for(k=0; k<tdim; ++k)
				targets[j*tdim + k] -= preds[k];
		}
	}
}

void run_ensemble(void* trees[], int ntrees, float features[], int fdim, float predictions[], int pdim, int n)
{
	int i, j, k;

	for(i=0; i<n; ++i)
	{
		for(j=0; j<pdim; ++j)
			predictions[i*pdim + j] = 0.0f;

		for(j=0; j<ntrees; ++j)
		{
			float* preds = run_tree(trees[j], &features[i*fdim]);

			for(k=0; k<pdim; ++k)
			{
				predictions[i*pdim + k] += preds[k];
			}
		}
	}
}

void del_ensemble(void* trees[], int ntrees)
{
	int i;

	for(i=0; i<ntrees; ++i)
	{
		del_tree(trees[i]);
		trees[i] = 0;
	}
}