#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

#include <limereg.h>

#define dim 512
unsigned char imgRef[dim*dim];
unsigned char imgTmp[dim*dim];

int main(void) {

	double xShift=0;
	double yShift=0;
	double rotation=0;
	double distanceMeasure=0;
	unsigned int iterationAmount=0;

	memset(imgRef, 0, dim*dim);
	memset(imgTmp, 0, dim*dim);

	int i=0;
	int ii=0;
	int off = (int)(dim*0.1f);
	for(i=(int)(dim*0.2f); i<(int)(dim*0.8f); i++)
	{
		for(ii=(int)(dim*0.2f); ii<(int)(dim*0.8f); ii++)
		{
			imgRef[i*dim+ii]=255;
			imgTmp[(i-off)*dim+(ii-off)]=255;
		}
	}

	int ret = Limereg_RegisterImage(
			imgRef,
			imgTmp,
			dim,
			dim,
			20,
			10,
			20,
			0,
			0,
			&xShift,
			&yShift,
			&rotation,
			&distanceMeasure,
			&iterationAmount
			);

	printf("retcode=%i, tx=%f px, ty=%f px, rot=%f °, SSD=%f, iterations: %u \n", ret, xShift, yShift, rotation, distanceMeasure, iterationAmount);

	return 0;
}
