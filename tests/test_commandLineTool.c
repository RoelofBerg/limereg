#include <stdio.h>
#include <stdlib.h>

#include "test_common.h"

#if HAVE_BOOST==1 && HAVE_BOOST_PROGRAM_OPTIONS_HPP==1 && HAVE_OPENCV==1

int executeShellCommand(const char* command)
{
    printf("%s\n", command);
    return system(command);
}

int main(void)
{
	int retcode=0;

	//Execute limereg executable and analyze console output
	printf("Test: Command line tool 'limereg'\n");
	retcode = executeShellCommand(
	        "../exe/limereg --tfile " IMGDIR "T_512.bmp --rfile " IMGDIR "R_512.bmp --nogui "
            "| tee /dev/tty | grep -G 'w = \\[5.* deg, 0.5.*, 14.6.*\\]'"
	        );

	if(0 == retcode) //continue upon success
	{
	    retcode = executeShellCommand(
	            "../exe/limereg --tfile " IMGDIR "lime1.png --rfile " IMGDIR "lime2.png --nogui --invert"
	            "| tee /dev/tty | grep -G 'w = \\[-4.* deg, 36.*, -55.*\\]'"
	            );
	}

	//Output and return test result
	outputTestResult((0 == retcode) ? true : false);
	return (0 == retcode) ? RET_SUCCESS : RET_FAILED;
}

#endif
