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

/*! @page liblimereg Library for lightweight image registration.

   Software development library, that performs a 2D, rigid image registration on two greyscale images and outputs either the transformation parameters or the registered image.

   Sourcecode:
   http://embeddedsoftwarearchitecture.com/limereg.html

   @section usage Usage Information
   Link with -mlimereg and #include <limereg.h>. API documentation see: man limereg.h

   @section copyright Copyright
   Copyright 2014, Roelof Berg, Licensed under the 3-clause BSD license at http://berg-solutions.de/limereg-license.html. Credit goes to Lars Koenig and Jan Ruehaak from Fraunhofer MEVIS in Germany.

   @section examples Examples
   In the source distribution at http://embeddedsoftwarearchitecture.com/limereg.html the following folders contain examples:
   tests: C written test automation that can be used as example code.
   exe: C++ written commandline utility using liblimereg.
*/

/*! \file limereg.h
    \brief Library for lightweight image registration

    Software development library, that performs a 2D, rigid image registration on two greyscale images and outputs either the transformation parameters or the registered image.

	Examples:

	In the source distribution at http://embeddedsoftwarearchitecture.com/limereg.html the following folders contain examples:

	tests: C written test automation that can be used as example code.

	exe: C++ written commandline utility using liblimereg.

    Copyright 2014, Roelof Berg, Licensed under the 3-clause BSD license at http://berg-solutions.de/limereg-license.html. Credit goes to Lars Koenig and Jan Ruehaak from Fraunhofer MEVIS in Germany.

	Sourcecode:
	http://embeddedsoftwarearchitecture.com/limereg.html
*/

#ifdef __cplusplus
extern "C" {
#endif

enum Limereg_RetCode
{
	//General return codes
	LIMEREG_RET_SUCCESS=0,				//!< No error
	LIMEREG_RET_INTERNAL_ERROR=1,		//!< Unexpected internal error
	LIMEREG_RET_RCV_NULLPTR=2,			//!< An unexpected nullpointer was passed as an argument
	LIMEREG_RET_INVALID_PYRAMID_TYPE=3,	//!< The enum value of PyramidImage is invalid in the current context
	//Parameter parsing
	LIMEREG_RET_IMAGE_TOO_SMALL=100,	//!< xDimension or yDimension smaller than alloweg (e.g. 0)
	LIMEREG_RET_MAX_ROT_INVALID=101,	//!< The rotation in registrResultLimits is invalid (too big or small)
	LIMEREG_RET_MAX_TRANS_INVALID=102,	//!< The shift in registrResultLimits is invalid (too big)
	LIMEREG_RET_STARTPARAM_INVALID=103,	//!< The content of startParameters in Limereg_RegisterImage() is invalid. Check if the content is plausible and within the bounds of registrResultLimits.
	//Registration processing
	LIMEREG_RET_ABORT_MAXITER_EXCEEDED=200,	//!< The registration algorithm took more iterations than allowed by maxIterations and was aborted
	//Temporary codes
	LIMEREG_RET_STENCIL_NOT_IMPL_YET=9997, //!< Currently stencil images are unsupported and the pointer named stencilImage has to be set to 0
	/*deprecated: 9998*/
	LIMEREG_RET_IMAGES_MUST_HAVE_SAME_SIZE=9999	//!< Currently the images to be registered must both have the same size (this limitation will be removed soon)
};

/*! Flags to control the registration algorithm
 */
enum Limereg_Flags
{
	Limereg_Trafo_Rigid=0			//!< Rigid transformation: Supports horizontal, vertical shift and rotation. The size of the image area remains the same.
	//, Limereg_Trafo_Affine=2 ... coming soon ...
};

/*! \brief Pointer to pixeldata and image dimensions
 * Image buffer with data pointer and image dimensions.
 * The buffer consists of one byte per pixel of luminance data (greyscale).
 */
struct Limereg_Image
{
	enum PixelType							//!< The supported pixel interpretation types
	{
		Limereg_Grayscale_8=0				//!< Only luminance, no color, 8 bit per pixel
	} pixelType;							//!< How to interpret pixelBuffer
	enum PyramidImage						//!< Normal or pyramid image
	{
		Limereg_NotPyramidized=0,			//!< pixelBuffer contains a plain image. This is the usual case, Limereg will generate a multilevel pyramid internally.)
		Limereg_Pyramidized					//!< pixelBuffer contains a pyramid image, e.g. generated by Limereg_CreatePyramid().
	} pyramidImage;
	unsigned char* pixelBuffer;				//!< Byte array with luminance data (1 byte per pixel).
	unsigned int imageWidth;				//!< Horizontal image dimension
	unsigned int imageHeight;				//!< Vertical image dimension
};

/*! \brief Parameters for a rigid transformation
 * Parameters for a rigid transformation. That is horizontal and vertical shift and a rotation angle.
 * The image will retain it's dimensions, it will not be sheared, only shift and rotation are allowed.
 */
struct Limereg_TrafoParams
{
	double xShift;			//<! Horizontal shift in pixels (fractions of a pixel are allowed)
	double yShift;			//<! Vertical shift in pixels (fractions of a pixel are allowed)
	double rotationDeg;		//<! Image rotation in degrees

	//coming soon: For affine the above values will become unions and three more affine parameters will be added here.
	//Then: Read out when Trafo_Affine is passed to flags
};


/*! \brief Advanced parameters for controlling the image registration
 * This parameters allow the control of advanced parameters. Each parameter can be set to 0 for an
 * autodetect or default behavior. Also a nullpointer can be passed instead of this struct if for
 * all advanced parameters the autodetect/default value shall be used.
 */
struct Limereg_AdvancedRegControl
{
	unsigned int maxIterations;		//<! Maximum amount of iterations to abort the algorithm (0=default)
	double stopSensitivity;			//<! Sensitivity of the STOP criteria (0=autotetect, 0<x<1, the smaller x is, the harder the algorithm tries and the more time it takes) See Gill, Murray, Wright: Practical Optimization
	unsigned int pyramidLevelCount;		//<! Amount of levels of coarser images (0=autodetect)
	unsigned int skipFineLevelCount;	//<! Ignore the n finest levels (this is usually 0 = calculate up to the full image size)
	struct Limereg_TrafoParams* startParameters;	//<! Shift and rotation at which the very first iteration will start with (NULLPOINTER = Start at zero shift and rotation)
	struct Limereg_Image* stencilImage;	//<! Greyscale image for making particular pixels of T less important, should be grayscale and have the same dimensions as T (NULLPOINTER = ignore, every pixel has the same importance)
};

/*! \brief Result limits for a rigid transformation
 * Maximum allowed result (shift/rotation) for the rigid transformation result of the image registration.
 * The algorithm will stay in this boundary, it will not abort.
 *
 * The smaller you choose the values, the faster the algorithm will be and the less memory it will consume.
 *
 * BUT GIVE THE ALGORITHM ENOUGH SPACE TO WORK HERE. Allow more than the expected final registration result needs,
 * because it might happen that the iterative approach needs more freedom during the iterations than in the end.
 * If you ever have to troubleshoot a bad registration result, raise this parameters for testing.
 *
 * (There is some reserved space as we might add affine transformations in a future version.)
 */
struct Limereg_TrafoLimits
{
	double maxRotationDeg;			//<! Maximum rotation allowed in degree in the range 0<=maxRot<=180
	double maxTranslationPercent;	//<! Maximum translation allowed in percent of the horizontal image dimension (0<=maxTrans<=100)

	//coming soon for affine trafo: double maxMagnification;
	//Then: Read out when Trafo_Affine is passed to flags
};

/*! \brief Get library version.
 * Returns the package version of liblimereg in use by the current application.
 *
 * @return Pointer to a null terminated char array containing the version string.
 */
const char* Limereg_GetVersion();

/*! \brief Register two images.
 * Find out the horizontal/vertical shift and the rotation for the best possible
 * overlay of both images.
 *
 * The images are treated as byte array where one byte matches the luminance (grey-value) of one pixel.
 *
 * @param[in] referenceImage Reference image R (image to be matched against)
 * @param[in] templateImage Template image T (image to be shifted/rotated until it matches to the reference image)
 * @param[in] registrResultLimits Maximum shift and rotation allowed/expected. The algorithm will stay inside this boundaries.
 * @param[in] flags Variations in the mathematical approach (0=default)
 * @param[in] advancedCtrl Advanced parameters for fine tuning the algorithm (NULLPOINTER = Autodetect best settings)
 * @param[out] registrResult Registration result: Shift and rotation for the best detected image alignment (NULL = ignored)
 * @param[out] distanceMeasure For informational purposes. The distance measure of the final result (the lower, the better the images are aligned, not comparable between images of different size) (NULL = ignored)
 * @param[out] iterationAmount Total amount of algorithm iterations passed (NULL = ignored)
 * @param[out] iterationsPerLevel NULL = ignored / or a pointer to an array with levelCount (see above) elements that will be filled with the iterations needed on each level (beginning with the coarsest one)
 * @return return code
 */
enum Limereg_RetCode Limereg_RegisterImage(
		const struct Limereg_Image* referenceImage,
		const struct Limereg_Image* templateImage,
		const struct Limereg_TrafoLimits* registrResultLimits,
		const unsigned int flags,
		const struct Limereg_AdvancedRegControl* advancedCtrl,
		struct Limereg_TrafoParams* registrResult,
		double* distanceMeasure,
		unsigned int* iterationAmount,
		unsigned int* iterationsPerLevel
		);

/*! \brief Create multilevel pyramid (FOR ADVANCED USE ONLY. IF NOT USED, THE PYRAMID IS CREATED AUTOMATICALLY).
 * Create a multilevel pyramid. For a usual image registration this function should be AVOIDED.
 * In usual cases Limereg_RegisterImage() creates the multilevel pyramid internally, automatically.
 * The images of each pyramid level will be bigger than the (scaled) source image, because margins
 * will be added that allow Dirichlet boundary conditions to be used without jumps / pipeline hazards.
 *
 * The member Limereg_Image.PyramidImage of the output buffer will be set to Pyramidized and the data pointer
 * will point to the pyramid image data. The memory for the image data will be created on the heap by this
 * function, use Limereg_DeletePyramid() to free this memory again lateron.
 *
 * Important: The parameters registrResultLimits, flags and pyramidLevelCount have to be equal to the later
 * calls to Limereg_RegisterImage() and have to stay equal for every call to Limereg_RegisterImage().
 *
 * @param[in] sourceImage Image data for the finest level of the multilevel pyramid.
 * @param[in] registrResultLimits Maximum shift and rotation allowed/expected. The algorithm will stay inside this boundaries.
 * @param[in] flags Variations in the mathematical approach (0=default)
 * @param[in] advancedCtrl Advanced parameters for fine tuning the algorithm (NULLPOINTER = Autodetect best settings, maxIterations, stopSensitivity and startParameters are ignored here)
 * @param[out] pyramidImage Buffer of subsequent pyramid images, starting with the finest, proceeding up to the coarsest level
 *
 * @return return code
 */
enum Limereg_RetCode Limereg_CreatePyramid(
		const struct Limereg_Image* sourceImage,
		const struct Limereg_TrafoLimits* registrResultLimits,
		const unsigned int flags,
		const struct Limereg_AdvancedRegControl* advancedCtrl,
		struct Limereg_Image* pyramidImage
		);

/*! \brief Delete multilevel pyramid created by Limereg_CreatePyramid() (FOR ADVANCED USE ONLY. IF NOT USED, THE PYRAMID IS CREATED AUTOMATICALLY).
 * Delete a multilevel pyramid that was created by Limereg_CreatePyramid(). For a usual image registration
 * this function should be AVOIDED. In usual cases Limereg_RegisterImage() creates and deletes the multilevel
 * pyramid internally, automatically.
 *
 * The member Limereg_Image.PyramidImage of the output buffer is expected to have the value Pyramidized.
 *
 * @return return code
 */
enum Limereg_RetCode Limereg_DeletePyramid(
		const struct Limereg_Image* pyramidImage
		);

/*! \brief Rigid image transformation.
 * Apply shift and rotation obtained by Limereg_RegisterImage to an image.
 *
 * The images are treated as byte array where one byte matches the luminance (grey-value) of one pixel.
 *
 * @param[in] sourceImage Source image to be shifted/rotated
 * @param[in] trafoParams Shift and rotation to be applied
 * @param[out] transformedImage Result: Output image will be written to here (same image dimensions as the sourceImage).
 * @return return code
 */
enum Limereg_RetCode Limereg_TransformImage(
		const struct Limereg_Image* sourceImage,
		const struct Limereg_TrafoParams* trafoParams,
		struct Limereg_Image* transformedImage
		);

/*! \brief Generate difference image.
 * Calculate the difference image between reference and the template image (sum of squared differences).
 *
 * The images are treated as byte array where one byte matches the luminance (grey-value) of one pixel.
 *
 * @param[in] referenceImage Reference image (image to be matched against)
 * @param[in] templateImage Template image (image to be shifted/rotated until it matches to the reference image)
 * @param[out] differenceImage Result: Output image will be written to here. The passed width and height fields have to match the buffer space (width*heigth bytes).
 * @return return code
 */
enum Limereg_RetCode Limereg_CalculateDiffImage(
		const struct Limereg_Image* referenceImage,
		const struct Limereg_Image* templateImage,
		struct Limereg_Image* differenceImage
		);


#ifdef __cplusplus
}
#endif

#endif //_LIMEREG_H_
