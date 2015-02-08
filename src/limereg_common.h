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

/**
* Typedefs, constants and compile options for our application
*/

#pragma once

//-- C99 compliance--------------------------------------------------------------

//Typedefs necessary to make Microsofts Non-Standard-Compliant-Compiler more conform to the C99 standard
#if defined(_MSC_VER) 
typedef signed __int8 int8_t; 
typedef signed __int16 int16_t; 
typedef signed __int32 int32_t; 
typedef signed __int64 int64_t; 
typedef unsigned __int8 uint8_t; 
typedef unsigned __int16 uint16_t; 
typedef unsigned __int32 uint32_t; 
typedef unsigned __int64 uint64_t;
#else
//Linux
#include <stdint.h>
#endif 

//-- Typedefs ------------------------------------------------------------------

#include "CMatlabArray.h"

typedef double t_reg_real;
typedef TMatlabArray_Double TMatlabArray_Reg_Real;
typedef uint8_t t_pixel;
typedef TMatlabArray_UInt8 TMatlabArray_Pixel;

//-- Exceptions ----------------------------------------------------------------

#include "limereg_exceptions.h"

//-- Constants -----------------------------------------------------------------

//NAN depending on type of t_reg_real
#if WIN32
	static const t_reg_real REG_REAL_NAN = _Nan._Double;
#else
	static const t_reg_real REG_REAL_NAN = NAN;
#endif

// RETURN VALUES
/** Application result values (console app) */
static const int32_t APP_RET_SUCCESS = 0;
/** Application result values (console app) */
static const int32_t APP_RET_ERROR = 1;

//CONSOLE OUTPUT
/** String constant (message for console output) */
static const string APP_ERR_EXCEPTION = "Unknown error. Terminating.";

//GRAFICS OUTPUT
/** Max. image size on screen */
static const uint32_t APP_MAX_IMG_SIZE = 300;

//SPECIAL CHARACTERS
/** String constant (special character for console output) */
static const string CRLF = "\n";
/** String constant (special character for console output) */
static const string DIR_DELIM = "/";
/** String constant (special character for console output) */
static const string SPACE = " ";

//APPLICATION VERSION
static const uint32_t APP_VERSION_MAJOR = 1;
static const uint32_t APP_VERSION_MINOR = 2;
static const uint32_t APP_VERSION_SUBMINOR = 0;

//APPLICATION SPECIFIC CONSTANTS
static const uint32_t gui_LEVELCOUNT_AUTOTETECT_DIVISOR=32;		//< log2(ImageWidth / [this constant]) = default amount of levels (multilevel autodetection)
