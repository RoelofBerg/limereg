/* =====================================
=== LIMEREG - Lightweight Image Registration ===
========================================

Forked from the project FIMREG, which was written for a distributed calculation on the PCIe card DSPC-8681 of Advantech. LIMEREG does not use DSPs and can
be run on an ordinary PC without special hardware. FIMREG was originally developed by by Roelof Berg, Berg Solutions (rberg@berg-solutions.de) with support
from Lars Koenig, Fraunhofer MEVIS (lars.koenig@mevis.fraunhofer.de) and Jan Ruehaak, Fraunhofer MEVIS (jan.ruehaak@mevis.fraunhofer.de).

THIS IS A LIMITED RESEARCH PROTOTYPE. Documentation: www.berg-solutions.de/limereg.html

------------------------------------------------------------------------------

Copyright (c) 2014, Roelof Berg
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of the owner nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------------*/

//precompiled header (MUST BE THE FIRST ENTRY IN EVERY CPP FILE)
#include "stdafx.h"

#include "../limereg_common.h"
#include "limereg.h"

#include "../CRegistrator.h"

using namespace Limereg;

namespace Limereg {

static const char gAppVersion[] = VERSION;

Limereg_RetCode CheckImageSize(const Limereg_Image* Image)
{
	if(NULL == Image->pixelBuffer)
	{
		return LIMEREG_RET_RCV_NULLPTR;
	}

	if(0==Image->imageWidth || 0==Image->imageHeight)
	{
		return LIMEREG_RET_IMAGE_TOO_SMALL;
	}

	return LIMEREG_RET_SUCCESS;
}

Limereg_RetCode CheckImageSize(const Limereg_Image* ImageA, const Limereg_Image* ImageB)
{
	Limereg_RetCode checkResult=LIMEREG_RET_SUCCESS;

	checkResult=CheckImageSize(ImageA);
	if(LIMEREG_RET_SUCCESS != checkResult)
	{
		return checkResult;
	}

	checkResult=CheckImageSize(ImageB);
	if(LIMEREG_RET_SUCCESS != checkResult)
	{
		return checkResult;
	}

	if(false == ((ImageA->imageWidth == ImageB->imageWidth) && (ImageA->imageHeight == ImageB->imageHeight)))
	{
		return LIMEREG_RET_IMAGES_MUST_HAVE_SAME_SIZE;
	}

	return LIMEREG_RET_SUCCESS;
}

}	//Namespace Limereg

const char* Limereg_GetVersion()
{
	return gAppVersion;
}

Limereg_RetCode Limereg_RegisterImage(
		const Limereg_Image* referenceImage,
		const Limereg_Image* templateImage,
		const Limereg_TrafoLimits* registrResultLimits,
		const unsigned int flags /*unused in the current version, things like an affine registration might be added here*/,
		const Limereg_AdvancedRegControl* advancedCtrl,
		Limereg_TrafoParams* registrResult,
		double* distanceMeasure,
		unsigned int* iterationAmount,
		unsigned int* iterationsPerLevel
		)
{
	Limereg_RetCode ret = CheckImageSize(referenceImage, templateImage);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}

	//Avoid errors when we add x and y dimensions, make clear where it is used in a shared way.
	unsigned int xyDimension = referenceImage->imageWidth;

	//Check for nullpointers (images are already checked in CheckImageSize())
	if(NULL == registrResultLimits) //Note: The other parameters are allowed to be NULL (meaning default/ignore)
	{
		return LIMEREG_RET_RCV_NULLPTR;
	}

	//Sanity checks
	if(180.0f<registrResultLimits->maxRotationDeg || 0.0f>registrResultLimits->maxRotationDeg)
	{
		return LIMEREG_RET_MAX_ROT_INVALID;
	}
	double maxRotationRad = registrResultLimits->maxRotationDeg * M_PI / 180.0f;

	double maxTranslationPercent = registrResultLimits->maxTranslationPercent;
	if(100.0f<maxTranslationPercent || 0.0f>maxTranslationPercent)
	{
		return LIMEREG_RET_MAX_TRANS_INVALID;
	}

	unsigned int maxIterations=0;
	double stopSensitivity = 0;
	unsigned int pyramidLevelCount = 0;
	unsigned int skipFineLevelCount = 0;
	t_reg_real aRegStartParams[3] = {0, 0, 0};
	if(NULL != advancedCtrl)
	{
		if(NULL != advancedCtrl->stencilImage)
		{
			return LIMEREG_RET_STENCIL_NOT_IMPL_YET;
		}

		maxIterations = advancedCtrl->maxIterations;
		pyramidLevelCount = advancedCtrl->pyramidLevelCount;
		stopSensitivity = advancedCtrl->stopSensitivity;
		skipFineLevelCount = advancedCtrl->skipFineLevelCount;
		if(NULL != advancedCtrl->startParameters)
		{
			//todo: Check whether startparams are within the bounds and return LIMEREG_RET_STARTPARAM_INVALID if necessary.
			aRegStartParams[0] = advancedCtrl->startParameters->rotationDeg * M_PI / 180.0f;
			aRegStartParams[1] = advancedCtrl->startParameters->xShift;
			aRegStartParams[2] = advancedCtrl->startParameters->yShift;
		}
	}

	//When levelcount is set to 0: Autodetect of amount of levels (multilevel pyramid)
	//todo: avoid redundancy to CRegistrationController
	if(0 == pyramidLevelCount)
	{
		uint32_t iDim = (referenceImage->imageHeight < referenceImage->imageWidth) ? referenceImage->imageWidth : referenceImage->imageHeight;
		pyramidLevelCount = uint32_t(ceil(log2(t_reg_real(iDim / gui_LEVELCOUNT_AUTOTETECT_DIVISOR))));
	}

	if(0 == stopSensitivity)
	{
		//todo: remove ugly redundancy to CRegistrationController
		const t_reg_real DEF_CMD_PARAM_STOPSENS = 0.7f;
		stopSensitivity = DEF_CMD_PARAM_STOPSENS;
	}

	if(0 == maxIterations)
	{
		maxIterations = 150;	//ToDo: Share constants with exe (also param checking seems a bit redundant to me here ...)
	}

	//Execute the registration algorithm
	t_reg_real aRegResult[3] = {0, 0, 0};
	double distanceMeasureResult=0;
	CRegistrator oRegistrator;
	unsigned int iterationAmountResult = oRegistrator.RegisterImages(
			referenceImage->imageWidth,
			referenceImage->imageHeight,
			maxIterations,
			maxRotationRad,
			maxTranslationPercent,
			pyramidLevelCount,
			skipFineLevelCount,
			stopSensitivity,
			referenceImage->pixelBuffer,
			templateImage->pixelBuffer,
			aRegStartParams,
			aRegResult,
			distanceMeasureResult,
			iterationsPerLevel
			);

	//Pass back the registration result
	//||  || NULL == distanceMeasure || NULL == iterationAmount
	if(NULL != registrResult)
	{
		registrResult->rotationDeg = aRegResult[0] * 180.0f / M_PI;
		registrResult->xShift = aRegResult[1];
		registrResult->yShift = aRegResult[2];
	}

	if(NULL != distanceMeasure)
	{
		*distanceMeasure = distanceMeasureResult;
	}

	if(NULL != iterationAmount)
	{
		*iterationAmount = iterationAmountResult;
	}

	return LIMEREG_RET_SUCCESS;
}

Limereg_RetCode Limereg_CreatePyramid(
		const Limereg_Image* sourceImage,
		const struct Limereg_TrafoLimits* registrResultLimits,
		const unsigned int flags,
		const struct Limereg_AdvancedRegControl* advancedCtrl,
		Limereg_Image* pyramidImage
		)
{
	if(Limereg_Image::Limereg_NotPyramidized != sourceImage->pyramidImage)
	{
		return LIMEREG_RET_INVALID_PYRAMID_TYPE;
	}

	//Workaround as long as this functionality isn't implemented:
	//Shallow copy the image. For the caller it looks like Create- and DeletePyramid would function already.
	//(Except that the algorithm performance is low because the pyramid won't be cached and will allways be created newly.)
	*pyramidImage = *sourceImage;
	pyramidImage->pyramidImage = Limereg_Image::Limereg_Pyramidized;

	return LIMEREG_RET_SUCCESS;
}

Limereg_RetCode Limereg_DeletePyramid(
		const Limereg_Image* pyramidImage
		)
{
	if(Limereg_Image::Limereg_Pyramidized != pyramidImage->pyramidImage)
	{
		return LIMEREG_RET_INVALID_PYRAMID_TYPE;
	}

	//ToDo: Add explicit pyramid image handling
	//For now we just ignore this request becaus nothing has been created ...

	return LIMEREG_RET_SUCCESS;
}

Limereg_RetCode Limereg_TransformImage(
		const Limereg_Image* sourceImage,
		const Limereg_TrafoParams* trafoParams,
		Limereg_Image* transformedImage
		)
{
	Limereg_RetCode ret = CheckImageSize(sourceImage, transformedImage);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}

	//Check for nullpointers (images are already checked in CheckImageSize())
	if(NULL == trafoParams)
	{
		return LIMEREG_RET_RCV_NULLPTR;
	}

	//Perform the image processing operation
	t_reg_real aRegParams[3] = {
			(t_reg_real)trafoParams->rotationDeg * M_PI / 180.0f,
			(t_reg_real)trafoParams->xShift,
			(t_reg_real)trafoParams->yShift
			};

	CRegistrator oRegistrator;
	oRegistrator.TransformImage(
			sourceImage->imageWidth,
			sourceImage->imageHeight,
			aRegParams,
			sourceImage->pixelBuffer,
			transformedImage->pixelBuffer
			);

	return LIMEREG_RET_SUCCESS;
}

Limereg_RetCode Limereg_CalculateDiffImage(
		const Limereg_Image* referenceImage,
		const Limereg_Image* templateImage,
		Limereg_Image* differenceImage
		)
{
	Limereg_RetCode ret = CheckImageSize(referenceImage, templateImage);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}
	ret = CheckImageSize(templateImage, differenceImage);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}

	//Perform the image processing operation
	CRegistrator oRegistrator;
	oRegistrator.CalculateDiffImage(referenceImage->imageWidth, referenceImage->imageHeight,
			referenceImage->pixelBuffer, templateImage->pixelBuffer, differenceImage->pixelBuffer);

	return LIMEREG_RET_SUCCESS;
}
