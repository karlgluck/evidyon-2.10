//------------------------------------------------------------------------------------------------
// File:    main.cpp
//
// Desc:    Entry point to the server monitoring application
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include <windows.h>
#include <string>
#include <direct.h>


/// This flag is set to 'false' when the console's window closes
bool globalRunningFlag = true;
HANDLE globalCloseEvent = 0;


//------------------------------------------------------------------------------------------------
// Name: ConsoleHandler
// Desc: Handles Windows messages to the console window
//------------------------------------------------------------------------------------------------
BOOL WINAPI ConsoleHandler( DWORD CtrlType )
{
    if( CtrlType == CTRL_SHUTDOWN_EVENT ||
        CtrlType == CTRL_CLOSE_EVENT )
    {
        globalRunningFlag = false;
        SetEvent( globalCloseEvent );
    }

    return TRUE;
}

//------------------------------------------------------------------------------------------------
// Name: main
// Desc: Takes argument 2 (the server's EXE file name) and runs the program.  When the program
//       bombs, the time is logged and the server is restarted.
//------------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    if( argc < 2 ) return -1;

    std::string serverEXEFile = argv[1];//std::string("\"") + argv[1] + "\"";

    int timesRestarted = 1;

    std::string directory = serverEXEFile.substr( 0, serverEXEFile.find_last_of( "\\" ) );

    globalCloseEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    while( globalRunningFlag )
    {
        STARTUPINFO startupInfo;
        ZeroMemory( &startupInfo, sizeof(startupInfo) );

        PROCESS_INFORMATION processInfo;

        // Make sure the directory is set
        SetCurrentDirectory( directory.c_str() );
        _chdir( directory.c_str() );

        // Start the server
        if( !CreateProcessA( serverEXEFile.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE|ABOVE_NORMAL_PRIORITY_CLASS,
                             NULL, NULL, &startupInfo, &processInfo ) )
        {
            DWORD error = GetLastError();
            MessageBox( NULL, "Couldn't restart server!", "Error", MB_OK );
            return -1;
        }

        // Wait for the server to close
        HANDLE waitEvents[] = { globalCloseEvent, processInfo.hProcess };
        WaitForMultipleObjects( 2, waitEvents, FALSE, INFINITE );

        CloseHandle( processInfo.hProcess );
        CloseHandle( processInfo.hThread );

        // Restart the server after 15 seconds
        if( globalRunningFlag )
        {
            printf( "Server restarting in 15 seconds (done %lu times)\n", ++timesRestarted );
            Sleep( 15000 );
        }
    }

    return 0;
}