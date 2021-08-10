/*******************************************************************************
 * Copyright (c) 2015 QFish <im@qfi.sh>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/
#ifndef __CSS_FOUNDATION_H_
#define __CSS_FOUNDATION_H_

#include "cssparser.h"

#ifdef __cplusplus
extern "C" {
#endif
    
struct CssInternalParser;

/**
 *  Positon, for error debug
 */
typedef struct {
    unsigned int line;
    unsigned int column;
    unsigned int offset;
} CssSourcePosition;

/**
 *  String
 */
typedef struct {
    char* data;
    size_t length;
    size_t capacity;
} CssParserString;

extern const CssParserString cssAsteriskString;

void cssprsr_string_to_lowercase(struct CssInternalParser* parser, CssParserString* string);

// Initializes a new CssParserString.
void cssprsr_string_init(struct CssInternalParser* parser, CssParserString* output);

// Appends some characters onto the end of the CssParserString.
void cssprsr_string_append_characters(struct CssInternalParser* parser, const char* str, CssParserString* output);

// Prepends some characters at the start of the CssParserString.
void cssprsr_string_prepend_characters(struct CssInternalParser* parser, const char* str, CssParserString* output);

// Transforms a CssParserString to characters.
const char* cssprsr_string_to_characters(struct CssInternalParser * parser, const CssParserString* str);

// Transforms a CssParserString to characters with a char prepended at the start of the CssParserString.
const char* cssprsr_string_to_characters_with_prefix_char(struct CssInternalParser * parser, const CssParserString* str, const char prefix);
    
// Appends a string onto the end of the CssParserString.
void cssprsr_string_append_string(struct CssInternalParser* parser, CssParserString* str, CssParserString* output);

// Returns a bool value that indicates whether a given string matches the beginning characters of the receiver.
bool cssprsr_string_has_prefix(const char* str, const char* prefix);

/**
 *  Number
 */
typedef struct {
    CssParserString raw;
    double val;
} CssParserNumber;

/**
 *  Array 
 */
// Initializes a new CssArray with the specified initial capacity.
void cssprsr_array_init(struct CssInternalParser* parser, size_t capacity, CssArray* array);

// Frees the memory used by an CssArray.  Does not free the contained
// pointers, but you should free the pointers if necessary.
void cssprsr_array_destroy(struct CssInternalParser* parser, CssArray* array);

// Adds a new element to an CssArray.
void cssprsr_array_add(struct CssInternalParser* parser, void* element, CssArray* array);

// Removes and returns the element most recently added to the CssArray.
// Ownership is transferred to caller.  Capacity is unchanged.  If the array is
// empty, NULL is returned.
void* cssprsr_array_pop(struct CssInternalParser* parser, CssArray* array);

// Inserts an element at a specific index.  This is potentially O(N) time, but
// is necessary for some of the spec's behavior.
void cssprsr_array_insert_at(struct CssInternalParser* parser, void* element, int index, CssArray* array);

// Removes an element from the array, or does nothing if the element is not in
// the array.
void cssprsr_array_remove(struct CssInternalParser* parser, void* element, CssArray* array);

// Removes and returns an element at a specific index.  Note that this is
// potentially O(N) time and should be used sparingly.
void* cssprsr_array_remove_at(struct CssInternalParser* parser, int index, CssArray* array);

/**
 *  An alloc / free method wrapper
 */
void* cssprsr_parser_alloc(struct CssInternalParser* parser, size_t size);
void cssprsr_parser_free(struct CssInternalParser* parser, void* ptr);
    
#ifdef __cplusplus
}
#endif

#endif /* __CSS_FOUNDATION_H_ */
