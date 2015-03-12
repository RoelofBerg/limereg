#include <stdio.h>
#include <stdlib.h>

#include "test_common.h"

#if HAVE_BOOST==1 && HAVE_BOOST_PROGRAM_OPTIONS_HPP==1 && HAVE_OPENCV==1


int main(void)
{
	printf("Testing Limereg executable\n");
	system("../exe/limereg --tfile testimg/T_512.bmp --rfile testimg/R_512.bmp");

	outputTestResult();
}

#endif
