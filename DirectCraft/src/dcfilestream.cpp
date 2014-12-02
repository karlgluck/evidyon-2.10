//------------------------------------------------------------------------------------------------
// File:    dcfilestream.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include <string>
#include "../dc/dcstream.h"     // Basic stream interfaces
#include <stdio.h>              // For the standard file declaration and methods
#include "../dc/dcfilestream.h"


using namespace dc;


//------------------------------------------------------------------------------------------------
// Name:  dcFileStream
// Desc:  
//------------------------------------------------------------------------------------------------
dcFileStream::dcFileStream()
{
    myFile = 0;
}


//------------------------------------------------------------------------------------------------
// Name:  ~dcFileStream
// Desc:  
//------------------------------------------------------------------------------------------------
dcFileStream::~dcFileStream()
{
    close();
}


//------------------------------------------------------------------------------------------------
// Name:  open
// Desc:  
//------------------------------------------------------------------------------------------------
bool dcFileStream::open( const char * name, int how )
{
    // Close a currently open file
    close();

    // Which access specifier to use when opening the file
    const char * mask;

    // Build the access specifier
    switch( how )
    {
        case STREAM_OVERWRITE|STREAM_TEXT:  mask = "w+t"; break;
        case STREAM_OVERWRITE:              mask = "w+b"; break;
        case STREAM_TEXT:                   mask = "r+t"; break;
        default:                            mask = "r+b"; break;
    }

    // Open the file
    return 0 == fopen_s( &myFile, name, mask );
}


//------------------------------------------------------------------------------------------------
// Name:  close
// Desc:  
//------------------------------------------------------------------------------------------------
void dcFileStream::close()
{
    if( myFile )
    {
        fclose( myFile );
        myFile = 0;
    }
}


//------------------------------------------------------------------------------------------------
// Name:  seek
// Desc:  
//------------------------------------------------------------------------------------------------
bool dcFileStream::seek( size_t location )
{
    return 0 == fseek( myFile, location, SEEK_SET );
}


//------------------------------------------------------------------------------------------------
// Name:  move
// Desc:  
//------------------------------------------------------------------------------------------------
bool dcFileStream::move( int amount )
{
    if( myFile )
        return 0 == fseek( myFile, amount, SEEK_CUR );
    else
        return false;
}


//------------------------------------------------------------------------------------------------
// Name:  tell
// Desc:  
//------------------------------------------------------------------------------------------------
size_t dcFileStream::tell()
{
    if( myFile )
        return ftell( myFile );
    else
        return -1;
}


//------------------------------------------------------------------------------------------------
// Name:  read
// Desc:  
//------------------------------------------------------------------------------------------------
bool dcFileStream::read( void * buffer, size_t size )
{
    if( myFile )
        return 1 == fread( buffer, size, 1, myFile );
    else
        return false;
}


//------------------------------------------------------------------------------------------------
// Name:  end
// Desc:  
//------------------------------------------------------------------------------------------------
bool dcFileStream::end()
{
    if( myFile )
        return 0 != feof(myFile);
    else
        return true;
}


//------------------------------------------------------------------------------------------------
// Name:  write
// Desc:  
//------------------------------------------------------------------------------------------------
bool dcFileStream::write( const void * buffer, size_t size )
{
    if( myFile )
        return 1 == fwrite( buffer, size, 1, myFile );
    else
        return false;
}



//------------------------------------------------------------------------------------------------
// Name:  flush
// Desc:  Forces this file stream to write to the disk
//------------------------------------------------------------------------------------------------
void dcFileStream::flush()
{
    if( myFile ) fflush( myFile );
}



//------------------------------------------------------------------------------------------------
// Name:  size
// Desc:  
//------------------------------------------------------------------------------------------------
size_t dcFileStream::size() {
  if (myFile) {
    long current = ftell(myFile);
    fseek(myFile, 0, SEEK_END);
    size_t size = ftell(myFile);
    fseek(myFile, current, SEEK_SET);
    return size;
  } else {
    return 0;
  }
}