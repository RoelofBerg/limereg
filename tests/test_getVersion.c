#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include "test_common.h"
#include <limereg.h>

bool test_getVersion()
{
	printf("Test: Limereg_GetVersion()\n");
	return (0 == strcmp(VERSION, Limereg_GetVersion()));
}

int main(void)
{
	//Execute test
	bool result = outputTestResult(test_getVersion());
	return (result ? RET_SUCCESS : RET_FAILED);
}
