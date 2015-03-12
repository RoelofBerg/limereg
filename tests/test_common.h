#ifndef _TEST_COMMON_H
#define _TEST_COMMON_H

//Output of GNU autotools
#include "../config.h"

typedef enum { false, true } bool;

#define RET_SUCCESS = 0;
#define RET_FAILED = 1;

bool outputTestResult(bool result)
{
	if(result)
	{
		printf("PASS :)\n\n");
	}
	else
	{
		printf("FAIL :(\n\n");
	}

	return result;
}



#endif //_TEST_COMMON_H
