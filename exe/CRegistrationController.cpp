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
* limereg.cpp : Defines the entry point for the console application.
*/

//precompiled header (MUST BE THE FIRST ENTRY IN EVERY CPP FILE)
#include "stdafx.h"

//liblimereg shared object / DLL
#include <limereg.h>

#include "CRegistrationController.h"

#include "TimeMeasurement.h"

uint32_t guClipDarkNoise=0;

/**
 * Common error message text when image loading fails.
 */
const string gcsCannotLoadImage = "Cannot load image.";

//our own log2 command
//t_reg_real log2(t_reg_real d) {return log(d)/log(t_reg_real(2)) ;}

CRegistrationController::CRegistrationController()
: m_uiMaxIter(0),
  m_iLevelCount(0),
  m_fStopSens(REG_REAL_NAN),
  m_fMaxRotation(0),
  m_fMaxTranslation(0),
  m_bNoGui(false)
{
}

CRegistrationController::~CRegistrationController()
{
}

int CRegistrationController::Main(int argc, char *argv[])
{
	//Parse CMDLine
	if (!ParseParameters(argc, argv))
	{
		return APP_RET_ERROR;
	}

	RegisterImage();

	return APP_RET_SUCCESS;
}

void CRegistrationController::RegisterImage()
{
	// load template image
	// ToDo: This class is too big. Refactor out an image class containing all image handling (maybe outlayering OpenCV) (and possibly also the cmdline param stuff at the bottom of this file).
	IplImage* imgTmp = 0; 
	imgTmp=cvLoadImage(m_sTFilename.c_str(), 0);
	if(!CheckImage(imgTmp, m_sTFilename))
		exit(0);
	uint32_t iDim = imgTmp->height;
	t_pixel* pixelBytesTmp = (t_pixel *)imgTmp->imageData;

	// load reference image
	IplImage* imgRef = 0; 
	imgRef=cvLoadImage(m_sRFilename.c_str(), 0);
	if(!CheckImage(imgRef, m_sRFilename, iDim))
		exit(0);
	t_pixel* pixelBytesRef = (t_pixel *)imgRef->imageData;

//todo: the lib can do this, we can remove it here (verify !)
	//When levelcount is set to 0: Autodetect of amount of levels (multilevel pyramid)
	if(0 == m_iLevelCount)
	{
		m_iLevelCount = uint32_t(ceil(log2(t_reg_real(iDim / gui_LEVELCOUNT_AUTOTETECT_DIVISOR))));
		printf("Multilevel autodetection suggests %i levels.\n", m_iLevelCount);
	}

	//Measure calculation duration
	double dCPUTimeStart = get_cpu_time();
	double dWallTimeStart = get_wall_time();

	//Register Images
	const int ciRegParamCount = 3;
	t_reg_real SSD=0;
	uint32_t iNumIter=0;
	uint32_t* iterationsPerLevel = new uint32_t[m_iLevelCount];

	Limereg_Image refPixels;
	refPixels.pixelBuffer = pixelBytesRef;
	refPixels.imageWidth = (uint32_t)iDim;
	refPixels.imageHeight = (uint32_t)iDim;
	refPixels.pixelType = Limereg_Image::Grayscale_8;
	refPixels.pyramidImage = Limereg_Image::NotPyramidized;

	Limereg_Image tmpPixels;
	tmpPixels.pixelBuffer = pixelBytesTmp;
	tmpPixels.imageWidth = (uint32_t)iDim;
	tmpPixels.imageHeight = (uint32_t)iDim;
	refPixels.pixelType = Limereg_Image::Grayscale_8;
	refPixels.pyramidImage = Limereg_Image::NotPyramidized;

	Limereg_TrafoLimits trafoLimits;
	trafoLimits.maxRotationDeg = m_fMaxRotation;
	trafoLimits.maxTranslationPercent = m_fMaxTranslation;

	Limereg_AdvancedRegControl advRegCtrl;
	advRegCtrl.pyramidLevelCount = m_iLevelCount;
	advRegCtrl.skipFineLevelCount = 0;
	advRegCtrl.maxIterations = m_uiMaxIter;
	advRegCtrl.stopSensitivity = m_fStopSens;
	advRegCtrl.startParameters = NULL;

	Limereg_TrafoParams registrResult;
	Limereg_RegisterImage(
			&refPixels,
			&tmpPixels,
			&trafoLimits,
			0,
			&advRegCtrl,
			&registrResult,
			&SSD,
			&iNumIter,
			iterationsPerLevel
			);

	double xShift=registrResult.xShift;
	double yShift=registrResult.yShift;
	double rotation=registrResult.rotationDeg;

	//Output registration iterations
	for(int i=0; i<m_iLevelCount; i++)
	{
		printf("Switched to finer level after %u iterations.\n", iterationsPerLevel[i]);
	}

	//Measure calculation duration
	double dWallTimeStop = get_wall_time();
	double dCPUTimeStop = get_cpu_time();
	printf("\nRegistration duration: %.3fms wall clock time, %.3fms cpu time (wo. image I/O)\n",
			(dWallTimeStop-dWallTimeStart)*1000, (dCPUTimeStop-dCPUTimeStart)*1000
			);

	string sResult = (boost::format("Iterations = %1%, SSD = %2%, w = [%3% deg, %4% px, %5% px]") % iNumIter % SSD % rotation % xShift % yShift).str();
	printf("%s\n", sResult.c_str());

	bool bNeedTransImage = false;
	IplImage* imgTmpTrns;
	if(!m_bNoGui)
	{

		bNeedTransImage = (!m_bNoGui) || (0<m_sSaveTransImage.size());
		if(bNeedTransImage)
		{
			// calculate transformed template image
			imgTmpTrns=cvCloneImage(imgTmp);

			Limereg_Image tmpPixels;
			tmpPixels.pixelBuffer = (t_pixel *)imgTmp->imageData;
			tmpPixels.imageWidth = (uint32_t)iDim;
			tmpPixels.imageHeight = (uint32_t)iDim;
			refPixels.pixelType = Limereg_Image::Grayscale_8;
			refPixels.pyramidImage = Limereg_Image::NotPyramidized;

			Limereg_Image tmpTrnsPixels;
			tmpTrnsPixels.pixelBuffer = (t_pixel *)imgTmpTrns->imageData;
			tmpTrnsPixels.imageWidth = (uint32_t)iDim;
			tmpTrnsPixels.imageHeight = (uint32_t)iDim;
			refPixels.pixelType = Limereg_Image::Grayscale_8;
			refPixels.pyramidImage = Limereg_Image::NotPyramidized;

			//todo: examine retval
			Limereg_TransformImage(
					&tmpPixels,
					&registrResult,
					&tmpTrnsPixels
					);

			if(0<m_sSaveTransImage.size())
			{
				printf("Saving result to file '%s'.\n", m_sSaveTransImage.c_str());
				int iRetval = cvSaveImage(m_sSaveTransImage.c_str(), imgTmpTrns);
				/*if(0!=iRetval)
				{
					printf("ERROR, CANNOT SAVE IMAGE, CHECK FILENAME.\n");
				}*/
			}
		}

		// calculate difference image between ORIGINAL template image and reference image
		IplImage* imgDiffOrig;
		imgDiffOrig=cvCloneImage(imgTmp);


		Limereg_Image imgDiffOrigPixels;
		imgDiffOrigPixels.pixelBuffer = (t_pixel *)imgDiffOrig->imageData;
		imgDiffOrigPixels.imageWidth = (uint32_t)iDim;
		imgDiffOrigPixels.imageHeight = (uint32_t)iDim;
		refPixels.pixelType = Limereg_Image::Grayscale_8;
		refPixels.pyramidImage = Limereg_Image::NotPyramidized;

		//todo: examine retval
		Limereg_CalculateDiffImage(&refPixels, &tmpPixels, &imgDiffOrigPixels);

		// calculate difference image between TRANSFORMED template image and reference image
		IplImage* imgDiffFinal;
		imgDiffFinal=cvCloneImage(imgTmp);

		Limereg_Image imgDiffFinalPixels;
		imgDiffFinalPixels.pixelBuffer = (t_pixel *)imgDiffFinal->imageData;
		imgDiffFinalPixels.imageWidth = (uint32_t)iDim;
		imgDiffFinalPixels.imageHeight = (uint32_t)iDim;
		refPixels.pixelType = Limereg_Image::Grayscale_8;
		refPixels.pyramidImage = Limereg_Image::NotPyramidized;

		//todo: examine retval
		Limereg_CalculateDiffImage(&refPixels, &tmpPixels, &imgDiffFinalPixels);

		// show images
		//Intelligent image display size (size of image but not more than a maximum)
		uint32_t iDisplayImgSize = (iDim>APP_MAX_IMG_SIZE) ? APP_MAX_IMG_SIZE : iDim;
		ShowImage(imgTmp, "Template Image", 0, 0, iDisplayImgSize);
		ShowImage(imgRef, "Reference Image", 1, 0, iDisplayImgSize);
		ShowImage(imgTmpTrns, "Registered Image", 2, 0, iDisplayImgSize);
		ShowImage(imgDiffOrig, "Difference BEFORE", 0, 1, iDisplayImgSize);
		ShowImage(imgDiffFinal, "Difference AFTER", 1, 1, iDisplayImgSize);

		// Give windows some time to paint the content
		cvWaitKey(1);

		// wait for a key
		cvWaitKey(0);

		// release the images
		cvReleaseImage(&imgDiffFinal);
		cvReleaseImage(&imgDiffOrig);
	}

	if(bNeedTransImage)
	{
		cvReleaseImage(&imgTmpTrns);
	}
	cvReleaseImage(&imgRef);
	cvReleaseImage(&imgTmp);
}

/**
* Check wether image file is valid. (Will also check image size)
*/
bool CRegistrationController::CheckImage(IplImage* Image, const string& sFilename, uint32_t Dim)
{
	//Show all errors at once to the user (e.g. wrong color, height and width)
	bool bRetVal = CheckImage(Image, sFilename);
	if(NULL == Image)
	{
		//Function call above allready reported an error
		return false;
	}

	//Above checks wether image is square
	if(Image->height != Dim)
	{
		printf("%s Width and height must be %i but are %i.\n", gcsCannotLoadImage.c_str(), Dim, Image->height);
		bRetVal=false;
	}

	return bRetVal;
}

/**
* Check wether image file is valid. (Will not check image size)
*/
bool CRegistrationController::CheckImage(IplImage* Image, const string& sFilename)
{
	if(NULL == Image)
	{
		printf("%s '%s'", gcsCannotLoadImage.c_str(), sFilename.c_str());
		return false;
	}

	//Show all errors at once to the user (e.g. wrong color, height and width)
	bool bRetVal=true;

	const int iAllowedChannelCount=1;
	if(Image->nChannels != iAllowedChannelCount)
	{
		printf("%s Color channel count must be %i but is %i.\n", gcsCannotLoadImage.c_str(), iAllowedChannelCount, Image->nChannels);
		bRetVal=false;
	}

	if(Image->height != Image->width)
	{
		printf("%s Image dimensions must be square (width=height) but size is %i x %i.\n", gcsCannotLoadImage.c_str(), Image->width, Image->height);
		bRetVal=false;
	}

	/*
	const int iAllowedWidthStep=iDim;
	if(Image->widthStep != iDim)
	{
	printf("%s Internal image property WidthStep must be %i but is %i.\n", gcsCannotLoadImage.c_str(), iDim, Image->widthStep);
	bRetVal=false;
	}
	*/

	if((Image->height%2)!=0)
	{
		printf("%s Image dimensions must be even but width and height are odd (%i).\n", gcsCannotLoadImage.c_str(), Image->height);
		bRetVal=false;
	}

	if(Image->height<=0)
	{
		printf("%s Image dimensions must be bigger than 0 (but are %i).\n", gcsCannotLoadImage.c_str(), Image->height);
		bRetVal=false;
	}

	return bRetVal;
}

/**
* Display image data of 'image' in window named 'WindowName' at the zero based image position xPos, yPos.
* The position is calculated based on the image dimensions WndSize (e.g. 1,2 an image in the second row and third column)
*/
void CRegistrationController::ShowImage(IplImage* Image, const string& WindowName, uint32_t xPos, uint32_t yPos, uint32_t WndSize)
{
	cvNamedWindow(WindowName.c_str(), 0); 
	MoveWindow(WindowName, xPos, yPos, WndSize);
	cvShowImage(WindowName.c_str(), Image );
}

/**
* Move a window to the zero based image position xPos, yPos.
* The position is calculated based on the image dimensions WndSize (e.g. 1,2 an image in the second row and third column)
*/
void CRegistrationController::MoveWindow(const string& WindowName,  uint32_t xPos, uint32_t yPos, uint32_t WndSize)
{
	const uint32_t iXSpacer=20;
	const uint32_t iYSpacer=40;

	uint32_t xPosPixel = xPos*(iXSpacer+WndSize);
	uint32_t yPosPixel = yPos*(iYSpacer+WndSize);
	cvMoveWindow(WindowName.c_str(), xPosPixel, yPosPixel);
	cvResizeWindow(WindowName.c_str(), WndSize, WndSize);
}

/**
 * Read and validate the shell command line parameters the user entered.
 * Print out an error report and usage-information if the parameter validation failed.
 * Print out only usage-information if the user requested this information.
 * If parameter parsing was successful: Print out parsed parameters for verification.
 *
 * \param argc amount of cmdline args
 * \param argv pointer to array of argument strings
 * \retval success
 */
bool CRegistrationController::ParseParameters(int argc, char ** argv)
{
	// DEFAULT CMDLINE PARAMETERS
	const uint32_t DEF_CMD_PARAM_MAXITER = 150;
	const uint32_t DEF_CMD_PARAM_LEVELCOUNT = 0;
	const t_reg_real DEF_CMD_PARAM_MAXROTATION = 45.0f;
	const t_reg_real DEF_CMD_PARAM_MAXTRANSLATION = 30.0f;
	const t_reg_real DEF_CMD_PARAM_STOPSENS = 0.7f;
	const uint8_t DEF_CMD_PARAM_CDN=10;

	//CMDLine parameter tokens
	const char csHelp[] = "help";
	const char csVersion[] = "version";
	const char csTFilename[] = "tfile";
	const char csRFilename[] = "rfile";
	const char csOutFilename[] = "outfile";
	const char csMaxIter[] = "maxiter";
	const char csLevelCount[] = "levels";
	const char csMaxRotation[] = "maxrot";
	const char csMaxTranslation[] = "maxtrans";
	const char csStopSens[] = "stopsens";
	const char csClipDarkNoise[] = "cdn";
	const char csNoGui[] = "nogui";

	string version( "\n" "Copyright 2014, Roelof Berg, Licensed under the 3-clause BSD license at "
					"http://berg-solutions.de/limereg-license.html" ". Credit goes to Lars Koenig "
					"and Jan Ruehaak from Fraunhofer MEVIS in Germany."
					);

	//Define expected cmdline parameters to boost
	options_description desc("limereg - Lightweight Image Registration\n"
		                     "Performs a simple rigid image registration. "
		                     "The image files must be square and grayscale. Supported image formats: *.bmp, *.dib, *.jpeg, "
							 "*.jpg, *.jpe, *.jp2, *.png, *.pbm, *.pgm, *.ppm, *.sr, *.ras, *.tiff, *.tif.\n\n"
							 "Usage: limereg --rfile <reference image> --tfile <template image> [OPTIONS]"
							 "\n\nOptions"
							 );

	options_description exmpl("Examples:\n"
							 "limereg --tfile t.jpg --rfile r.png\tShifts/Rotates t.jpg for alignment with r.png and displays the result.\n"
							 "limereg --tfile t.bmp --rfile r.tif --nogui --outfile o.jpg\tOutputs the result to o.jpg, no GUI display.\n"
							 );

	desc.add_options()
		(csHelp, "Show usage information.")

		(csVersion, "Show application version and copyright information.")

		(csRFilename, value<string>(), "Template image file (will be transformed). The image must be grayscale colored and it must have even and square pixel dimensions.")

		(csTFilename, value<string>(), "Reference image file (will be matched against). The image must be grayscale colored and it must have even and square pixel dimensions.")

		(csOutFilename, value<string>(), "Output image file (optional). Gives the option to save the registered template image.")

		(csMaxIter, value<uint32_t>(), (boost::format("Max. amount of iterations.\n"
									"[Optional parameter, default: %1%]") % DEF_CMD_PARAM_MAXITER).str().c_str())

		(csLevelCount, value<uint32_t>(), (boost::format("Amount of levels in the multilevel pyramid of shrinked image copies. 0 means autodetect by the formula log2(ImageWidth/%1%).\n"
									"[Optional parameter, default: %2%]") % gui_LEVELCOUNT_AUTOTETECT_DIVISOR % DEF_CMD_PARAM_LEVELCOUNT).str().c_str())

		(csMaxRotation, value<t_reg_real>(), (boost::format("Max. expected rotation in degrees. Used for internal optimizations in DSP calculation mode. "
									"If a rotation value will become necessary that is higher than specified here, the algorithm will still succeed "
									"but the calculation speed will slow down).\n"
									"[Optional parameter, default: %1%]") % DEF_CMD_PARAM_MAXROTATION).str().c_str())

		(csMaxTranslation, value<t_reg_real>(), (boost::format("Max. expected translation in percent. Used for internal optimizations in DSP calculation mode. "
									"If a translation value will become necessary that is higher than specified here, the algorithm will still succeed "
									"but the calculation speed will slow down).\n"
									"[Optional parameter, default: %1%]") % DEF_CMD_PARAM_MAXTRANSLATION).str().c_str())

		(csStopSens, value<t_reg_real>(), (boost::format("Sensitivity of the STOP criteria for the gauss-newton algorithm.\n"
									"[Optional parameter, use ranges between 1 (not sensitive, stops early) to 0.0001 (very sensitive) default: %1%]") % DEF_CMD_PARAM_STOPSENS).str().c_str())

		(csClipDarkNoise, value<uint32_t>(), (boost::format("Clip dark pixels until (including) the given luminance to zero. This reduces image sensor noise in the usually uninteresting dark pixels. A high value optimizes data compression and calculation performance at the cost of calculation accuracy.\n"
									"[Optional parameter, range 0...255, 0 disables any clipping, default: %1%]") % (uint32_t)DEF_CMD_PARAM_CDN).str().c_str())

        (csNoGui, "If active no GUI output will be shown. Use this for batch registration of several images.\n"
									"[Flag parameter]")
	;

	desc.add(exmpl);

	try
	{
		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);

		//CMDLine parameter --help .......................................................................................
		if (0 < vm.count(csHelp))
		{
			CLogger::PrintUsage(desc);
			return false;
		}

		//CMDLine parameter --version .......................................................................................
		if (0 < vm.count(csVersion))
		{
			CLogger::PrintInfo(version);
			return false;
		}

		//CMDLine parameter --t .......................................................................................
		if (0 < vm.count(csTFilename))
		{
			m_sTFilename = vm[csTFilename].as<string>();
			boost::algorithm::trim(m_sTFilename);
		}
		else //== 0
		{
			m_sTFilename = "";	//Just to be stateless (unnecessary when method called only once)
		}

		if(0 == m_sTFilename.size())
		{
			//Argument missing: Error and exit (mandatory)
			CLogger::PrintError((boost::format("Missing argument '--%1%' (template image).") % csTFilename).str());
			CLogger::PrintUsage(desc);
			return false;
		}

		//CMDLine parameter --r .......................................................................................
		if (0 < vm.count(csRFilename))
		{
			m_sRFilename = vm[csRFilename].as<string>();
			boost::algorithm::trim(m_sRFilename);
		}
		else //== 0
		{
			m_sRFilename = "";	//Just to be stateless (unnecessary when method called only once)
		}

		if(0 == m_sRFilename.size())
		{
			//Argument missing: Error and exit (mandatory)
			CLogger::PrintError((boost::format("Missing argument '--%1%' (reference image).") % csRFilename).str());
			CLogger::PrintUsage(desc);
			return false;
		}

		//CMDLine parameter --out .......................................................................................
		if (0 < vm.count(csOutFilename))
		{
			m_sSaveTransImage = vm[csOutFilename].as<string>();
			boost::algorithm::trim(m_sSaveTransImage);
		}
		else //== 0
		{
			m_sSaveTransImage = "";	//Just to be stateless (unnecessary when method called only once)
		}

		if(0 == m_sRFilename.size())
		{
			//Argument missing: Error and exit (mandatory)
			CLogger::PrintError((boost::format("Missing argument '--%1%' (reference image).") % csRFilename).str());
			CLogger::PrintUsage(desc);
			return false;
		}

		//CMDLine parameter --maxiter .......................................................................................
		if (0 < vm.count(csMaxIter))
		{
			m_uiMaxIter = vm[csMaxIter].as<uint32_t>();
			if(m_uiMaxIter==0)
			{
				CLogger::PrintError((boost::format("Invalid argument '--%1%'. Max. amount of iterations must be > 0 iterations.") % csMaxIter).str());
				CLogger::PrintUsage(desc);
				return false;
			}
		}
		else //== 0
		{
			//Argument missing: Use hardcoded default value
			m_uiMaxIter = DEF_CMD_PARAM_MAXITER;
		}


		//CMDLine parameter --levels .......................................................................................
		if (0 < vm.count(csLevelCount))
		{
			m_iLevelCount = vm[csLevelCount].as<uint32_t>();
			if(m_iLevelCount==0)
			{
				CLogger::PrintError((boost::format("Invalid argument '--%1%'. Max. amount of pyramid levels must be > 0.") % csLevelCount).str());
				CLogger::PrintUsage(desc);
				return false;
			}
		}
		else //== 0
		{
			//Argument missing: Use hardcoded default value
			m_iLevelCount = DEF_CMD_PARAM_LEVELCOUNT;
		}


		//CMDLine parameter --maxrot .......................................................................................
		if (0 < vm.count(csMaxRotation))
		{
			m_fMaxRotation = vm[csMaxRotation].as<t_reg_real>();
			t_reg_real fMaxRotDeg=180.0f;
			if(m_fMaxRotation<0 || m_fMaxRotation>fMaxRotDeg)
			{
				CLogger::PrintError((boost::format("Invalid argument '--%1%'. Max. rotaion must be in between the range [0 deg ... %2% deg].") % csMaxRotation % fMaxRotDeg).str());
				CLogger::PrintUsage(desc);
				return false;
			}
		}
		else //== 0
		{
			//Argument missing: Use hardcoded default value
			m_fMaxRotation = DEF_CMD_PARAM_MAXROTATION;
		}

		//CMDLine parameter --maxtrans .......................................................................................
		if (0 < vm.count(csMaxTranslation))
		{
			m_fMaxTranslation = vm[csMaxTranslation].as<t_reg_real>();
			if(m_fMaxTranslation<0)
			{
				CLogger::PrintError((boost::format("Invalid argument '--%1%'. Max. translation must be >= 0 percent.") % csMaxTranslation).str());
				CLogger::PrintUsage(desc);
				return false;
			}
		}
		else //== 0
		{
			//Argument missing: Use hardcoded default value
			m_fMaxTranslation = DEF_CMD_PARAM_MAXTRANSLATION;
		}

		//CMDLine parameter --stopsens .......................................................................................
		if (0 < vm.count(csStopSens))
		{
			m_fStopSens = vm[csStopSens].as<t_reg_real>();
		}
		else //== 0
		{
			//Argument missing: Use hardcoded default value
			m_fStopSens = DEF_CMD_PARAM_STOPSENS;
		}
		
		//CMDLine parameter --CDN .......................................................................................
		if (0 < vm.count(csClipDarkNoise))
		{
			//Boost handles uint8_t as char. Hence we use uint32_t and check for uint8_t value range ourselves.
			uint32_t uClipDarkNoise_u32 = vm[csClipDarkNoise].as<uint32_t>();

			if(255<uClipDarkNoise_u32)
			{
				CLogger::PrintError((boost::format("Invalid argument '--%1%'. Max. clipping luminance can be 255.") % uClipDarkNoise_u32).str());
				CLogger::PrintUsage(desc);
				return false;
			}

			guClipDarkNoise = (uint8_t)uClipDarkNoise_u32;
		}
		else //== 0
		{
			//Argument missing: Use hardcoded default value
			guClipDarkNoise = DEF_CMD_PARAM_CDN;
		}

		//CMDLine parameter --nogui ..........................................................................
		m_bNoGui = (0 < vm.count(csNoGui));

	}
	catch(exception& e)
	{
		CLogger::PrintError((boost::format("Invalid arguments. ('%1%')") % e.what()).str());
		CLogger::PrintUsage(desc);
		return false;
	}

	//..........................................................................................................................
	//Reply parsed parameters to user console
	const char szEnabled[] = "enabled";
	const char szDisabled[] = "disabled";
	CLogger::PrintInfo((boost::format("Parameters: %1%=\"%2%\" %3%=\"%4%\" %5%=\"%6%\" %7%=%8%[iter.] %9%=%10%[levels, 0=autom.] %11%=%12%[deg.] %13%=%14%[percent] %15%=%16% %17%=%18% %19%=%20%")
			% csTFilename % m_sTFilename
			% csRFilename % m_sRFilename
			% csOutFilename % m_sSaveTransImage
			% csMaxIter % m_uiMaxIter
			% csLevelCount % m_iLevelCount
			% csMaxRotation % m_fMaxRotation
			% csMaxTranslation % m_fMaxTranslation
			% csStopSens % m_fStopSens
			% csClipDarkNoise % guClipDarkNoise
			% csNoGui % (m_bNoGui ? szEnabled : szDisabled)
			).str());
	return true;
}


