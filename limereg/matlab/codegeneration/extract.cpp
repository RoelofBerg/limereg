/* =====================================
=== LIMEREG - Lightweight Image Registration ===
========================================

Forked by Roelof Berg, Berg Solutions (rberg@berg-solutions.de) from the project
FIMREG. FIMREG was written for a distributed calculation on the PCIe card DSPC-8681
of Advantech. LIMEREG does not use DSPs and can be run without special hardware.

THIS IS A LIMITED RESEARCH PROTOTYPE. Documentation: www.berg-solutions.de/limereg.html

------------------------------------------------------------------------------

Copyright (c) 2014, RoelofBerg
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

/*
 * extract.cpp
 *
 * Code generation for function 'extract'
 *
 * C source code generated on: Mon Jul 02 10:11:21 2012
 *
 */

#include "stdafx.h"

/* Include files */
#include "rt_nonfinite.h"
#include "calcDSPLayout.h"
#include "diffimg.h"
#include "dstr_ssd.h"
#include "gaussnewton.h"
#include "gen_example_data.h"
#include "generatePyramidPC.h"
#include "get_current_time_in_sec.h"
#include "jacobian.h"
#include "jacobianOnTarget.h"
#include "myprintf.h"
#include "notiifyFinishedOnTarget.h"
#include "sendToTarget.h"
#include "ssd.h"
#include "ssdOnTarget.h"
#include "start_jacobianOnTarget.h"
#include "start_ssdOnTarget.h"
#include "transform.h"
#include "transmitImageData.h"
#include "waitUntilTargetReady.h"
#include "extract.h"
#include "dspreg_emxutil.h"

/* Custom Source Code */
#include "pseudo_stdafx.h"             //precompiled header not possible because of include position of matlab

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */

uint32_T getImagePartSize(const uint32_T BoundBox[4])
{
	  uint32_T width = BoundBox[1] - BoundBox[0];
	  return ((BoundBox[3] - BoundBox[2]) + 1U) * (width + 1U);
}

/**
 * Extracts the relevant parts of an image (for a DSP).
 */
void extract(const uint8_T *ImgVec, const uint32_T BoundBox[4], uint32_T m, CBufferedWriter& BufferedWriter)
{
	  uint32_T picXS;
	  uint32_T width = BoundBox[1] - BoundBox[0];
	  uint32_T yoff = ((BoundBox[3] - BoundBox[2]) + 1U) * (width + 1U);

	  uint32_T ip = 0U;
	  //(openmp unnecessary because pc<->dsp wil not be measured)
	  for(uint32_T picY=BoundBox[2]-1; picY<BoundBox[3]; picY++)
	  {
	    yoff = (picY) * m;
	    picXS = yoff + BoundBox[0];
	    yoff += BoundBox[1];

		/*
		Matlab hatte ein if generiert, macht das Sinn ?
	    if (picXS > yoff) {
	      i7 = 1;
	      i8 = 0;
	    } else {
	      i7 = (int32_T)picXS;
	      i8 = (int32_T)yoff;
	    }
		dann wurde als Laenge i8-i7+1 verwendet ...
		*/

		//memcpy to send buffer (e.g. when PCIe transfer: directly to a kernel buffer in the non-paged memory pool that will be used for DMA)
	    BufferedWriter.Enqueue(&ImgVec[picXS - 1], yoff - picXS + 1);

	    ip = (ip + width) + 1U;
	  }
}

/* End of code generation (extract.cpp) */
