#include <stdio.h>

unsigned int wpartition(float x[], float w[], unsigned int start, unsigned int end, unsigned int pivot, float* sumleft, float* sumright)
{
	unsigned int i;
	float tmpx, tmpw, pivotx, pivotw;

	// move pivot to the last place
	tmpx = x[end]; tmpw = w[end];
	x[end] = x[pivot]; w[end] = w[pivot];
	x[pivot] = tmpx; w[pivot] = tmpw;

	pivotx = x[end];
	pivotw = w[end];

	// loop initialization
	pivot = start;
	*sumleft = 0.0f;
	*sumright = 0.0f;

	// partition loop
	for(i=start; i<end; ++i)
		if(x[i] < pivotx)
		{
			*sumleft += w[i];

			tmpx = x[i]; tmpw = w[i];
			x[i] = x[pivot]; w[i] = w[pivot];
			x[pivot] = tmpx; w[pivot] = tmpw;

			++pivot;
		}
		else
			*sumright += w[i];

	// move pivot to its final place
	tmpx = x[end]; tmpw = w[end];
	x[end] = x[pivot]; w[end] = w[pivot];
	x[pivot] = tmpx; w[pivot] = tmpw;

	return pivot;
}

float wmedian(float x[], float w[], unsigned int count)
{
	unsigned int pivot, i, left, right;
	float sumleft, sumright, cumsum, sum;

	// compute cumulative sum
	for(i=0, cumsum=0.0f; i<count; ++i)
		cumsum += w[i];

	// initilaize loop
	sum = 0.0f;
	left = 0;
	right = count - 1;

	// loop
	while(1)
	{
		if(left == right)
		{
			return x[left];
		}

		// choose pivot
		pivot = left;

		// partition
		pivot = wpartition(x, w, left, right, pivot, &sumleft, &sumright);

		//
		if(sum + sumleft >= cumsum/2.0f)
			right = pivot - 1;
		else if(sum + sumleft + w[pivot] >= cumsum/2.0f)
		{
			return x[pivot];
		}
		else
		{
			sum = sum + sumleft + w[pivot];

			left = pivot + 1;
		}
	}
}