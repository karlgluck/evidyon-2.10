//------------------------------------------------------------------------------------------------
// File:    dcstream.cpp
//
// Desc:    Implements generic stream methods
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include <string>
#include "../dc/dcstream.h"
#include "../dc/debug"



using namespace dc;

/// The magic number that identifies a string
#define DC_STRING_MAGIC_NUMBER  0xFF061888


//------------------------------------------------------------------------------------------------
// Name:  ~dcStream
// Desc:  Empty destructor
//------------------------------------------------------------------------------------------------
dcStream::~dcStream()
{
}



//------------------------------------------------------------------------------------------------
// Name:  readString
// Desc:  Reads a string from the stream
//------------------------------------------------------------------------------------------------
bool dcStreamIn::readString( std::string* string, size_t lengthLimit )
{
    // Read the magic number that identifies this item as a string
    size_t magicNumber;
    if( !read( &magicNumber, sizeof(magicNumber) ) ) return false;

    // For now, only warn about the magic number being wrong
    // TODO: fail if the magic number is wrong
    APP_WARNING( magicNumber != DC_STRING_MAGIC_NUMBER )
        ( "The stream being loaded uses legacy strings; re-save or risk losing contents in a future update" );
    if( magicNumber != DC_STRING_MAGIC_NUMBER ) return false;

    // Get the number of characters in the string
    size_t length = magicNumber;
    if( magicNumber == DC_STRING_MAGIC_NUMBER )
        if( !read( &length, sizeof(length) ) ) return false;

    // Check the string length
    if( lengthLimit != (size_t)(-1) )
    {
        if( APP_WARNING( length > lengthLimit )
            ( "readString encountered a string that was too long (%lu > %lu)", length, lengthLimit ) )
            return false;
    }

    // Allocate a temporary buffer for the string
    char* stringBuffer = new char[length+1];
    if( APP_FATAL( !stringBuffer )( "Out of memory" ) ) return false;

    // Read the string
    if( length > 0 && !read( stringBuffer, length ) ) { delete[] stringBuffer; return false; }

    // Add the string terminator
    stringBuffer[length] = '\0';

    // Assign the name
    string->resize( length + 1 );
    string->assign( stringBuffer );

    // Erase the buffer
    delete[] stringBuffer;

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  writeString
// Desc:  Writes a string into the stream to be read by dcStreamIn::readString
//------------------------------------------------------------------------------------------------
bool dcStreamOut::writeString( const std::string& string )
{
    // Save the string magic number first
    size_t magicNumber = DC_STRING_MAGIC_NUMBER;
    if( !write( &magicNumber, sizeof(magicNumber) ) ) return false;

    // Write the length of the string
    size_t length = string.length();
    if( !write( &length, sizeof(length) ) ) return false;

    // Write the text itself
    if( length > 0 && !write( string.c_str(), length ) ) return false;

    // Success
    return true;
}