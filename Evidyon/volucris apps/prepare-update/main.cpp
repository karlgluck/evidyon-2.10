//------------------------------------------------------------------------------------------------
// File:    main.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include <windows.h>
#include <string>
#include <direct.h>
#include <dc/filestream>

#if defined( _DEBUG )
#pragma comment( lib, "dc_d.lib" )
#else
#pragma comment( lib, "dc.lib" )
#endif


const char* EVIDYON_FILES_SRC[] = { "client.exe",
                                    "client_sse.exe",
                                    "client_sse2.exe",
                                    "evidyon.gme" };

const char* EVIDYON_FILES_DEST[] = { "evidyon.exe",
                                     "evidyon_sse.exe",
                                     "evidyon_sse2.exe",
                                     "evidyon.gme" };

const char* EVIDYON_DOWNLOAD_UPDATE_URL = "http://www.unseenstudios.com/evidyon/download/update/";


//------------------------------------------------------------------------------------------------
// Name: main
// Desc: Takes argument 2 (the server's EXE file name) and runs the program.  When the program
//       bombs, the time is logged and the server is restarted.
//------------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        printf( "Usage:  prepare-update \"folder\"" );

        return -1;
    }

    std::string sourceFolder = argv[0];
    sourceFolder = sourceFolder.substr( 0, sourceFolder.find_last_of('\\') );

    // The folder from which to read the update.  The name of the folder should be the update's version.
    std::string folder = argv[1];
    std::string version = folder.substr( folder.find_last_of('\\')+1 );

    // Copy from the current folder into the target folder
    for( int i = 0; i < sizeof(EVIDYON_FILES_SRC) / sizeof(const char*); ++i )
        CopyFile( ((sourceFolder + "\\") + EVIDYON_FILES_SRC[i]).c_str(), ((folder + "\\") + EVIDYON_FILES_DEST[i]).c_str(), FALSE );

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile( (folder + "\\*").c_str(), &findFileData );

    // Build the structure of the update
    std::string str = version + "\n-\n";

    // Add the strings
    str += EVIDYON_DOWNLOAD_UPDATE_URL;
    str += "\n-";

    // Find all files in this folder
    while( hFind != INVALID_HANDLE_VALUE )
    {
        __int64 size = ((__int64)(findFileData.nFileSizeHigh)<<32)|((__int64)(findFileData.nFileSizeLow));

        if( size )
        {
            char fileSize[32];
            sprintf_s( fileSize, "(%I64d)", size );

            str += "\n";
            str += findFileData.cFileName;
            str += fileSize;
        }

        if( !FindNextFile( hFind, &findFileData ) )
        {
            FindClose( hFind );
            hFind = INVALID_HANDLE_VALUE;
        }
    }

    str += "\n-\n\n-\n";

    dc::dcFileStream fs;
    if( fs.open( ((folder + "\\update-") + (version + ".txt")).c_str(), STREAM_TEXT|STREAM_OVERWRITE ) )
        fs.write( str.c_str(), str.length() );
    fs.close();

    return 0;
}