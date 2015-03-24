#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "test_common.h"
#include <limereg.h>

#define dim 4096
unsigned char imgRef[dim*dim];
unsigned char imgTmp[dim*dim];

bool test_searchSubimage()
{
	enum Limereg_RetCode ret;

	printf("Test: Search subimage by using Limereg_CreatePyramid() and Limereg_RegisterImage().\n");

	/*
	Limereg is currently not yet capable of registering images with different size, which sould be
	necessary for finding subimages.

	Current version: Just test the advanced interfaces that will be used lateron for subimage search.
	Upcoming version: Perform a subimage search.
	   Usage (then):
	   - Create pyramid images of R and T.
	   - On the coarsest level register the smaller image to be searched (T) on the bigger image (R)
	   - Do this several times from different starting positions. E.g. by putting an equidistant grid of starting positions on R.
	   - The best (smallest SSD result) n% (e.g. 20%) of positions may survive for the next run on the next finer pyramid level.
	   - The next finer level should have a finer grid of starting positions. E.g. for each position that 'survived' above, create four new grid-positions (2x2) around the survived one.
	   - Repeat this steps recursively until the finest level has been reached.
	   - The best (smallest SSD) result points to the found subimage, if the resulting SSD is below a threshold.

	   See also: http://embedded-software-architecture.com/?p=183, post from 2015, March, 22d
	*/

	//Clear image buffers
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
	templateImage.pixelBuffer = imgTmp;
	templateImage.imageWidth = dim;
	templateImage.imageHeight = dim;
	templateImage.pixelType = Limereg_Grayscale_8;
	templateImage.pyramidImage = Limereg_NotPyramidized;

	struct Limereg_TrafoLimits trafoLimits;
	trafoLimits.maxRotationDeg = 5.0;
	trafoLimits.maxTranslationPercent = 10.0;

	struct Limereg_TrafoParams startParams;
	startParams.xShift = xoff/2;
	startParams.yShift = yoff/2;
	startParams.rotationDeg = 5;

	const unsigned int pyramidLvl=4;
	const unsigned int skipLvl=2;
	struct Limereg_AdvancedRegControl advancedRegControl;
	advancedRegControl.maxIterations = 50;
	advancedRegControl.pyramidLevelCount = pyramidLvl;
	advancedRegControl.skipFineLevelCount = skipLvl;
	advancedRegControl.startParameters = &startParams;
	advancedRegControl.stencilImage = NULL;

	const unsigned int flags=0;

	//Allocate custom pyramid images
	struct Limereg_Image referencePyramid;
	ret = Limereg_CreatePyramid(&referenceImage, &trafoLimits, flags, &advancedRegControl, &referencePyramid);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		printf("Limereg_CreatePyramid(R) retcode=%i (ERROR !)\n", ret);
		return false;	//Operating system will cleanup memory
	}

	struct Limereg_Image templatePyramid;
	ret = Limereg_CreatePyramid(&templateImage, &trafoLimits, flags, &advancedRegControl, &templatePyramid);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		printf("Limereg_CreatePyramid(T) retcode=%i (ERROR !)\n", ret);
		return false;	//Operating system will cleanup memory
	}

	struct Limereg_TrafoParams registrResult;
	double distanceMeasure=0;
	unsigned int iterationAmount=0;
	unsigned int iterationsPerLevel[100];
	ret = Limereg_RegisterImage(
			&referencePyramid,
			&templatePyramid,
			&trafoLimits,
			flags,
			&advancedRegControl,
			&registrResult,
			&distanceMeasure,
			&iterationAmount,
			iterationsPerLevel
			);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		printf("Limereg_RegisterImage() retcode=%i (ERROR !)\n", ret);
		return false;	//Operating system will cleanup memory
	}

	//Verify registration result
	const float expextedRot = 0.0f;
	printf("Limereg_RegisterImage retcode=%i, tx=%f px, ty=%f px, rot=%f °, SSD=%f, iterations: %u\n",
			ret,
			registrResult.xShift,
			registrResult.yShift,
			registrResult.rotationDeg,
			distanceMeasure,
			iterationAmount);
	const unsigned int scale = skipLvl*skipLvl;
	printf("Expected about tx=%i, ty=%i, rot=%f\n", xoff/scale, yoff/scale, expextedRot);

	const float maxTransErrPix = 1;
	const float maxRotErrDeg = 0.1;
	if(maxTransErrPix > fabs(registrResult.xShift-(xoff/scale)) && maxRotErrDeg > fabs(registrResult.rotationDeg-(expextedRot/scale)))
	{
		return true;
	}

	//Free memory
	ret = Limereg_DeletePyramid(&referencePyramid);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		printf("Limereg_DeletePyramid(R) retcode=%i (ERROR !)\n", ret);
		return false;	//Operating system will cleanup memory
	}

	ret = Limereg_DeletePyramid(&templatePyramid);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		printf("Limereg_DeletePyramid(T) retcode=%i (ERROR !)\n", ret);
		return false;	//Operating system will cleanup memory
	}

	return false;
}

int main(void)
{
	//Execute test
	bool result = outputTestResult(test_searchSubimage());
	return (result ? RET_SUCCESS : RET_FAILED);
}
