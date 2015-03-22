#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "test_common.h"
#include <limereg.h>

#define dim 4096
unsigned char imgRef[dim*dim];
unsigned char imgTmp[dim*dim];

bool test_findAlignment()
{
	printf("Test: Finding the image alignment by using Limereg_RegisterImage().\n");

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

	//Execute function under test

	struct Limereg_Image referenceImage;
	referenceImage.pixelBuffer = imgRef;
	referenceImage.imageWidth = dim;
	referenceImage.imageHeight = dim;
	referenceImage.pixelType = Limereg_Grayscale_8;
	referenceImage.pyramidImage = Limereg_NotPyramidized;

	struct Limereg_Image templateImage;
	referenceImage.pixelBuffer = imgTmp;
	referenceImage.imageWidth = dim;
	referenceImage.imageHeight = dim;
	referenceImage.pixelType = Limereg_Grayscale_8;
	referenceImage.pyramidImage = Limereg_NotPyramidized;

	struct Limereg_TrafoLimits trafoLimits;
	trafoLimits.maxRotationDeg = 5.0;
	trafoLimits.maxTranslationPercent = 10.0;

	struct Limereg_TrafoParams registrResult;

	enum Limereg_RetCode ret = Limereg_RegisterImage(
			&referenceImage,
			&templateImage,
			&trafoLimits,
			0,
			NULL,
			&registrResult,
			&distanceMeasure,
			&iterationAmount,
			NULL
			);

	//Verify registration result
	if(LIMEREG_RET_SUCCESS == ret)
	{
		const float expextedRot = 0.0f;
		printf("retcode=%i, tx=%f px, ty=%f px, rot=%f °, SSD=%f, iterations: %u\n",
				ret,
				registrResult.xShift,
				registrResult.yShift,
				registrResult.rotationDeg,
				distanceMeasure,
				iterationAmount);
		printf("expected tx=%i, ty=%i, rot=%f\n", xoff, yoff, expextedRot);

		const float maxTransErrPix = 1;
		const float maxRotErrDeg = 0.1;
		if(maxTransErrPix > fabs(registrResult.xShift-xoff) && maxRotErrDeg > fabs(registrResult.rotationDeg-expextedRot))
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

int main(void)
{
	//Execute test
	bool result = outputTestResult(test_findAlignment());
	return (result ? RET_SUCCESS : RET_FAILED);
}
