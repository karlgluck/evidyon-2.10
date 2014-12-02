//------------------------------------------------------------------------------------------------
// File:    dcfilestream.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCFILESTREAM_H__
#define __DCFILESTREAM_H__



// These methods define how a stream will be opened
#define STREAM_OVERWRITE    1
#define STREAM_TEXT         2


namespace dc
{
    /**
     * 
     *   @author 
     */
    class dcFileStream : public dcStreamIn, public dcStreamOut
    {
        public:

            /**
             * Resets the class's internal variables
             */
            dcFileStream();

            /**
             * Gets rid of data used by the file stream object
             */
            ~dcFileStream();

            /**
             * Opens a file on fixed media
             *   @param name Path to the file
             *   @param how Combination of access specifiers
             *   @return Whether or not the operation succeeded
             */
            bool open( const char * name, int how );

            /**
             * Closes the file stream
             */
            void close();

            /**
             * Moves to a place within the file
             *   @param location Number of bytes offset from the start of the file
             *   @return 'true' if the location was valid
             */
            bool seek( size_t location );

            /**
             * Moves the file's read/write cursor
             *   @param amount How many bytes to move
             *   @return 'true' if the move succeeded
             */
            bool move( int amount );

            /**
             * Gets the current position of the file stream, such that
             * calling seek(tell()) will not move the cursor.
             * @return The current stream location
             */
            size_t tell();

            /**
             * Gets data from the file
             *   @param buffer Destination for the bytes
             *   @param size How many bytes to read
             *   @return Operation success flag
             */
            bool read( void * buffer, size_t size );

            /**
             * Determines EOF condition
             *   @return 'true' if there is no more data to be read
             */
            bool end();

            /**
             * Puts data into the file
             *   @param buffer Buffer of data to write
             *   @param size How many bytes to write
             *   @return Whether or not the writing was successful
             */
            bool write( const void * buffer, size_t size );

            /**
             * Forces this file stream to write to the disk
             */
            void flush();

            /**
             * Gets the number of bytes in the source file
             */
            size_t size();


        private:

            /// Source file
            FILE* myFile;
    };

}



#endif // __DC7FILESTREAM_HPP__
