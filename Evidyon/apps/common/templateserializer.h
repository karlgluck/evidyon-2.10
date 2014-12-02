#ifndef __TEMPLATESERIALIZER_H__
#define __TEMPLATESERIALIZER_H__

namespace TemplateSerializer {

// Uses a data template mask to compile a message.  The mask is a string
// with characters in the order of the variable parameters that represent
// the following values:
// f - float
// d - double
// c - char
// C - unsigned char
// s - short
// S - unsigned short
// l - long
// L - unsigned long
// r - size of structure (size_t), pointer to structure
// a{CSL} - # of elements in array, pointer to first array elements
char* compile(const char* mask, char* buffer, const char* end_of_buffer, ...);

// Interprets a data structure created using compileMessage.  The mask
// should be the same as the one that was used during compilation.
// f - float*
// d - double*
// c - char*
// C - unsigned char*
// s - short*
// S - unsigned short*
// l - long*
// L - unsigned long*
// r - size of structure (size_t), pointer to structure buffer
// a{CSL} - pointer to number of entries in buffer, buffer size in entries (size_t)
const char* decompile(const char* mask, const char* buffer, const char* end_of_buffer, ...);

}

#endif