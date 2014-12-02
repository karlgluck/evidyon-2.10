#ifndef __DCXASYNCHRONOUSSISOTHREADDATACONTROLLER_H__
#define __DCXASYNCHRONOUSSISOTHREADDATACONTROLLER_H__

#include "dcxbitmask.h"

namespace dcx {

template <typename DataType, const size_t Entries> class dcxAsynchronousSISOThreadDataController
{
public:

  dcxAsynchronousSISOThreadDataController()
  {
      myDataArray = 0;
      myWritingThreadDataMask.clearAll();
      myReadingThreadDataMask.clearAll();
  }

  bool create()
  {
      myDataArray = new DataType[Entries];
      return myDataArray != 0;
  }
  
  void destroy()
  {
      if( myDataArray ) delete[] myDataArray;
      myDataArray = 0;
  }

  bool writeByAssignment( const DataType data )
  {
      size_t index = nextWriteableIndex();
      if( index < Entries )
      {
          // We found a location!  Write here and update the mask
          myDataArray[index] = data;
          myWritingThreadDataMask.toggle( index );
          return true;
      }
      else
          return false;
  }
  
  bool readByAssignment( DataType* returnValue )
  {
      size_t index = nextReadableIndex();
      if( index < Entries )
      {
          // We found a location!  Copy the data into a temporary variable and return it
          *returnValue = myDataArray[index];
          myReadingThreadDataMask.toggle( index );
          return true;
      }
      else
          return false;
  }

  bool isValid() const
  {
      return myDataArray != 0;
  }
  
protected:

  size_t nextWriteableIndex() const
  {
      if( !isValid() ) return ((size_t)(-1));

      for( size_t i = 0; i < dcxBitMask<Entries>::MASK_SIZE; ++i )
      {
          dcxBitMask<Entries>::WORD_TYPE writing, reading;
          writing = myWritingThreadDataMask.values[i];
          reading = myReadingThreadDataMask.values[i];
          
          // If a bit is the same for both entries, it is available
          // to be written.  Find an entry with this quality.
          dcxBitMask<Entries>::WORD_TYPE available = ~(writing ^ reading);
          
          size_t currentEntry = i * dcxBitMask<Entries>::BITS_IN_WORD;
          while( available )
          {
              // If this entry can be written, return its index
              if( 1 == (available & 1) ) return currentEntry;

              ++currentEntry;
              available >>= 1;
          }
      }
      
      // Couldn't find an entry
      return ((size_t)(-1));
  }

  size_t nextReadableIndex() const
  {
      if( !isValid() ) return ((size_t)(-1));

      for( size_t i = 0; i < dcxBitMask<Entries>::MASK_SIZE; ++i )
      {
          dcxBitMask<Entries>::WORD_TYPE writing, reading;
          writing = myWritingThreadDataMask.values[i];
          reading = myReadingThreadDataMask.values[i];
          
          // If a bit is different between entries, it is available
          // for reading.
          dcxBitMask<Entries>::WORD_TYPE available = (writing ^ reading);
          
          size_t currentEntry = i * dcxBitMask<Entries>::BITS_IN_WORD;
          while( available ) // todo: use a bit twiddling hack to make this faster
          {
              // If this entry can be written, return its index
              if( 1 == (available & 1) ) return currentEntry;

              ++currentEntry;
              available >>= 1;
          }
      }
      
      // Couldn't find an entry
      return ((size_t)(-1));
  }
  
protected:

  volatile DataType* myDataArray;
  dcx::dcxBitMask<Entries> myWritingThreadDataMask;
  dcx::dcxBitMask<Entries> myReadingThreadDataMask;
};

}

#endif