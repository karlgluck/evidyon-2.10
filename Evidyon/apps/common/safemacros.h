#ifndef __SAFEMACROS_H__
#define __SAFEMACROS_H__


#define SAFE_DELETE(p)  if (p) { delete p; p = NULL; } 
#define SAFE_DELETE_ARRAY(p)  if (p) { delete[] p; p = NULL; }
#define SAFE_RELEASE(p)   if (p) { p->Release(); p = NULL; }
#define SAFE_RELEASE_ELEMENTS(a, n)   {for (size_t i=0;i<n;++i){SAFE_RELEASE(a[i])}}

#pragma warning(error : 4700)


#endif