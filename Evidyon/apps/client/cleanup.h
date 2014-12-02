//------------------------------------------------------------------------------------------------
// File:  cleanup.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __CLEANUP_H__
#define __CLEANUP_H__


#include <list>



class Cleanup
{
  enum Type
  {
    INTERFACE,
    OBJECT,
    ARRAY,
    WINDOW,
    STDFILE,
  };

  /// The entries that are in this cleanup class
  typedef std::list<std::pair<Type,void**>> Entries;


  public:

    /**
     * Initializes the class
     */
    Cleanup();

    /**
     * Erases all of the allocated memory in the class
     */
    ~Cleanup();

    /**
     * Adds a Direct3D interface to the cleanup list.  This will both
     * call (*iface)->Release and set (*iface) = NULL.
     */
    void addInterface(IUnknown** iface);

    /**
     * Adds the given pointer to the cleanup list.  This will
     * call delete (*pointer) and (*pointer) = NULL;
     */
    void addObject(void** pointer);

    /**
     * Adds the given pointer to the cleanup list.  This will
     * call delete [] (*pointer) and (*pointer) = NULL;
     */
    void addArray(void** pointer);

    /**
     * Adds the given window variable to the cleanup list.  This will
     * call DestroyWindow(*pointer) and *pointer = NULL;
     */
    void addWindow(HWND* pointer);

    /**
     * Adds the given file variable to the cleanup list.  This will
     * call fclose(*pointer) and *pointer = NULL;
     */
    void addStdFile(FILE** pointer);

    /**
     * Erases all of the entries in the list so that, when this
     * class is destructed, it won't erase all of the information
     * that it contains.
     */
    void clear();


  protected:

    /// The entries in the cleanup set
    Entries myEntries;
};





#endif