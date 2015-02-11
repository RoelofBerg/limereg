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

#pragma once

#include "limereg_common.h"

/**
* This class issues the matlab generated code to perform image registration and image manipulation necessary for image registration
*/
class CRegistrator
{
public:
	CRegistrator();
	virtual ~CRegistrator();

	uint32_t RegisterImages(uint32_t iPicDim, uint32_t iMaxIter, t_reg_real fMaxRotation, t_reg_real fMaxTranslation, uint32_t iLevelCount, t_reg_real fStopSensitivity, t_pixel* imgRef, t_pixel* imgTmp, t_reg_real (&aRegParams)[3], t_reg_real& fSSD);
	void TransformReferenceImage(uint32_t iPicDim, t_reg_real w[3], t_pixel* imgSrc, t_pixel* imgDst);
	void CalculateDiffImage(uint32_t iPicDim, t_pixel* imgRef, t_pixel* imgTmp, t_pixel* imgDst);
};
