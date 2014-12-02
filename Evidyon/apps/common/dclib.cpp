#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comctl32.lib")
#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(lib, "dc_d.lib")
#pragma comment(lib, "dcx_d.lib")
#else
#pragma comment(lib, "dc.lib")
#pragma comment(lib, "dcx.lib")
#endif