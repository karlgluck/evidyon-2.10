#ifndef __DCXWIN32INTERNETSTREAM_H__
#define __DCXWIN32INTERNETSTREAM_H__

#include "../dc/dcstream.h"
#include <windows.h>
#include <wininet.h>

// this file requires "wininet.lib"
// #pragma comment(lib, "wininet.lib")


namespace dcx {

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


}


#endif
