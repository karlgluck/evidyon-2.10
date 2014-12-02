#include "../dc/dcbuffer.h"


namespace dc {

dcBuffer::Reader::Reader() {
  myBuffer = 0;
  myAccessPtr = 0;
}



//------------------------------------------------------------------------------------------------
// Name:  Reader
// Desc:  Sets up this reader to scan from a buffer
//------------------------------------------------------------------------------------------------
dcBuffer::Reader::Reader(dcBuffer* buffer)
{
    // Save the incoming parameters
    myBuffer = buffer;

    // Reset the access pointer
    myAccessPtr = 0;
}


void dcBuffer::Reader::reset(dcBuffer* buffer) {
  myBuffer = buffer;
  myAccessPtr = 0;
}



//------------------------------------------------------------------------------------------------
// Name:  seek
// Desc:  Moves the IO cursor to a specific point in the file
//------------------------------------------------------------------------------------------------
bool dcBuffer::Reader::seek(size_t location)
{
    // Can't seek beyond the buffer
    if (!myBuffer || myBuffer->getDataSize() <= location)
        return false;

    // Save the new location
    myAccessPtr = location;

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  move
// Desc:  Shifts the cursor in the file relative to the current position
//------------------------------------------------------------------------------------------------
bool dcBuffer::Reader::move(int amount)
{
    // Move to the new location
    return seek(myAccessPtr + amount);
}




//------------------------------------------------------------------------------------------------
// Name:  seek
// Desc:  Gets the current position of the stream
//------------------------------------------------------------------------------------------------
size_t dcBuffer::Reader::tell()
{
    return myAccessPtr;
}




//------------------------------------------------------------------------------------------------
// Name:  read
// Desc:  Gets data from this stream
//------------------------------------------------------------------------------------------------
bool dcBuffer::Reader::read(void* buffer, size_t size)
{
    // Make sure the data will be valid
    if (!myBuffer || (myAccessPtr + size > myBuffer->myDataSize))
        return false;

    // Read the data
    memcpy(buffer, &myBuffer->myMemory[myAccessPtr], size);

    // Advance the access pointer
    myAccessPtr += size;

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  end
// Desc:  Determines whether or not the stream has reached the end of its input
//------------------------------------------------------------------------------------------------
bool dcBuffer::Reader::end()
{
    return !myBuffer || (myAccessPtr >= myBuffer->myDataSize);
}





//------------------------------------------------------------------------------------------------
// Name:  dcBuffer
// Desc:  Initializes the buffer
//------------------------------------------------------------------------------------------------
dcBuffer::dcBuffer() {
    myMemory = 0;
    myDataSize = 0;
    myBufferSize = 0;
}




//------------------------------------------------------------------------------------------------
// Name:  ~dcBuffer
// Desc:  Cleans up when the buffer is no longer needed
//------------------------------------------------------------------------------------------------
dcBuffer::~dcBuffer() {
  if (myMemory) {
    delete[] myMemory;
    myMemory = 0;
  }
}




//------------------------------------------------------------------------------------------------
// Name:  readFrom
// Desc:  Reads data from a source stream into the buffer
//------------------------------------------------------------------------------------------------
bool dcBuffer::readFrom(dcStreamIn* stream, size_t bytes)
{
    // Reset the data size, just incase something goes wrong
    myDataSize = 0;

    // Reallocate the buffer, if necessary
    if (bytes > myBufferSize)
    {
        // Erase the old memory pointer
        if (myMemory) {
          delete[] myMemory;
          myMemory = NULL;
        }

        // Allocate new memory
        myMemory = new unsigned char[bytes];
        if (!myMemory) return false;

        // Save the buffer size
        myBufferSize = bytes;
    }

    // Read data
    if (!stream->read(myMemory, bytes)) return false;

    // Save the new data size
    myDataSize = bytes;

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  readFrom
// Desc:  
//------------------------------------------------------------------------------------------------
bool dcBuffer::readFrom(const void * buffer, size_t bytes) {

  // Reset the data size, just incase something goes wrong
  myDataSize = 0;

  // Reallocate the buffer, if necessary
  if (bytes > myBufferSize) {

    // Erase the old memory pointer
    if (myMemory) {
      delete[] myMemory;
      myMemory = NULL;
    }

    // Allocate new memory
    myMemory = new unsigned char[bytes];
    if (!myMemory) return false;

    // Save the buffer size
    myBufferSize = bytes;
  }

  memcpy(myMemory, buffer, bytes);
  myDataSize = bytes;

  // Success
  return true;
}




//------------------------------------------------------------------------------------------------
// Name:  getDataPointer
// Desc:  Gets the raw data associated with this buffer so that it can be manipulated
//------------------------------------------------------------------------------------------------
void* dcBuffer::getDataPointer() const
{
    return myMemory;
}




//------------------------------------------------------------------------------------------------
// Name:  getDataSize
// Desc:  Obtains the number of bytes of information stored in the buffer
//------------------------------------------------------------------------------------------------
size_t dcBuffer::getDataSize() const
{
    return myDataSize;
}

}