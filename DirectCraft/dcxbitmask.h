//------------------------------------------------------------------------------------------------
// File:    dcxbitmask.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXBITMASK_H__
#define __DCXBITMASK_H__


namespace dcx {




template <const size_t BitsInMask, typename MaskWordType = size_t> struct dcxBitMask
{
    typedef MaskWordType WORD_TYPE;
    static const size_t BITS_IN_WORD = sizeof(MaskWordType)*8;
    static const size_t MASK_SIZE = 1 + BitsInMask/BITS_IN_WORD;

    void copy(const dcxBitMask& other) {
      for( size_t i = 0; i < MASK_SIZE; ++i )
          values[i] = other.values[i];
    }

    void clearAll()
    {
        for( size_t i = 0; i < MASK_SIZE; ++i )
            values[i] = 0;
    }

    void setAll()
    {
        for( size_t i = 0; i < MASK_SIZE; ++i )
            for( size_t b = 0; b < sizeof(MaskWordType); ++b )
                ((unsigned char*)&(values[i]))[b] = 0xFF;
    }

    void set( size_t bitIndex )
    {
        if( bitIndex < BitsInMask )
            values[bitIndex/BITS_IN_WORD] |= (MaskWordType)(1<<(bitIndex%BITS_IN_WORD));
    }

    // sets the bits from [lower, upper)
    void setRange(size_t lower, size_t upper) {
      if (upper > BitsInMask) upper = BitsInMask;
      //values[word] |= ~((~WORD_TYPE(0)) >> (BITS_IN_WORD - (lower % BITS_IN_WORD)));
      while (lower < upper) {
        set(lower);
        ++lower;
      }
    }

    void clear( size_t bitIndex )
    {
        if( bitIndex < BitsInMask )
            values[bitIndex/BITS_IN_WORD] &= ~((MaskWordType)(1<<(bitIndex%BITS_IN_WORD)));
    }
    
    void toggle( size_t bitIndex )
    {
        if( bitIndex < BitsInMask )
            values[bitIndex/BITS_IN_WORD] ^= ((MaskWordType)(1<<(bitIndex%BITS_IN_WORD)));
    }

    bool get( size_t bitIndex ) const
    {
        return 1 == (( bitIndex < BitsInMask ) ? 1&(values[bitIndex/BITS_IN_WORD] >> (bitIndex % BITS_IN_WORD)) : 0);
    }

    bool allSet() const {
      size_t i = 0;
      while (i < MASK_SIZE) { if (values[i++] != ~((MaskWordType)0)) return false; }
      return true;
    }

    bool allClear() const {
      size_t i = 0;
      while (i < MASK_SIZE) { if (values[i++] != 0) return false; }
      return true;
    }

    size_t countBitsSet() const {
      size_t c = 0; // c accumulates the total bits set
      for (size_t i = 0; i < MASK_SIZE; ++i) {
        MaskWordType v = values[i];
        while (v) {
          v &= v - 1; // clear the least significant bit set
          c++;
        }
      }
      return c;
    }

    MaskWordType values[MASK_SIZE];
};







}

#endif