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
#include "foundation.h"
#include "cssparser_i.h"

struct CssInternalParser;

const CssParserString cssAsteriskString = {"*", 1};

static const size_t defaultStringBufferSize = 12;

static void maybe_resize_string(struct CssInternalParser* parser,
                                size_t additional_chars,
                                CssParserString* str) {
    size_t new_length = str->length + additional_chars;
    size_t new_capacity = str->capacity;
    while (new_capacity < new_length) {
        new_capacity *= 2;
    }
    if (new_capacity != str->capacity) {
        char* new_data = cssprsr_parser_alloc(parser, new_capacity);
        memset(new_data, 0, str->length);
        memcpy(new_data, str->data, str->length);
        cssprsr_parser_free(parser, str->data);
        str->data = new_data;
        str->capacity = new_capacity;
    }
}

void cssprsr_string_init(struct CssInternalParser* parser,
                        CssParserString* output) {
    output->data = cssprsr_parser_alloc(parser, defaultStringBufferSize);
    memset( output->data, 0, sizeof(defaultStringBufferSize) );
    output->length = 0;
    output->capacity = defaultStringBufferSize;
}

void cssprsr_string_append_characters(struct CssInternalParser* parser,
                                     const char* str, CssParserString* output)
{
    size_t len = strlen(str);
    maybe_resize_string(parser, len, output);
    memcpy(output->data + output->length, str, len);
    output->length += len;
}

void cssprsr_string_prepend_characters(struct CssInternalParser* parser,
                                      const char* str,
                                      CssParserString* output)
{
    size_t len = strlen(str);
    size_t new_length = output->length + len;
    char* new_data = cssprsr_parser_alloc(parser, new_length);
    memcpy(new_data, str, len);
    memcpy(new_data+len, output->data, output->length);
    cssprsr_parser_free(parser, output->data);
    output->data = new_data;
    output->length = new_length;
    output->capacity = new_length;
}

void cssprsr_string_append_string(struct CssInternalParser* parser,
                                 CssParserString* str,
                                 CssParserString* output) {
    maybe_resize_string(parser, str->length, output);
    memcpy(output->data + output->length, str->data, str->length);
    output->length += str->length;
}

bool cssprsr_string_has_prefix(const char* str, const char* prefix)
{
    size_t pre_len = strlen(prefix);
    size_t str_len = strlen(str);
    return pre_len <= str_len && strncasecmp(prefix, str, pre_len);
}

void cssprsr_string_to_lowercase(struct CssInternalParser* parser,
                                CssParserString* str)
{
    if ( !str )
        return;
    // FIXME: @(QFish) the char* in string piece is const, to find a better way
    char *c = (char*)str->data;
    for (int i=0; i < str->length; i++) {
        *c = tolower(*c);
        c++;
    }
}



const char* cssprsr_string_to_characters(struct CssInternalParser * parser, const CssParserString* str)
{
    assert(NULL != str);
    if (NULL == str)
        return NULL;
    
    char* buffer = cssprsr_parser_alloc(parser, sizeof(char) * (str->length + 1));
    memcpy(buffer, str->data, str->length);
    buffer[str->length] = '\0';
    return buffer;
}

const char* cssprsr_string_to_characters_with_prefix_char(struct CssInternalParser * parser, const CssParserString* str, const char prefix)
{
    assert(str);
    if (NULL == str)
        return NULL;
    
    char* buffer = cssprsr_parser_alloc(parser, sizeof(char) * (str->length + 2));
    memcpy((buffer + 1), str->data, str->length);
    buffer[0] = prefix;
    buffer[str->length] = '\0';
    return buffer;
}

/**
 * Array
 */
void cssprsr_array_init(struct CssInternalParser* parser,
                       size_t initial_capacity, CssArray* array) {
    array->length = 0;
    array->capacity = (unsigned int)initial_capacity;
    if (initial_capacity > 0) {
        array->data = cssprsr_parser_alloc(parser, sizeof(void*) * initial_capacity);
    } else {
        array->data = NULL;
    }
}

void cssprsr_array_destroy(struct CssInternalParser* parser,
                          CssArray* array) {
    if (array->capacity > 0) {
        cssprsr_parser_free(parser, array->data);
    }
}

static void enlarge_array_if_full(struct CssInternalParser* parser,
                                  CssArray* array) {
    if (array->length >= array->capacity) {
        if (array->capacity) {
            size_t old_num_bytes = sizeof(void*) * array->capacity;
            array->capacity *= 2;
            size_t num_bytes = sizeof(void*) * array->capacity;
            void** temp = cssprsr_parser_alloc(parser, num_bytes);
            memcpy(temp, array->data, old_num_bytes);
            cssprsr_parser_free(parser, array->data);
            array->data = temp;
        } else {
            // 0-capacity array; no previous array to deallocate.
            array->capacity = 2;
            array->data = cssprsr_parser_alloc(parser, sizeof(void*) * array->capacity);
        }
    }
}

void cssprsr_array_add(struct CssInternalParser* parser,
                      void* element, CssArray* array) {
    enlarge_array_if_full(parser, array);
    assert(array->data);
    assert(array->length < array->capacity);
    array->data[array->length++] = element;
}

void* cssprsr_array_pop(struct CssInternalParser* parser,
                       CssArray* array) {
    if (array->length == 0) {
        return NULL;
    }
    return array->data[--array->length];
}

int cssprsr_array_index_of(CssArray* array, void* element) {
    for (int i = 0; i < array->length; ++i) {
        if (array->data[i] == element) {
            return i;
        }
    }
    return -1;
}

void cssprsr_array_insert_at(struct CssInternalParser* parser,
                            void* element, int index,
                            CssArray* array) {
    assert(index >= 0);
    assert(index <= array->length);
    enlarge_array_if_full(parser, array);
    ++array->length;
    memmove(&array->data[index + 1], &array->data[index],
            sizeof(void*) * (array->length - index - 1));
    array->data[index] = element;
}

void cssprsr_array_remove(struct CssInternalParser* parser,
                         void* node, CssArray* array) {
    int index = cssprsr_array_index_of(array, node);
    if (index == -1) {
        return;
    }
    cssprsr_array_remove_at(parser, index, array);
}

void* cssprsr_array_remove_at(struct CssInternalParser* parser,
                             int index, CssArray* array) {
    assert(index >= 0);
    assert(index < array->length);
    void* result = array->data[index];
    memmove(&array->data[index], &array->data[index + 1],
            sizeof(void*) * (array->length - index - 1));
    --array->length;
    return result;
}

/**
 *  An alloc / free method
 */
void* cssprsr_parser_alloc(struct CssInternalParser* parser, size_t size) {
    return parser->options->allocator(parser->options->userdata, size);
}

void cssprsr_parser_free(struct CssInternalParser* parser, void* ptr) {
    parser->options->deallocator(parser->options->userdata, ptr);
}
