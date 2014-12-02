//------------------------------------------------------------------------------------------------
// File:    dcbufferstream.h
//
// Desc:    Implements a stream that writes to a data buffer in memory
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCBUFFERSTREAM_H__
#define __DCBUFFERSTREAM_H__


namespace dc {

    /**
     * Implements a stream interface that writes to a buffer
     *   @author Karl Gluck
     */
    class dcBufferStream : public dcStreamIn, public dcStreamOut
    {
        public:

            /**
             * Resets the class's internal variables
             */
            dcBufferStream();

            /**
             * Cleans up the class
             */
            ~dcBufferStream();

            /**
             * Moves to a place within the stream
             *   @param location Number of bytes offset from the start of the file
             *   @return 'true' if the location was valid
             */
            bool seek( size_t location );

            /**
             * Moves the buffer's read/write cursor
             *   @param amount How many bytes to move
             *   @return 'true' if the move succeeded
             */
            bool move( int amount );

            /**
             * Gets data from the buffer
             *   @param buffer Destination for the bytes
             *   @param size How many bytes to read
             *   @return Operation success flag
             */
            bool read( void* buffer, size_t size );

            /**
             * Determines EOF condition
             *   @return 'true' if there is no more data to be read
             */
            bool end();

            /**
             * Puts data into the buffer
             *   @param buffer Buffer of data to write
             *   @param size How many bytes to write
             *   @return Whether or not the writing was successful
             */
            bool write( const void* buffer, size_t size );

            /**
             * Gets the current position of the stream, such that
             * calling seek(tell()) will not move the cursor.
             * @return The current stream location
             */
            size_t tell();


        private:

            /// The internal expanding buffer
            unsigned char* myBuffer;

            /// How large the buffer is
            size_t myBufferSize;

            /// The location of the read/write pointer
            size_t myAccessPtr;
    };

}

#endif
