#include <stdio.h>
#include <stdlib.h>

#include "test_common.h"

#if HAVE_BOOST==1 && HAVE_BOOST_PROGRAM_OPTIONS_HPP==1 && HAVE_OPENCV==1


int main(void)
{
	int retcode=0;

	printf("Testing limereg executable\n");
	retcode = system("../exe/limereg --tfile testimg/T_4096.bmp --rfile testimg/R_4096.bmp --nogui "
	                 "| tee /dev/tty | grep -G 'w = \[5.* deg, 6.*, 126.*\]'");

	printf("RETVAL: %d\n", retcode);
	outputTestResult();
}

#endif
