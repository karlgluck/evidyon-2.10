//------------------------------------------------------------------------------------------------
// File:    main.cpp
//
// Desc:    Entry point to the server monitoring application
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include <windows.h>
#include <dcx/cpu>
#include <wininet.h>
#include <dc/debug>
#include "commctrl.h"
#include <dcx/win32>
#include <windowsx.h>
#include "../../../Volucris/common/version.h"
#include <vector>
#include <direct.h>

#include "resource.h"



#define EVIDYON_UPDATE_URL        "http://www.unseenstudios.com/evidyon/download/update/update.txt"






#define Progress_Enable(hwndCtl, flags)         EnableWindow((hwndCtl), (fEnable))
#define Progress_Show(hwndCtl, fShow)           ShowWindow((hwndCtl), (fShow) ? SW_SHOWNORMAL : SW_HIDE)
#define Progress_SetRange(hwndCtl, min, max)    ((LPARAM)(DWORD)SNDMSG((hwndCtl), PBM_SETRANGE, 0L, (LPARAM)MAKELPARAM( (min), (max) )))
#define Progress_SetRange32(hwndCtl, min, max)    ((LPARAM)(DWORD)SNDMSG((hwndCtl), PBM_SETRANGE, (WPARAM)(int)(min), (LPARAM)(int)(max)))
#define Progress_SetPos(hwndCtl, pos)           ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETPOS, (WPARAM)(int)(pos), 0L ))
#define Progress_DeltaPos(hwndCtl, delta)       ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_DELTAPOS, (WPARAM)(int)(delta), 0L ))
#define Progress_SetStep(hwndCtl, step)         ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETSTEP, (WPARAM)(int)(step), 0L ))
#define Progress_StepIt(hwndCtl)                ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_STEPIT, 0L, 0L ))
#define Progress_SetMarquee(hwndCtl, fMarquee, msPerUpdate)  ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETMARQUEE, (WPARAM)(BOOL)(fMarquee), (LPARAM)(UINT)(msPerUpdate) ))
#define Progress_SetBkColor(hwndCtl, clrBk)     ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETBKCOLOR, 0L, (LPARAM)(COLORREF)(clkBk) ))
#define Progress_SetBarColor(hwndCtl, clrBar)   ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETBARCOLOR, 0L, (LPARAM)(COLORREF)(clrBar) ))

// PBST_NORMAL, PBST_ERROR, PBST_PAUSED
#define Progress_SetState(hwndCtl, state)       ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETSTATE, (WPARAM)(int)(state), 0L ))



#define ScrollBar_SetPos(hwndCtl, pos, fRedraw)     SetScrollPos((hwndCtl), SB_CTL, (pos), (fRedraw))
#define ScrollBar_GetPos(hwndCtl)                   GetScrollPos((hwndCtl), SB_CTL)

#define ScrollBar_SetRange(hwndCtl, posMin, posMax, fRedraw)    SetScrollRange((hwndCtl), SB_CTL, (posMin), (posMax), (fRedraw))
#define ScrollBar_GetRange(hwndCtl, lpposMin, lpposMax)         GetScrollRange((hwndCtl), SB_CTL, (lpposMin), (lpposMax))

class dcxWin32InternetStream : public dc::dcStreamIn
{
    public:
        public:

            /**
             * Initializes this class
             */
            dcxWin32InternetStream();

            /**
             * Cleans up the connection
             */
            ~dcxWin32InternetStream();

            /**
             * Opens the given file on the internet for reading
             */
            bool open( const char* sessionName, const char* fileAddress );

            /**
             * Cleans up this connection
             */
            void close();

            /**
             * Always fails for internet streams, since these are non-seekable
             * @return false
             */
            bool seek( size_t location );

            /**
             * Shifts the cursor in the file relative to the current position
             *   @param amount Number of bytes to shift
             *   @return Whether or not the IO point could be changed
             */
            bool move( int amount );

            /**
             * Gets the current position of the stream, such that
             * calling seek(tell()) will not move the cursor.
             * @return The current stream location
             */
            size_t tell() { return 0; }

            /**
             * Gets data from this stream.  This is a blocking operation.
             *   @param buffer Destination for the data
             *   @param size How many bytes to read
             *   @return Operation success flag
             */
            bool read( void * buffer, size_t size ) { return false; }

            /**
             * Tries to read data from the stream.  If no data can be read at this
             * time, the method returns 'true' but sets bytesRead to 0.  If the
             * return value is 'false', the stream is no longer valid for reading.
             */
            bool scan( void* buffer, size_t bufferSize, size_t* bytesRead );

            /**
             * Determines whether or not the stream has reached the end of its input
             *   @return 'true' if there is no more data to be read
             */
            bool end();


    protected:

        /**
         * Resets this class
         */
        void zero();

    protected:

        HINTERNET mySession;
        HINTERNET myInternetFile;
        size_t myCurrentLocation;
};

dcxWin32InternetStream::dcxWin32InternetStream()
{
    zero();
}

dcxWin32InternetStream::~dcxWin32InternetStream()
{
    close();
}

bool dcxWin32InternetStream::open( const char* agentName, const char* url )
{
    mySession = InternetOpen( agentName,
                             LOCAL_INTERNET_ACCESS,
                             NULL,  // no proxy
                             INTERNET_INVALID_PORT_NUMBER,
                             INTERNET_FLAG_DONT_CACHE );

    if( APP_ERROR( mySession == NULL )( "Couldn't open session with agent %s", agentName ) )
        return false;

    // Get this file
    myInternetFile = InternetOpenUrl( mySession, url, NULL, 0xFFFFFFFF, INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RAW_DATA, 0 );
    if( APP_ERROR( myInternetFile == NULL )( "Couldn't connect to %s", url ) )
        return false;

    // Success
    return true;
}


void dcxWin32InternetStream::close()
{
    if( myInternetFile )
    {
        InternetCloseHandle( myInternetFile );
        myInternetFile = NULL;
    }

    if( mySession )
    {
        InternetCloseHandle( mySession );
        mySession = NULL;
    }

    zero();
}

bool dcxWin32InternetStream::seek( size_t location )
{
    return false;
}

bool dcxWin32InternetStream::move( int amount )
{
    return false;
}
/**
 * Tries to read data from the stream.  If no data can be read at this
 * time, the method returns 'true' but sets bytesRead to 0.  If the
 * return value is 'false', the stream is no longer valid for reading.
 */
bool dcxWin32InternetStream::scan( void* buffer, size_t bufferSize, size_t* bytesRead )
{
    CONFIRM( buffer && bytesRead ) else return false;
    DWORD fOk, dwBytesRead;
    do
    {
        fOk = InternetReadFile( myInternetFile, buffer, bufferSize, &dwBytesRead );
    } while( !(fOk || dwBytesRead) );

    // This case indicates that the stream has finished
    if( fOk && !dwBytesRead )
    {
        InternetCloseHandle( myInternetFile );
        myInternetFile = 0;
    }

    // Set the number of bytes
    *bytesRead = dwBytesRead;

    // Return the result code
    return fOk != 0;
}

/**
 * Determines whether or not the stream has reached the end of its input
 *   @return 'true' if there is no more data to be read
 */
bool dcxWin32InternetStream::end()
{
    return !myInternetFile;
}

void dcxWin32InternetStream::zero()
{
    mySession = 0;
    myInternetFile = 0;
    myCurrentLocation = 0;
}


const char* STRING_ERROR_BOX_TITLE = "Launcher Error";
const char* STRING_ERROR_PROCESSORINCOMPATIBLE = "Your computer is incompatible with Evidyon.\n\nThis game runs on AMD/Intel processors only.";
const char* STRING_ERROR_CANTCONNECTTOSERVER = "Couldn't connect to update server";

class EvidyonLauncherDialog : public dcx::dcxWin32DialogController
{
    public:

    protected:
};

#include <dc/filestream>

class DownloadDialog : public dcx::dcxWin32DialogController
{
    static const int ID_PROGRESS = 1100;
    static const int ID_EDITBOX_STATUS = 1101;
    static const int ID_BUTTON_STARTGAME = 1102;
    static const int ID_BUTTON_SETTINGS = 1103;
    static const int ID_BUTTON_EXIT = 1104;
    static const int ID_COMBO_DOWNLOADSRC = 1105;
    static const COLORREF SKIN_TRANSPARENT_COLOR = RGB(0,255,0);

    public:

        HWND myMainWindow;

        HWND getProgressBar() { return GetDlgItem( myMainWindow, ID_PROGRESS ); }
        HWND getEditboxStatus() { return GetDlgItem( myMainWindow, ID_EDITBOX_STATUS ); }
        HWND getStartgameButton() { return GetDlgItem( myMainWindow, ID_BUTTON_STARTGAME ); }
        HWND getSettingsButton() { return GetDlgItem( myMainWindow, ID_BUTTON_SETTINGS ); }
        HWND getExitButton() { return GetDlgItem( myMainWindow, ID_BUTTON_EXIT ); }
        HWND getComboDownloadSrc() { return GetDlgItem( myMainWindow, ID_COMBO_DOWNLOADSRC ); }

        void appendStatusLine( const char* text )
        {
            size_t newTextLen = strlen(text);
            HWND editBox = getEditboxStatus();
            int len = Edit_GetTextLength( editBox );

            size_t combinedLength = len + 1 + newTextLen + 5;
            char* currentText = new char[combinedLength];
            Edit_GetText( editBox, currentText, combinedLength );
            strcat_s( currentText, combinedLength, "\r\n" );
            strcat_s( currentText, combinedLength, text );
            Edit_SetText( editBox, currentText );
            delete[] currentText;

            Edit_SetSel( editBox, combinedLength, combinedLength );
            Edit_ScrollCaret( editBox );
        }

        struct UpdateFileData
        {
            std::string myNewerVersion;
            std::vector<std::string> myDownloadWebsites;
            std::vector<std::pair<std::string,size_t>> myFilesAndSizes;
            std::string myUpdateText;
        };

        void getCurrentVersion( std::string* currentVersion )
        {
            dc::dcFileStream fs;
            if( fs.open( "version", STREAM_TEXT ) )
            {
                fs.readString( currentVersion, 16 );
                fs.close();
            }
            else
                currentVersion->assign( VERSION_TEXT );
        }

        void saveCurrentVersion( const std::string& currentVersion )
        {
            dc::dcFileStream fs;
            if( APP_ERROR( !fs.open( "version", STREAM_TEXT|STREAM_OVERWRITE ) )( "Couldn't open version file" ) )
                return;
            fs.writeString( currentVersion );
            fs.close();
        }

        void execute( HINSTANCE hInstance, dcx::dcxCPUFeatures* cpuFeatures )
        {
            myMainWindow = createMainDialog( hInstance );

            std::string currentVersion;
            getCurrentVersion( &currentVersion );

            UpdateFileData update;
            bool clientNeedsUpdating;
            if( !downloadVersionFile( currentVersion.c_str(), &update, &clientNeedsUpdating ) )
            {
                errorMessageBox( "Couldn't contact the update server; please try again later" );
                goto EXIT;
            }

            // Add the update text to the output
            {
                std::string updateText = "--------------------------------------------\r\n\r\n" + update.myUpdateText + 
                                         "--------------------------------------------";
                appendStatusLine( updateText.c_str() );
            }

            // Update the client
            bool allowGameStart = false;
            if( clientNeedsUpdating  )
            {
                const char* downloadWebsite;
                if( pickDownloadSource( &update.myDownloadWebsites, &downloadWebsite ) )
                {
                    if( downloadUpdates( downloadWebsite, &update.myFilesAndSizes ) )
                    {
                        allowGameStart = true;
                    }
                    else
                    {
                        appendStatusLine( "ERROR!  Unable to download updates at this time.  If you are running Vista, this could "\
                                          "be due to the permissions for the directory in which Evidyon is installed being set to read-only." );
                    }
                }
                else
                    goto EXIT;
            }
            else
                allowGameStart = true;

            if( allowGameStart )
            {
                // Save the new version
                saveCurrentVersion( update.myNewerVersion );

                // Re-enable the "launch" button
                Button_Enable( getStartgameButton(), TRUE );
            }

            // Wait for the user to do something
            myLaunchGameFlag = false;
            while( dcx::dcxWin32StdMessagePump( myMainWindow ) && !myLaunchGameFlag );

            // Close the window
            if( IsWindow( myMainWindow ) )
            {
                EndDialog( myMainWindow, 0 );
                DestroyWindow( myMainWindow );
                dcx::dcxWin32StdMessagePump( 0 );

                // Launch the client
                std::string exeFile = "evidyon";

                STARTUPINFO startupInfo;
                ZeroMemory( &startupInfo, sizeof(startupInfo) );

                PROCESS_INFORMATION processInfo;

                // Build the version to run based on the computer's capabilities
                if( cpuFeatures->sse2 )
                {
                    exeFile.append( "_sse2" );
                }
                else if( cpuFeatures->sse )
                {
                    exeFile.append( "_sse" );
                }

                exeFile.append( ".exe" );
                
                // Start the server
                if( !CreateProcessA( exeFile.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE|ABOVE_NORMAL_PRIORITY_CLASS,
                                     NULL, NULL, &startupInfo, &processInfo ) )
                {
                    DWORD error = GetLastError();
                    MessageBox( NULL, "Failed to launch Evidyon", "Error", MB_OK );
                    goto EXIT;
                }

                // Run the client
                WaitForSingleObject( processInfo.hProcess, INFINITE );

                // Free the process and exit
                CloseHandle( processInfo.hProcess );
                CloseHandle( processInfo.hThread );
            }

EXIT:
            DestroyWindow( myMainWindow );
        }

        bool myLaunchGameFlag;

        void errorMessageBox( const char* text )
        {
        }

        bool downloadUpdates( const char* downloadWebsite, std::vector<std::pair<std::string,size_t>>* filesAndSizes )
        {
            std::vector<std::pair<std::string,std::string>> tempFilesToLocalFiles;

            // Initialize the progress bar.  All files are scaled to 65535 since the 32-bit range
            // doesn't seem to be working correctly.
            Progress_SetRange( getProgressBar(), 0, 65535 );

            // Repeat for each of the files
            for( std::vector<std::pair<std::string,size_t>>::iterator p = filesAndSizes->begin(); p != filesAndSizes->end(); ++p )
            {
                // Update the display window
                {
                    // Get the number of bytes
                    char bytes[16];
                    if( p->second > 1000 )
                    {
                        sprintf_s( bytes, "%lu KB", p->second / 1000 );
                    } else if( p->second > 1000000 )
                    {
                        sprintf_s( bytes, "%lu MB", p->second / 1000000 );

                    } else
                    {
                        sprintf_s( bytes, "%lu bytes", p->second );
                    }

                    std::string text = "Downloading ";
                    text.append( p->first );
                    text.append( " - " );
                    text.append( bytes );
                    appendStatusLine( text.c_str() );
                }

                // Get the file's name on the website
                std::string fileUrl = downloadWebsite;
                fileUrl.append( p->first );

                // Get the file's local name
                std::string fileLocalName = "~";
                fileLocalName.append( p->first );

                // Open the local file
                HANDLE hDestination;
                hDestination = CreateFile( fileLocalName.c_str(),
                                           GENERIC_WRITE,
                                           FILE_SHARE_READ,
                                           NULL,
                                           CREATE_ALWAYS,
                                           FILE_FLAG_WRITE_THROUGH | 
                                           FILE_FLAG_SEQUENTIAL_SCAN,
                                           NULL );
                CONFIRM( hDestination ) else return false;

                // Add to the list of local files
                tempFilesToLocalFiles.push_back( std::pair<std::string,std::string>( fileLocalName, p->first ) );

                // Download this file
                dcxWin32InternetStream reader;
                if( !reader.open( "EvidyonClient", fileUrl.c_str() ) )
                {
                    CloseHandle( hDestination );
                    return false;
                }

                // Reset the progress bar
                Progress_SetPos( getProgressBar(), 0 );

                char buffer[512];
                size_t bytesRead = 0;
                size_t bytes;
                while( reader.scan( buffer, sizeof(buffer), &bytes ) && !reader.end() )
                {
                    // Write this data into the output file
                    DWORD bytesWritten;
                    BOOL success = WriteFile( hDestination,
                                              buffer,
                                              bytes,
                                              &bytesWritten,
                                              NULL );

                    // Exit if the data couldn't be written or the user exited the launcher
                    if( !dcx::dcxWin32StdMessagePump( myMainWindow ) ||
                        APP_WARNING( !success || !bytesWritten )( "Couldn't write data to temporary file" ) )
                        break;

                    // Add to the number of bytes that were read
                    bytesRead += bytes;

                    // Update the progress bar
                    Progress_SetPos( getProgressBar(), (WORD)(((double)bytesRead / (double)p->second) * 65535) );
                }

                bool completed = reader.end();

                // Get rid of the output file and internet connection
                reader.close();
                CloseHandle( hDestination );

                // If we didn't finish the download, fail
                if( APP_ERROR( !completed )( "Didn't complete download of %s", fileUrl.c_str() ) )
                    return false;
            }

            // Delete original files and copy files over
            for( std::vector<std::pair<std::string,std::string>>::iterator i  = tempFilesToLocalFiles.begin();
                                                                           i != tempFilesToLocalFiles.end(); ++i )
            {
                // Copy over the existing file
                CopyFile( i->first.c_str(), i->second.c_str(), FALSE );

                // Erase the temporary file
                DeleteFile( i->first.c_str() );
            }

            // Max out the bar
            Progress_SetPos( getProgressBar(), 65535 );

            // Success
            return true;
        }

        bool downloadVersionFile( const char* currentVersionString, UpdateFileData* update, bool* needsUpdating )
        {
            ASSERT( currentVersionString && update && needsUpdating ) else return false;

            dcxWin32InternetStream versionReader;
            if( !versionReader.open( "EvidyonClient", EVIDYON_UPDATE_URL ) )
                return false;

            // Read the update text file
            char updateFile[2048];
            ZeroMemory( updateFile, sizeof(updateFile) );

            size_t bytesRead = 0;
            size_t bytes;
            while( versionReader.scan( &updateFile[bytesRead], 512, &bytes ) && !versionReader.end() )
            {
                // Handle the message loop
                dcx::dcxWin32StdMessagePump( myMainWindow );

                // Add to the number of bytes that were read
                bytesRead += bytes;
            }

            // Close the reader
            versionReader.close();

            // Compare the version
            char versionText[64];
            sscanf_s( updateFile, "%[^\r\n]", versionText, sizeof(versionText) );
            bool needsUpdate = false;
            update->myNewerVersion = versionText;

            // Display the current version
            {
                std::string currentVersionOutputText = "Update server contacted!  Current version is ";
                currentVersionOutputText.append( versionText );
                appendStatusLine( currentVersionOutputText.c_str() );
            }

            // Get information from the update file
            {
                // We have to update; build the information to do so
                std::string updateFileStr = updateFile;
                std::vector<std::string> lines;

                // Read each line of the file into the string
                while( !updateFileStr.empty() )
                {
                    size_t lineEnd = updateFileStr.find_first_of( "\r\n\0" );
                    lines.push_back( updateFileStr.substr( 0, lineEnd ) );
                    updateFileStr.erase( 0, updateFileStr.find_first_not_of( "\r\n\0", lineEnd ) );
                }

                // Repeat for each segment
                int segment = 0;
                for( std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); ++i )
                {
                    // This is the segment indicator
                    if( i->compare( "-" ) == 0 ) { ++segment; continue; }

                    // Send a segment
                    switch( segment )
                    {
                        // Download websites for the update
                        case 1:
                            update->myDownloadWebsites.push_back( *i );
                            break;
                        case 2:
                        {
                            // Parse out the components
                            char filename[256];
                            unsigned int size;
                            CONFIRM( 2 == sscanf_s( i->c_str(), "%[^(](%lu)", filename, sizeof(filename), &size ) ) else break;
                            update->myFilesAndSizes.push_back( std::pair<std::string,size_t>( filename, size ) );
                        } break;
                        case 3:
                            update->myUpdateText.append( *i + "\r\n\r\n" );
                            break;
                    }
                }
            }

            // Is this version different from the current one?  If so, we have to update.
            *needsUpdating = stricmp( versionText, currentVersionString ) != 0;
            return true;
        }

        bool pickDownloadSource( std::vector<std::string>* websites, const char** selectedWebsite )
        {
            ASSERT( websites && selectedWebsite ) else return false;
            HWND hCombo = getComboDownloadSrc();

            // Add the website options to the combo box
            for( std::vector<std::string>::iterator i = websites->begin(); i != websites->end(); ++i )
                ComboBox_AddString( hCombo, i->c_str() );

            ComboBox_Enable( hCombo, TRUE );
            ComboBox_SetCurSel( hCombo, 0 );

            // Wait for the user to pick something
            int curSel = 0;
            while( ((curSel <= 0) || ComboBox_GetDroppedState( hCombo ))  && dcx::dcxWin32StdMessagePump( myMainWindow ) )
                curSel = ComboBox_GetCurSel( hCombo );

            // If the user closes the dialog, just exit
            if( curSel <= 0 ) return false;

            // Hide the combo box, show the progress bar
            ComboBox_Enable( hCombo, FALSE );
            ShowWindow( getProgressBar(), SW_SHOWDEFAULT );

            // Set the website address from which to download
            *selectedWebsite = websites->at(curSel-1).c_str();
            return true;
        }

        HWND createMainDialog( HINSTANCE hInstance )
        {
            dcx::dcxWin32DialogTemplate tpl;

            // Create the dialog
            tpl.setSizeInDialogUnits( 469, 469 );
            tpl.setStyleAsPrompt();
            tpl.setStyle( WS_POPUP | WS_CLIPSIBLINGS | WS_SYSMENU | DS_ABSALIGN | DS_NOIDLEMSG | DS_CENTER );
            tpl.setTitle( "Evidyon" );
            tpl.addEditBoxInDialogUnits( ID_EDITBOX_STATUS, WS_VSCROLL|WS_BORDER|ES_MULTILINE|ES_READONLY|ES_LEFT|ES_AUTOVSCROLL, 10+55, 10 +50, 180, 70, "Contacting update server..." );
            tpl.addButtonInDialogUnits( ID_BUTTON_STARTGAME, WS_DISABLED, 20+55, 90+10+5+15+10 +50, 160, 20, "Play Evidyon" );
            tpl.addButtonInDialogUnits( ID_BUTTON_SETTINGS, WS_DISABLED, 20+55, 90+10+10+15+10+15+5 +50, 70, 20, "Change Settings" );
            tpl.addButtonInDialogUnits( ID_BUTTON_EXIT, 0, 180-70+55, 90+10+10+15+10+15+5 +50, 70, 20, "Quit Game" );
            tpl.addComboBoxInDialogUnits( ID_COMBO_DOWNLOADSRC, WS_DISABLED|CBS_DROPDOWNLIST|CBS_HASSTRINGS|CBS_SIMPLE, 10+55, 90+1+5 +50 - 10, 180, 150 );
            tpl.addProgressBarInDialogUnits( ID_PROGRESS, PBS_SMOOTH, 10+55, 90+10+5 +50 , 180, 15 );

            BYTE buffer[1024];
            HWND hWnd = createDialog( hInstance, tpl.compile( buffer, sizeof(buffer) ), NULL );

            if( dcx::dcxWin32WindowsVersionIs2000OrXP() )
            {
                // Set this window up as a layered window, which is only supported on 2000/xp systems 

                #define LWA_COLORKEY  0x00000001
                #define WS_EX_LAYERED  0x00080000

                typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
                lpfnSetLayeredWindowAttributes SetLayeredWindowAttributes;

                HMODULE hUser32 = GetModuleHandle("user32.dll");
                SetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32,"SetLayeredWindowAttributes");
                SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong( hWnd, GWL_EXSTYLE) | WS_EX_LAYERED );
                SetLayeredWindowAttributes( hWnd, SKIN_TRANSPARENT_COLOR, 0, LWA_COLORKEY );
            }
            else
            {
                // Use a region to define this window.  This is less efficient on new systems than using the layered approach
                HRGN hRegion = CreateEllipticRgn( 0, 0, 469, 469 );
                SetWindowRgn( hWnd, hRegion, TRUE );
            }

            // Add the default text to the combo box
            ComboBox_AddString( GetDlgItem( hWnd, ID_COMBO_DOWNLOADSRC ), "(Pick a source to begin updating)" );

            // Update the window.  Get rid of the progress bar for now, since it doesn't do
            // anything until an update needs to be downloaded.
            ShowWindow( GetDlgItem( hWnd, ID_PROGRESS ), SW_HIDE );
            ShowWindow( hWnd, SW_SHOW );

            // Return the window we just created
            return hWnd;
        }
//
//        void run( HINSTANCE hInstance )
//        {
//            HWND myMainWindow;
//            dcx::dcxWin32DialogTemplate tpl;
//
//            long dbu = GetDialogBaseUnits();
//            int pixelx = 469*2, pixely = 469*2;
//            int dialogunitX = (pixelx * 4) / LOWORD(dbu);
//            int dialogunitY = (pixely * 8) / HIWORD(dbu);
//
//            tpl.setSizeInDialogUnits( dialogunitX, dialogunitY );
//            tpl.setStyleAsPrompt();
//            tpl.setStyle( WS_POPUP   | WS_VISIBLE    | WS_CLIPSIBLINGS | WS_SYSMENU | DS_ABSALIGN |
//                          DS_NOIDLEMSG    | DS_CENTER );
//            tpl.setTitle( "Evidyon" );
//            tpl.addEditBoxInDialogUnits( ID_EDITBOX_STATUS, WS_BORDER|ES_MULTILINE|ES_READONLY|ES_LEFT, 10+55, 10 +50, 180, 90, "Contacting update server..." );
//            //tpl.addProgressBarInDialogUnits( 100, PBS_SMOOTH/*|PBS_MARQUEE*/, 10+55, 90+10+5 +50, 180, 15 );
//            tpl.addButtonInDialogUnits( ID_BUTTON_STARTGAME, WS_DISABLED, 20+55, 90+10+5+15+10 +50, 160, 20, "Launch Evidyon" );
//            tpl.addButtonInDialogUnits( ID_BUTTON_SETTINGS, WS_DISABLED, 20+55, 90+10+10+15+10+15+5 +50, 70, 20, "Change Settings" );
//            tpl.addButtonInDialogUnits( ID_BUTTON_EXIT, 0, 180-70+55, 90+10+10+15+10+15+5 +50, 70, 20, "Exit Launcher" );
//            tpl.addComboBoxInDialogUnits( ID_COMBO_DOWNLOADSRC, WS_DISABLED|CBS_DROPDOWNLIST|CBS_HASSTRINGS|CBS_SIMPLE, 10+55, 90+1+5 +50, 180, 150 );
//            BYTE buffer[1024];
//            myMainWindow = createDialog( hInstance, tpl.compile( buffer, 1024 ), NULL );
//
//            ComboBox_AddString( GetDlgItem( myMainWindow, ID_COMBO_DOWNLOADSRC ), "(Pick a source to begin updating)" );
//
//            //HRGN hRegion = CreateEllipticRgn( 5, 5, 469, 469 );
//            //SetWindowRgn( myMainWindow, hRegion, TRUE );
//
//#define LWA_COLORKEY  0x00000001
//#define WS_EX_LAYERED  0x00080000
//
//typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
//lpfnSetLayeredWindowAttributes SetLayeredWindowAttributes;
//
//HMODULE hUser32 = GetModuleHandle("user32.dll");
//SetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32,"SetLayeredWindowAttributes");
//SetWindowLong(myMainWindow, GWL_EXSTYLE, GetWindowLong(myMainWindow, GWL_EXSTYLE) | WS_EX_LAYERED);
//SetLayeredWindowAttributes( myMainWindow, RGB(0,255,0), 0, LWA_COLORKEY );
//
//
//            Edit_SetSel( GetDlgItem( myMainWindow , ID_EDITBOX_STATUS ), 0, 0 );
//            ComboBox_SetCurSel( GetDlgItem( myMainWindow, ID_COMBO_DOWNLOADSRC ), 0 );
//
//            HWND h = GetDlgItem( myMainWindow, 100 );
//            Progress_SetPos( h, 0 );
//            UpdateWindow( myMainWindow );
///*
//            while( windowsMessagePump() )
//            {
//            }
//            return;
//*/
//            dcxWin32InternetStream versionReader;
//            if( !versionReader.open( "EvidyonClient", EVIDYON_UPDATE_URL ) )
//                return;
//
//
//            {
//                // Read the update text file
//                //char* updateFile = new char[1024*16];
//                char updateFile[2048];
//                ZeroMemory( updateFile, sizeof(updateFile) );
//
//                size_t bytesRead = 0;
//                while( dcx::dcxWin32StdMessagePump( myMainWindow ) )
//                {
//                    unsigned char buffer[512];
//                    size_t bytes;
//                    if( !versionReader.scan( &updateFile[bytesRead], 512, &bytes ) || versionReader.end() )
//                        break;
//                    bytesRead += bytes;
//                }
//
//                // Compare the version
//                char versionText[64];
//                sscanf( updateFile, "%[^\r\n]", versionText );
//
//                if( stricmp( versionText, VERSION_TEXT ) != 0 )
//                {
//                    // We have to update; build the information to do so
//                    std::string updateFileStr = updateFile;
//                    std::vector<std::string> lines;
//
//                    // Read each line of the file
//                    while( !updateFileStr.empty() )
//                    {
//                        size_t lineEnd = updateFileStr.find_first_of( "\r\n\0" );
//                        lines.push_back( updateFileStr.substr( 0, lineEnd ) );
//                        updateFileStr.erase( 0, updateFileStr.find_first_not_of( "\r\n\0", lineEnd ) );
//                    }
//
//                    lines;
//                }
//
//
//
//
//
//
//                Button_Enable( GetDlgItem( myMainWindow, ID_BUTTON_STARTGAME ), TRUE );
//
//                //delete[] updateFile;
//
//
//            }
//            size_t bytesRead = 0;
//            Progress_SetRange( h, 0, 65535 );
//
//            while( dcx::dcxWin32StdMessagePump( myMainWindow ) )
//            {
//                //unsigned char buffer[512];
//                //size_t bytes;
//                //if( !versionReader.scan( buffer, 512, &bytes ) || versionReader.end() )
//                //    break;
//                //bytesRead += bytes;
//                //Progress_SetPos( h, (65535)*(bytesRead/32000000.0f) );
//            }
//
//            versionReader.close();
//        }


        INT_PTR dialogNotification( HWND hDlg, HWND hControl, UINT_PTR ControlID, UINT Notification )
        {
            if( ControlID == ID_BUTTON_EXIT )
            {
                EndDialog( hDlg, 0 );
                DestroyWindow( hDlg );
                return 0;
            }
            if( ControlID == ID_BUTTON_STARTGAME )
            {
                myLaunchGameFlag = true;
            }
            return 0;
        }

//------------------------------------------------------------------------------------------------
// Name: dialogEvent
// Desc: Default implementation
//------------------------------------------------------------------------------------------------
INT_PTR dialogEvent( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( uMsg == WM_CLOSE ) { EndDialog( hDlg, 0 ); DestroyWindow( hDlg ); }
    if( uMsg == WM_INITDIALOG )
    {
    }

    switch( uMsg )
    {
        case WM_NCPAINT:
        {
            PAINTSTRUCT ps;

            //HANDLE hBm = LoadImage( NULL, "evidyon-bg.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE );
            HANDLE hBm = LoadImage( (HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_SHARED );

            HDC hImgDc = CreateCompatibleDC( 0 );
            HGDIOBJ oldBM = SelectObject( hImgDc, (HGDIOBJ)hBm );

            BeginPaint( hDlg, &ps );
            BitBlt( ps.hdc, 0, 0, 469, 469, hImgDc, 0, 0, SRCCOPY );
            SelectObject( hImgDc, oldBM );
            DeleteObject( hBm );
            DeleteDC( hImgDc );
            EndPaint( hDlg, &ps );

            /*
            //ValidateRect( GetDlgItem( myMainWindow, 100 ), NULL );

            PAINTSTRUCT ps;
            HDC hdc = GetWindowDC(hDlg);//GetDCEx(hWnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);
            //HDC hdc = BeginPaint( hWnd, &ps );

            //HBITMAP hBm = LoadBitmap( GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BTRLOGINWINDOW) );
            HGDIOBJ oldBM = SelectObject( hImgDc, (HGDIOBJ)hBm );

            BitBlt( hdc, 0, 0, 400, 300, hImgDc, 0, 0, SRCCOPY );

            SelectObject( hImgDc, oldBM );

            DeleteObject( hBm ) ;

            DeleteDC( hImgDc );

            ReleaseDC( hDlg, hdc );
            EndPaint( hDlg, &ps );*/

        } break;

        case WM_LBUTTONDOWN:
        {
          // ---------------------------------------------------------
          // this is a common trick for easy dragging of the window.
          // this message fools windows telling that the user is
          // actually dragging the application caption bar.
          // ---------------------------------------------------------
          SendMessage(hDlg, WM_NCLBUTTONDOWN, HTCAPTION,NULL);
        } break;

        // Change the color of the dialog's background
        case WM_CTLCOLORDLG:
        {
            LOGBRUSH lb;
            lb.lbColor = SKIN_TRANSPARENT_COLOR;
            lb.lbHatch = 0;
            lb.lbStyle = BS_SOLID;
            return (INT_PTR)CreateBrushIndirect( &lb );
        } break;

/*
        case WM_NCLBUTTONUP:

            if( ((INT)wParam) == HTCLOSE )
            {
                DestroyWindow( hDlg );
            }
            else
                break;

            return 0;

        case WM_NCACTIVATE:

            return 0;

        case WM_NCHITTEST:
            {
                // Get the mouse's position
                POINT pt;
                GetCursorPos( &pt );

                // Find the window's current rectangle
                RECT rc;
                GetWindowRect( hDlg, &rc );

                // Subtract the mouse location
                pt.x -= rc.left;
                pt.y -= rc.top;

                // If the mouse is in the bar, return caption bar...
                if( (pt.x >= 4)   && (pt.y >= 4) &&
                    (pt.x <= 377) && (pt.y <= 16) )
                    return HTCAPTION;
                else if( (pt.x >= 382) && (pt.y >= 4) &&
                         (pt.x <= 394) && (pt.y <= 16) )
                {
                    //g_strUserName[0] = '\0';
                    //g_strPassword[0] = '\0';
                    return HTCLOSE;
                }
                else
                    return HTCLIENT;
            }

        case WM_NCMOUSEMOVE:
            {
                // TODO: highlight buttons
                break;
            }

        case WM_ERASEBKGND:


            return 0;*/
    }
    return 0;
}

};

//------------------------------------------------------------------------------------------------
// Name: WinMain
// Desc: 
//------------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, INT nArgs )
{
    // Find out what the CPU supports
    dcx::dcxCPUFeatures cpuFeatures;
    if( !dcx::dcxGetCPUFeatures( &cpuFeatures ) )
    {
        MessageBox( NULL, STRING_ERROR_PROCESSORINCOMPATIBLE, STRING_ERROR_BOX_TITLE, MB_OK );
        goto EXIT;
    }

    {
        INITCOMMONCONTROLSEX InitCtrlEx;
        InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
        InitCtrlEx.dwICC = ICC_TAB_CLASSES|ICC_PROGRESS_CLASS;
        InitCommonControlsEx(&InitCtrlEx);
/*
        // Initialize the current directory
    // Print the CWD
    char currentDirectory[512];
    GetCurrentDirectory( 512, currentDirectory );
    int i = 0; ++i;
    DEBUG_INFO( "win32-cwd:  %s", currentDirectory );
*/
        /*
        std::string launcherEXEFile = lpCmdLine;
        std::string directory = launcherEXEFile.substr( 0, launcherEXEFile.find_last_of( "\\" ) );
        SetCurrentDirectory( directory.c_str() );
        _chdir( directory.c_str() );*/

    }

    {
        DownloadDialog d;
        d.execute( hInstance, &cpuFeatures );
    }

/*
    {

    }*/

EXIT:

    // Finalize the debug log
    DEBUG_ATEXIT();

    // Quit
    return 0;
}
