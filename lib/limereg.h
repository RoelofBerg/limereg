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

#ifndef _LIMEREG_H_
#define _LIMEREG_H_

#ifdef __cplusplus
extern "C" {
#endif

//General return codes
#define LIMEREG_RET_SUCCESS 0				//! No error
#define LIMEREG_RET_INTERNAL_ERROR 1		//! Unexpected internal error
//Parameter parsing
#define LIMEREG_RET_IMAGE_TOO_SMALL 100		//! xDimension or yDimension smaller than alloweg (e.g. 0)
#define LIMEREG_RET_MAX_ROT_INVALID 101		//! Parameter maxRotationDeg invalid (too big or small)
#define LIMEREG_RET_MAX_TRANS_INVALID 102	//! Parameter maxTranslationPercent invalid (too big or small)
//Registration processing
#define LIMEREG_RET_ABORT_MAXITER_EXCEEDED 200	//! The registration algorithm took more iterations than allowed by maxIterations and was aborted
//Temporary codes
#define LIMEREG_RET_IMAGES_MUST_BE_SQUARE 9999	//! Currently xDimension must be equal to yDimension (this limitation will be removed soon)

/*!
 * Register two images. Find out the horizontal/vertical shift and the rotation for the best possible
 * overlay of both images.
 *
 * The images are treated as byte array where one byte matches the luminance (grey-value) of one pixel.
 *
 * @param[in] imgRef Reference image (image to be matched against)
 * @param[in] imgTmp Template image (image to be shifted/rotated until it matches to the reference image)
 * @param[in] xDimension Common horizontal image dimension of imgRef and imgTmp
 * @param[in] yDimension Common vertical image dimension of imgRef and imgTmp
 * @param[in] maxIterations Maximum amount of iterations to abort the algorithm
 * @param[in] maxRotationDeg Maximum rotation allowed in degree in the range 0<=maxRot<=180 (the algorithm will stay in this boundary, it will not abort)
 * @param[in] maxTranslationPercent Maximum translation allowed in percent of the horizontal image dimension (the algorithm will stay in this boundary, it will not abort)
 * @param[in] int levelCount Amount of levels of coarser images (0=autodetect)
 * @param[in] stopSensitivity Sensitivity of the STOP criteria (0=autotetect) See Gill, Murray, Wright: Practical Optimization
 * @param[out] xShift Registration result: Horizontal shift for best detected image alignment
 * @param[out] yShift Registration result: Vertical shift for best detected image alignment
 * @param[out] rotation Registration result: Rotation for best detected image alignment in degrees
 * @param[out] distanceMeasure For informational purposes. The distance measure of the final result (the lower, the better the images are aligned, not comparable between images of different size)
 * @param[out] iterationAmount Amount of algorithm iterations passed
 * @return return code (0=success, see LIMEREG_RET...)
 */
int Limereg_RegisterImage(
		unsigned char* imgRef,
		unsigned char* imgTmp,
		unsigned int xDimension,
		unsigned int yDimension,
		unsigned int maxIterations,
		double maxRotationDeg,
		double maxTranslationPercent,
		unsigned int levelCount,
		double stopSensitivity,
		double* xShift,
		double* yShift,
		double* rotation,
		double* distanceMeasure,
		unsigned int* iterationAmount
		);

/*!
 * Apply shift and rotation obtained by Limereg_RegisterImage to an image.
 *
 * The images are treated as byte array where one byte matches the luminance (grey-value) of one pixel.
 *
 * @param[in] imgSrc Source image to be shifted/rotated
 * @param[in] xDimension Horizontal image dimension of imgSrc
 * @param[in] yDimension Vertical image dimension of imgSrc
 * @param[out] xShift Horizontal shift in pixels
 * @param[out] yShift Vertical shift in pixels
 * @param[out] rotation Rotation in degrees
 * @param[out] imgDst Result: Output image will be written to here (same image dimensions as the imgSrc).
 * @return return code (0=success, see LIMEREG_RET...)
 */
int Limereg_TransformImage(
		unsigned char* imgSrc,
		unsigned int xDimension,
		unsigned int yDimension,
		double xShift,
		double yShift,
		double rotation,
		unsigned char* imgDst
		);

/*!
 * Calculate the difference image between reference and the template image.
 *
 * The images are treated as byte array where one byte matches the luminance (grey-value) of one pixel.
 *
 * @param[in] imgRef Reference image (image to be matched against)
 * @param[in] imgTmp Template image (image to be shifted/rotated until it matches to the reference image)
 * @param[in] xDimension Common horizontal image dimension of imgRef and imgTmp
 * @param[in] yDimension Common vertical image dimension of imgRef and imgTmp
 * @param[out] imgDst Result: Output image will be written to here (same image dimensions as the input images).
 * @return return code (0=success, see LIMEREG_RET...)
 */
int Limereg_CalculateDiffImage(
		unsigned char* imgRef,
		unsigned char* imgTmp,
		unsigned int xDimension,
		unsigned int yDimension,
		unsigned char* imgDst
		);


#ifdef __cplusplus
}
#endif

#endif //_LIMEREG_H_
