#include <stdio.h>
#include <stdlib.h>

#include "test_common.h"

#if HAVE_BOOST==1 && HAVE_BOOST_PROGRAM_OPTIONS_HPP==1 && HAVE_OPENCV==1


int main(void)
{
	int retcode=0;

	//Execute limereg executable and analyze console output
	printf("Test: Command line tool 'limereg'\n");
	char command[] = "../exe/limereg --tfile " IMGDIR "T_512.bmp --rfile " IMGDIR "R_512.bmp --nogui "
                   "| tee /dev/tty | grep -G 'w = \\[5.* deg, 0.5.*, 14.6.*\\]'";
	printf("%s\n", command);
	retcode = system(command);

	//Output and return test result
	outputTestResult((0 == retcode) ? true : false);
	return (0 == retcode) ? RET_SUCCESS : RET_FAILED;
}

#endif
