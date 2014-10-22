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

/*
 * myprintf.cpp
 *
 * CODE GENERATED BY MATLAB CODER (THE HUMAN READABILITY IS THEREFORE LIMITED)
 *
 */

#include "stdafx.h"

/* Include files */
#include "rt_nonfinite.h"
 
#include "diffimg.h"
 
#include "gaussnewton.h"
#include "gen_example_data.h"
#include "generatePyramidPC.h"
 
#include "jacobian.h"
 
#include "myprintf.h"
 
 
#include "ssd.h"
 
 
 
#include "transform.h"
 
 

/* Custom Source Code */
#include "pseudo_stdafx.h"             //precompiled header not possible because of include position of matlab

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

void h_myprintf(real32_T flt)
{
  char_T formatString[10];
  int32_T i32;
  static const char_T b_formatString[10] = { '%', 's', ' ', '%', 'f', ' ', '%',
    's', '\x0a', '\x00' };

  char_T cv28[31];
  static const char_T cv29[31] = { 'S', 'w', 'i', 't', 'c', 'h', 'e', 'd', ' ',
    't', 'o', ' ', 'f', 'i', 'n', 'e', 'r', ' ', 'l', 'e', 'v', 'e', 'l', ' ',
    'a', 'f', 't', 'e', 'r', ' ', '\x00' };

  char_T cv30[12];
  static const char_T cv31[12] = { 'i', 't', 'e', 'r', 'a', 't', 'i', 'o', 'n',
    's', '.', '\x00' };

  /* Matlab coder compilation: */
  for (i32 = 0; i32 < 10; i32++) {
    formatString[i32] = b_formatString[i32];
  }

  for (i32 = 0; i32 < 31; i32++) {
    cv28[i32] = cv29[i32];
  }

  for (i32 = 0; i32 < 12; i32++) {
    cv30[i32] = cv31[i32];
  }

  printf(formatString, cv28, flt, cv30);
}

void myprintf(const emxArray_char_T *strL, real32_T flt, const emxArray_char_T
              *strR)
{
  char_T formatString[10];
  int32_T i37;
  static const char_T b_formatString[10] = { '%', 's', ' ', '%', 'f', ' ', '%',
    's', '\x0a', '\x00' };

  char_T b_strL[2];
  int32_T strL_idx_0;
  int32_T loop_ub;
  char_T b_strR[2];

  /* Matlab coder compilation: */
  for (i37 = 0; i37 < 10; i37++) {
    formatString[i37] = b_formatString[i37];
  }

  strL_idx_0 = strL->size[0];
  i37 = 0;
  while (i37 <= 0) {
    loop_ub = strL_idx_0 - 1;
    for (i37 = 0; i37 <= loop_ub; i37++) {
      b_strL[i37] = strL->data[i37];
    }

    i37 = 1;
  }

  b_strL[1] = '\x00';
  strL_idx_0 = strR->size[0];
  i37 = 0;
  while (i37 <= 0) {
    loop_ub = strL_idx_0 - 1;
    for (i37 = 0; i37 <= loop_ub; i37++) {
      b_strR[i37] = strR->data[i37];
    }

    i37 = 1;
  }

  b_strR[1] = '\x00';
  printf(formatString, b_strL, flt, b_strR);
}

/* End of code generation (myprintf.cpp) */
