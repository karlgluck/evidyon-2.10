//------------------------------------------------------------------------------------------------
// File:    dcxcpu.h
//
// Desc:    Reads a mask that defines the features supported by the CPU
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "../dcx/dcxcpu.h"
#include "../dc/debug"



using namespace dcx;



//------------------------------------------------------------------------------------------------
// Name:  dcxGetCPUFeatures
// Desc:  Obtains the operating features of the CPU using the CPUID assembler instruction
//------------------------------------------------------------------------------------------------
bool dcx::dcxGetCPUFeatures( dcxCPUFeatures* features )
{
    int hasCPUID = 0;
    unsigned long standardFeatures = 0; // Standard IA features
    unsigned long extendedFeatures = 0; // Extended AMD style features

    char manufacturer[13];

	// Use CPUID to get features of the processor
	manufacturer[12] = 0;
	_asm
	{
        // First check for a CPUID instruction (at least a 486) //

        pushfd      // push EFLAGS to stack

        pop eax     // pop EFLAGS back to eax

        mov edx, eax    // copy EFLAGS for later

        xor eax, 1<<21  // Toggle bit 21 (ID)

        push eax    // push changed value to stack

        popfd       // put toggled value back into EFLAGS

        pushfd      // push EFLAGS back to stack

        pop eax     // get current EFLAGS back from stack

        xor eax,edx // see if bit 21 (ID) has changed

        jz short done   // didn't change, doesn't have a CPUID instruction

        mov [hasCPUID], 1   // Supports CPUID


        // Get Manufacturer

        mov eax, 0  // CPUID function 0 : Manufacturer + highest CPUID

        cpuid
        mov dword ptr manufacturer, ebx     //

        mov dword ptr manufacturer+4, edx	// Get Manufacturer

        mov dword ptr manufacturer+8, ecx	//


        // AMD ('AuthenticAMD')? //

        cmp ebx, 'htuA'
        jne short not_amd
        cmp edx, 'itne'
        jne short not_amd
        cmp ecx, 'DMAc'
        je get_features
        not_amd:

        // Intel ('GenuineIntel')? //

        cmp ebx, 'uneG'
        jne short not_intel
        cmp edx, 'Ieni'
        jne short not_intel
        cmp ecx, 'letn'
        je get_features
        not_intel:

        // Cyrix ('CyrixInstead')? //

        cmp ebx, 'iryC'
        jne short not_cyrix
        cmp edx, 'snIx'
        jne short not_cyrix
        cmp ecx, 'deat'
        je get_features
        not_cyrix:

        get_features:
        cmp eax, 1      // Check for any standard CPUID functions

        jb short done   // Can't get features

        mov eax, 1      // CPUID function 1 : Features + Version

        cpuid
        mov [standardFeatures], edx// Store standard CPU features list 


        // Next find out which extended features are present (if any) //

        mov eax, 0x80000000     // CPUID 80000000 : how many extended features

        cpuid
        cmp eax, 0x80000001     // check for at least 1 extended function

        jb short done           // not found - K5 model0 or other unknown

        mov eax, 0x80000001     // CPUID 80000001 : Extended features list

        cpuid
        mov [extendedFeatures], edx // Store extended CPU features list

        done:
	}

    DEBUG_INFO( "CPU type: \"%s\"", manufacturer );

	if( APP_WARNING( !hasCPUID )( "CPU doesn't support CPUID instruction" ) ) return false;

	// FPU (Floating Point Unit)
    features->fpu = 1 & (standardFeatures >> 0);
    APP_WARNING( !features->fpu )( "CPU doesn't have a floating-point unit!" );

    // Detect features
    features->mmx = 1 & (standardFeatures >> 23);       // MMX (MultiMedia eXtensions)
    features->amd_mmx = 1 & (extendedFeatures >> 22);   // AMD MMX Extensions
    features->amd_3dnow = 1 & (extendedFeatures >> 31); // AMD 3DNow!
    features->amd_e3dnow = 1 & (extendedFeatures >> 30);// AMD 3DNow! Extensions
    features->sse = 1 & (standardFeatures >> 25);       // SSE
    features->sse2 = 1 & (standardFeatures >> 26);      // SSE2
    features->htt = 1 & (standardFeatures >> 28);       // HyperThreading Technology

    // Succcess
	return true;
}
