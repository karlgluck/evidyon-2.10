//------------------------------------------------------------------------------------------------
// File:    bufferstream.cpp
//
// Desc:    Implements a stream that writes to a data buffer in memory
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include <string>
#include "../dc/dcstream.h"
#include "../dc/dcbufferstream.h"
#include <memory.h>



using namespace dc;



//------------------------------------------------------------------------------------------------
// Name: dcBufferStream
// Desc: Resets the class's internal variables
//------------------------------------------------------------------------------------------------
dcBufferStream::dcBufferStream()
{
    myBuffer = 0;
    myBufferSize = 0;
    myAccessPtr = 0;
}


//------------------------------------------------------------------------------------------------
// Name: dcBufferStream
// Desc: Cleans up the class
//------------------------------------------------------------------------------------------------
dcBufferStream::~dcBufferStream()
{
    if( myBuffer )
        delete[] myBuffer;
}



//------------------------------------------------------------------------------------------------
// Name: seek
// Desc: Moves to a place within the stream
//------------------------------------------------------------------------------------------------
bool dcBufferStream::seek( size_t location )
{
    if( location < 0 || location > myBufferSize ) return false;
    myAccessPtr = location;
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: move
// Desc: Moves the buffer's read/write cursor
//------------------------------------------------------------------------------------------------
bool dcBufferStream::move( int Amount )
{
    return seek( (int)myAccessPtr + Amount );
}



//------------------------------------------------------------------------------------------------
// Name: read
// Desc: Gets data from the buffer
//------------------------------------------------------------------------------------------------
bool dcBufferStream::read( void* buffer, size_t size )
{
    // Don't read past the end of the buffer
    if( (myBuffer == 0) || (myAccessPtr + size ) > myBufferSize ) return false;

    // Scan the information
    memcpy_s( buffer, size, myBuffer + myAccessPtr, size );

    // Advance the pointer
    myAccessPtr += size;

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: end
// Desc: Determines EOF condition
//------------------------------------------------------------------------------------------------
bool dcBufferStream::end()
{
    return myAccessPtr == myBufferSize;
}



//------------------------------------------------------------------------------------------------
// Name: write
// Desc: Puts data into the buffer
//------------------------------------------------------------------------------------------------
bool dcBufferStream::write( const void* buffer, size_t size )
{
    // If we would write past the end of the buffer, reallocate it
    if( (myAccessPtr + size ) > myBufferSize )
    {
        // Calculate a new buffer size that is double the current size, padded a little bit
        size_t newBufferSize = (size_t)((myBufferSize + myAccessPtr + size) * 2);

        // Create a new buffer 
        unsigned char* pNewBuffer = new unsigned char[newBufferSize];
        if( !pNewBuffer ) return false;

        // If there was anything in our current buffer, save it to the new one
        if( myBuffer != 0 )
        {
            memcpy( pNewBuffer, myBuffer, myBufferSize );
            delete[] myBuffer;
        }

        // Set the buffer pointer
        myBuffer = pNewBuffer;

        // Save the new size
        myBufferSize = newBufferSize;
    }

    // Scan the information into the buffer
    memcpy_s( myBuffer + myAccessPtr, myBufferSize - myAccessPtr, buffer, size );

    // Advance the pointer
    myAccessPtr += size;

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: tell
// Desc: Gets the current position of the stream, such that
//------------------------------------------------------------------------------------------------
size_t dcBufferStream::tell()
{
    return myAccessPtr;
}
