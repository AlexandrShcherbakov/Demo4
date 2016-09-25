#ifndef CL_H_INCLUDED
#define CL_H_INCLUDED

#include "CL/clew.h"
#include "CL/cl_gl.h"

#include <stdio.h>
#include <stdlib.h>

namespace CL {

static int cl_err = 0;
#define CHECK_CL(call) checkCLFun((call), (char*)__FILE__, __LINE__);
const char * getOpenCLErrorString(cl_int err);
void checkCLFun(cl_int cErr, char* file, int line);

}

#endif // CL_H_INCLUDED
