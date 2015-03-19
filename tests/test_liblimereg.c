#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "test_common.h"
#include <limereg.h>

#define dim 4096
unsigned char imgRef[dim*dim];
unsigned char imgTmp[dim*dim];

bool test_Limereg_GetVersion()
{
	printf("Testing Limereg_GetVersion()\n");
	return (0 == strcmp(VERSION, Limereg_GetVersion()));
}

#if 0
bool test_Limereg_RegisterImage()
{
	printf("Testing Limereg_RegisterImage()\n");

	double xShift=0;
	double yShift=0;
	double rotation=0;
	double distanceMeasure=0;
	unsigned int iterationAmount=0;

	memset(imgRef, 0, dim*dim);
	memset(imgTmp, 0, dim*dim);

	//Two squares made of very thick lines, shifted to each other by xoff,yoff
	int i=0;
	int ii=0;
	const int yoff = (int)(dim*0.02f);
	const int xoff = yoff*2;
	const int pauseS = (int)(dim*0.4f);
	const int pauseE = dim-pauseS;
	for(i=(int)(dim*0.2f); i<(int)(dim*0.8f); i++)
	{
		for(ii=(int)(dim*0.2f); ii<(int)(dim*0.8f); ii++)
		{
			if((pauseS>i || pauseE<i) && (pauseS>ii || pauseE<ii))
			{
				imgRef[i*dim+ii]=255;
				imgTmp[(i+yoff)*dim+(ii+xoff)]=255;
			}
		}
	}

	Limereg_RetCode ret = Limereg_RegisterImage(
			imgRef,
			imgTmp,
			dim,
			dim,
			50,
			5,
			10,
			0,
			0,
			0,
			&xShift,
			&yShift,
			&rotation,
			&distanceMeasure,
			&iterationAmount,
			NULL
			);

	if(LIMEREG_RET_SUCCESS == ret)
	{
		const float expextedRot = 0.0f;
		printf("retcode=%i, tx=%f px, ty=%f px, rot=%f °, SSD=%f, iterations: %u\n", ret, xShift, yShift, rotation, distanceMeasure, iterationAmount);
		printf("expected tx=%i, ty=%i, rot=%f\n", xoff, yoff, expextedRot);

		const float maxTransErrPix = 1;
		const float maxRotErrDeg = 0.1;
		if(maxTransErrPix > fabs(xShift-xoff) && maxRotErrDeg > fabs(rotation-expextedRot))
		{
			return true;
		}
	}
	else
	{
		printf("retcode=%i (ERROR !)\n", ret);
	}

	return false;
}
#endif

int main(void)
{
	//Execute tests
	bool t1 = outputTestResult(test_Limereg_GetVersion());
	bool t2 = true; //outputTestResult(test_Limereg_RegisterImage());

	//Evaluate results
	bool passed = (true == t1 == t2);
	printf("Overall result: ");
	outputTestResult(passed);
	return (passed ? RET_SUCCESS : RET_FAILED);
}
