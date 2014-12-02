//------------------------------------------------------------------------------------------------
// File:    dcxcpu.h
//
// Desc:    Reads a mask that defines the features supported by the CPU
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXCPU_H__
#define __DCXCPU_H__



namespace dcx {

    /**
     * Defines basic features of the CPU
     */
    struct dcxCPUFeatures
    {
        unsigned fpu        : 1;    // FPU (Processor is newer than a 386/486)
        unsigned mmx        : 1;    // MMX
        unsigned amd_mmx    : 1;    // AMD MMX Extensions
        unsigned amd_3dnow  : 1;    // AMD 3DNow!
        unsigned amd_e3dnow : 1;    // AMD 3DNow! Extensions
        unsigned sse        : 1;    // SSE (Intel Streaming SIMD Extensions)
        unsigned sse2       : 1;    // SSE2
        unsigned htt        : 1;    // HyperThreading Technology
    };

    /**
     * Obtains the operating features of the CPU using the CPUID assembler instruction
     * @param features Output mask defining which features the CPU supports
     * @return Whether or not the CPU supports CPUID (if false, this method can't be used)
     */
    bool dcxGetCPUFeatures( dcxCPUFeatures* features );

}



#endif