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

#include "limereg_common.h"
#include "limereg.h"

#include "CRegistrator.h"

static const char gAppVersion[] = VERSION;

const char* Limereg_GetVersion()
{
	return gAppVersion;
}

int CheckImageSize(Limereg_PixelBytearray* Image)
{
	if(NULL == Image->pixelBuffer)
	{
		return LIMEREG_RET_RCV_NULLPTR;
	}

	if(0==Image->imageWidth || 0==Image->imageHeight)
	{
		return LIMEREG_RET_IMAGE_TOO_SMALL;
	}

	if(Image->imageWidth != Image->imageHeight)
	{
		return LIMEREG_RET_IMAGES_MUST_BE_SQUARE;
	}
	return LIMEREG_RET_SUCCESS;
}

int CheckImageSize(Limereg_PixelBytearray* ImageA, Limereg_PixelBytearray* ImageB)
{
	int checkResult=0;

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

	if(false == (ImageA->imageWidth == ImageA->imageHeight == ImageB->imageWidth ==ImageB->imageHeight))
	{
		return LIMEREG_RET_IMAGES_MUST_HAVE_SAME_SIZE;
	}

	return LIMEREG_RET_SUCCESS;
}

int Limereg_RegisterImage(
		Limereg_PixelBytearray* imgRef,
		Limereg_PixelBytearray* imgTmp,
		unsigned int maxIterations,
		Limereg_TrafoLimits* registrResultLimits,
		unsigned int levelCount,
		double stopSensitivity,
		unsigned int flags /*unused in the current version, things like an affine registration might be added here*/,
		Limereg_TrafoParams* registrResult,
		double* distanceMeasure,
		unsigned int* iterationAmount,
		unsigned int* iterationsPerLevel
		)
{
	int ret = CheckImageSize(imgRef, imgTmp);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}

	//Avoid errors when we add x and y dimensions, make clear where it is used in a shared way.
	unsigned int xyDimension = imgRef->imageWidth;

	//Check for nullpointers (images are already checked in CheckImageSize())
	if(NULL == registrResultLimits || NULL == registrResult || NULL == distanceMeasure
		|| NULL == iterationAmount || NULL == iterationsPerLevel
	  )
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


	//When levelcount is set to 0: Autodetect of amount of levels (multilevel pyramid)
	//todo: avoid redundancy to CRegistrationController
	if(0 == levelCount)
	{
		levelCount = uint32_t(ceil(log2(t_reg_real(xyDimension / gui_LEVELCOUNT_AUTOTETECT_DIVISOR))));
	}

	if(0 == stopSensitivity)
	{
		//todo: remove ugly redundancy to CRegistrationController
		const t_reg_real DEF_CMD_PARAM_STOPSENS = 0.7f;
		stopSensitivity = DEF_CMD_PARAM_STOPSENS;
	}

	//Execute the registration algorithm
	t_reg_real aRegParams[3] = {0, 0, 0};
	CRegistrator oRegistrator;
	*iterationAmount = oRegistrator.RegisterImages(
			xyDimension,
			maxIterations,
			maxRotationRad,
			maxTranslationPercent,
			levelCount,
			stopSensitivity,
			imgRef,
			imgTmp,
			aRegParams,
			*distanceMeasure,
			iterationsPerLevel
			);

	//Pass back the registration result
	registrResult->rotationDeg = aRegParams[0] * 180.0f / M_PI;
	registrResult->xShift = aRegParams[1];
	registrResult->yShift = aRegParams[2];

	return LIMEREG_RET_SUCCESS;
}

int Limereg_TransformImage(
		Limereg_PixelBytearray* imgSrc,
		Limereg_TrafoParams* trafoParams,
		Limereg_PixelBytearray* imgDst
		)
{
	int ret = CheckImageSize(imgSrc, imgDst);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}

	//Check for nullpointers (images are already checked in CheckImageSize())
	if(NULL == trafoParams)
	{
		return LIMEREG_RET_RCV_NULLPTR;
	}

	//Avoid errors when we add x and y dimensions, make clear where it is used in a shared way.
	unsigned int xyDimension = imgSrc->imageWidth;

	//Perform the image processing operation
	t_reg_real aRegParams[3] = {
			(t_reg_real)trafoParams->rotationDeg * M_PI / 180.0f,
			(t_reg_real)trafoParams->xShift,
			(t_reg_real)trafoParams->yShift
			};

	CRegistrator oRegistrator;
	oRegistrator.TransformImage(xyDimension, aRegParams, imgSrc, imgDst);

	return LIMEREG_RET_SUCCESS;
}

int Limereg_CalculateDiffImage(
		Limereg_PixelBytearray* imgRef,
		Limereg_PixelBytearray* imgTmp,
		Limereg_PixelBytearray* imgDst
		)
{
	int ret = CheckImageSize(imgRef, imgTmp);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}
	int ret = CheckImageSize(imgTmp, imgDst);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}

	//Avoid errors when we add x and y dimensions, make clear where it is used in a shared way.
	int xyDimension = imgRef->imageWidth;

	//Perform the image processing operation
	CRegistrator oRegistrator;
	oRegistrator.CalculateDiffImage(xyDimension, imgRef->pixelBuffer, imgTmp->pixelBuffer, imgDst->pixelBuffer);

	return LIMEREG_RET_SUCCESS;
}

