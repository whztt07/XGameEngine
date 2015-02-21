#ifndef _XGlobal_H_
#define _XGlobal_H_

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")

#include "d3d9.h"
#include "d3dx9.h"
#include "dinput.h"
#include <tchar.h>

#ifndef ReleaseCOM
#define ReleaseCOM(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#endif
