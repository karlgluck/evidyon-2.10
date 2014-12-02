//------------------------------------------------------------------------------------------------
// File:  cleanup.cpp
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include <d3dx9.h>
#include "cleanup.h"



//------------------------------------------------------------------------------------------------
// Name:  Cleanup
// Desc:  
//------------------------------------------------------------------------------------------------
Cleanup::Cleanup()
{
}



//------------------------------------------------------------------------------------------------
// Name:  ~Cleanup
// Desc:  
//------------------------------------------------------------------------------------------------
Cleanup::~Cleanup()
{
  for (Entries::iterator i = myEntries.begin(); i != myEntries.end(); ++i)
  {
    switch(i->first)
    {
      case ARRAY:
      {
        void* pointer = *((void**)(i->second));
        if (pointer) delete[] pointer;
        *((void**)(i->second)) = NULL;
      } break;

      case INTERFACE:
      {
        IUnknown* iface = *((IUnknown**)(i->second));
        if (iface) iface->Release();
        (*(IUnknown**)i->second) = NULL;
      } break; 

      case OBJECT:
      {
        void* pointer = *((void**)(i->second));
        if (pointer) delete pointer;
        *((void**)(i->second)) = NULL;
      } break;

      case WINDOW:
      {
        HWND hWnd = *((HWND*)(i->second));
        if (hWnd) DestroyWindow(hWnd);
        *((HWND*)(i->second)) = NULL;
      } break;

      case STDFILE:
      {
        FILE* pFile = *((FILE**)(i->second));
        if (pFile) fclose(pFile);
        *((FILE**)(i->second)) = NULL;
      } break;
    }
  }
}



//------------------------------------------------------------------------------------------------
// Name:  addArray
// Desc:  
//------------------------------------------------------------------------------------------------
void Cleanup::addArray(void** pointer)
{
  myEntries.push_front(Entries::value_type(ARRAY, pointer));
}



//------------------------------------------------------------------------------------------------
// Name:  addInterface
// Desc:  
//------------------------------------------------------------------------------------------------
void Cleanup::addInterface(IUnknown** iface)
{
  myEntries.push_front(Entries::value_type(INTERFACE, (void**)iface));
}



//------------------------------------------------------------------------------------------------
// Name:  addObject
// Desc:  
//------------------------------------------------------------------------------------------------
void Cleanup::addObject(void** pointer)
{
  myEntries.push_front(Entries::value_type(OBJECT, pointer));
}


//------------------------------------------------------------------------------------------------
// Name:  addWindow
// Desc:  Adds the given window variable to the cleanup list
//------------------------------------------------------------------------------------------------
void Cleanup::addWindow(HWND* pointer)
{
  myEntries.push_front(Entries::value_type(WINDOW, (void**)pointer));
}


//------------------------------------------------------------------------------------------------
// Name:  addStdFile
// Desc:  Adds the given file variable to the cleanup list
//------------------------------------------------------------------------------------------------
void Cleanup::addStdFile(FILE** pointer)
{
  myEntries.push_front(Entries::value_type(STDFILE, (void**)pointer));
}



//------------------------------------------------------------------------------------------------
// Name:  clear
// Desc:  
//------------------------------------------------------------------------------------------------
void Cleanup::clear()
{
  myEntries.clear();
}




