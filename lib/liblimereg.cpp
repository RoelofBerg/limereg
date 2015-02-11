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

int CheckImageSize(unsigned int xDim, unsigned int yDim)
{
	if(0==xDim || 0== yDim)
	{
		return LIMEREG_RET_IMAGE_TOO_SMALL;
	}

	if(xDim != yDim)
	{
		return LIMEREG_RET_IMAGES_MUST_BE_SQUARE;
	}

	return LIMEREG_RET_SUCCESS;
}

int Limereg_RegisterImage(
		unsigned char* imgRef,
		unsigned char* imgTmp,
		unsigned int xDimension,
		unsigned int yDimension,
		unsigned int maxIterations,
		double maxRotationDegree,
		double maxTranslationPercent,
		unsigned int levelCount,
		double stopSensitivity,
		double* xShift,
		double* yShift,
		double* rotation,
		double* distanceMeasure,
		unsigned int* iterationAmount
		)
{
	int ret = CheckImageSize(xDimension, yDimension);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}

	//Avoid errors when we add x and y dimensions, make clear where it is used in a shared way.
	int xyDimension = xDimension;

	//Default parameters
	//todo: What about parameter checking ? We have this knowledge in CRegistrationController tightly bound to the boost parameter parsing (better first parse, then verify ...).

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
			maxRotationDegree,
			maxTranslationPercent,
			levelCount,
			stopSensitivity,
			imgRef,
			imgTmp,
			aRegParams,
			*distanceMeasure
			);

	//Pass back the registration result
	*xShift = aRegParams[0];
	*yShift = aRegParams[1];
	*rotation = aRegParams[2];

	return LIMEREG_RET_SUCCESS;
}

int Limereg_TransformReferenceImage(
		unsigned char* imgSrc,
		unsigned int xDimension,
		unsigned int yDimension,
		double xShift,
		double yShift,
		double rotation,
		unsigned char* imgDst
		)
{
	int ret = CheckImageSize(xDimension, yDimension);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}

	//Avoid errors when we add x and y dimensions, make clear where it is used in a shared way.
	int xyDimension = xDimension;

	//Perform the image processing operation
	t_reg_real aRegParams[3] = {(t_reg_real)xShift, (t_reg_real)yShift, (t_reg_real)rotation};
	CRegistrator oRegistrator;
	oRegistrator.TransformReferenceImage(xyDimension, aRegParams, imgSrc, imgDst);

	return LIMEREG_RET_SUCCESS;
}

int Limereg_CalculateDiffImage(
		unsigned char* imgRef,
		unsigned char* imgTmp,
		unsigned int xDimension,
		unsigned int yDimension,
		unsigned char* imgDst
		)
{
	int ret = CheckImageSize(xDimension, yDimension);
	if(LIMEREG_RET_SUCCESS != ret)
	{
		return ret;
	}

	//Avoid errors when we add x and y dimensions, make clear where it is used in a shared way.
	int xyDimension = xDimension;

	//Perform the image processing operation
	CRegistrator oRegistrator;
	oRegistrator.CalculateDiffImage(xyDimension, imgRef, imgTmp, imgDst);

	return LIMEREG_RET_SUCCESS;
}

