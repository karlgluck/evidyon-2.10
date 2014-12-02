#include "../dcx/dcxwin32internetstream.h"
#include "../dc/debug"

namespace dcx {

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
    mySession = InternetOpen(agentName,
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

}