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
#ifndef _CSS_PARSER_I_H_
#define _CSS_PARSER_I_H_

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <strings.h>

#include "cssparser_lex.h"
#include "cssparser_tab.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSSPRSR_RFELX_DEBUG            0
#define CSSPRSR_RBISON_DEBUG           0
#define CSSPRSR_RPARSER_DEBUG          0
#define CSSPRSR_RPARSER_LOG_ENABLE     0

struct CssInternalOutput;
struct CssInternalOptions;

typedef void* (*CssAllocatorFunction)(void* userdata, size_t size);

typedef void (*CssDeallocatorFunction)(void* userdata, void* ptr);

typedef struct CssInternalOptions {
    CssAllocatorFunction allocator;
    CssDeallocatorFunction deallocator;
    void* userdata;
} CssOptions;

extern const CssOptions kCssDefaultOptions;

typedef struct CssInternalParser {
    // Settings for this parse run.
    const struct CssInternalOptions* options;
    
    // Output for the parse.
    struct CssInternalOutput* output;
    
    // The flex tokenizer info
    yyscan_t* scanner;
    
    // The floating declarations
    CssArray* parsed_declarations;

#if CSSPRSR_RPARSER_DEBUG
    // The floating selectors
    CssArray* parsed_selectors;
#endif

    CssSourcePosition* position;
    CssParserString default_namespace;
    
} CssParser;

CssArray* cssprsr_new_array(CssParser* parser);
CssStylesheet* cssprsr_new_stylesheet(CssParser* parser);
void cssprsr_parser_reset_declarations(CssParser* parser);
void cssprsr_add_namespace(CssParser* parser, CssParserString* prefix, CssParserString* uri);
CssRule* cssprsr_new_font_face(CssParser* parser);
CssRule* cssprsr_new_keyframes_rule(CssParser* parser, CssParserString* name, CssArray* keyframes, bool isPrefixed);
CssKeyframe* cssprsr_new_keyframe(CssParser* parser, CssArray* selectors);
CssArray* cssprsr_new_Keyframe_list(CssParser* parser);
void cssprsr_keyframe_rule_list_add(CssParser* parser, CssKeyframe* keyframe, CssArray* list);
void cssprsr_parser_clear_keyframes(CssParser* parser, CssArray* keyframes);
void cssprsr_set_charset(CssParser* parser, CssParserString* charset);
CssRule* cssprsr_new_import_rule(CssParser* parser, CssParserString* href, CssArray* media);
CssValue* cssprsr_new_value(CssParser* parser);
CssValue* cssprsr_new_dimension_value(CssParser* parser, CssParserNumber* value, CssValueUnit unit);
CssValue* cssprsr_new_number_value(CssParser* parser, int sign, CssParserNumber* value, CssValueUnit unit);
CssValue* cssprsr_new_operator_value(CssParser* parser, int value);
CssValue* cssprsr_new_ident_value(CssParser* parser, CssParserString* value);
CssValue* cssprsr_new_function_value(CssParser* parser, CssParserString* name, CssArray* args);
CssValue* cssprsr_new_list_value(CssParser* parser, CssArray* list);
void cssprsr_value_set_string(CssParser* parser, CssValue* value, CssParserString* string);
void cssprsr_value_set_sign(CssParser* parser, CssValue* value, int sign);
CssArray* cssprsr_new_value_list(CssParser* parser);
void cssprsr_value_list_add(CssParser* parser, CssValue* value, CssArray* list);
void cssprsr_value_list_insert(CssParser* parser, CssValue* value, int index, CssArray* list);
void cssprsr_value_list_steal_values(CssParser* parser, CssArray* values, CssArray* list);
CssRule* cssprsr_new_media_rule(CssParser* parser, CssArray* medias, CssArray* rules);
CssArray* cssprsr_new_media_list(CssParser* parser);
void cssprsr_media_list_add(CssParser* parser, CssMediaQuery* media_query, CssArray* medias);
CssMediaQuery* cssprsr_new_media_query(CssParser* parser, CssMediaQueryRestrictor r, CssParserString *type, CssArray* exps);
CssMediaQueryExp * cssprsr_new_media_query_exp(CssParser* parser, CssParserString* feature, CssArray* values);
CssArray* cssprsr_new_media_query_exp_list(CssParser* parser);
void cssprsr_media_query_exp_list_add(CssParser* parser, CssMediaQueryExp* exp, CssArray* list);
CssArray* cssprsr_new_rule_list(CssParser* parser);
CssArray* cssprsr_rule_list_add(CssParser* parser, CssRule* rule, CssArray* rule_list);
CssRule* cssprsr_new_style_rule(CssParser* parser, CssArray* selectors);
void cssprsr_start_declaration(CssParser* parser);
void cssprsr_end_declaration(CssParser* parser, bool flag, bool ended);
void cssprsr_set_current_declaration(CssParser* parser, CssParserString* tag);
bool cssprsr_new_declaration(CssParser* parser, CssParserString* name, bool important, CssArray* values);
void cssprsr_parser_clear_declarations(CssParser* parser);
void cssprsr_start_selector(CssParser* parser);
void cssprsr_end_selector(CssParser* parser);
CssQualifiedName * cssprsr_new_qualified_name(CssParser* parser, CssParserString* prefix, CssParserString* localName, CssParserString* uri);
CssSelector* cssprsr_new_selector(CssParser* parser);
CssSelector* cssprsr_sink_floating_selector(CssParser* parser, CssSelector* selector);
CssSelector* cssprsr_rewrite_specifier_with_element_name(CssParser* parser, CssParserString* tag, CssSelector* specifier);
CssSelector* cssprsr_rewrite_specifier_with_namespace_if_needed(CssParser* parser, CssSelector* specifier);
CssSelector* cssprsr_rewrite_specifiers(CssParser* parser, CssSelector* specifiers, CssSelector* newSpecifier);
void cssprsr_adopt_selector_list(CssParser* parser, CssArray* selectors, CssSelector* selector);
void cssprsr_selector_append(CssParser* parser, CssSelector* selector, CssSelector* new_selector, CssSelectorRelation relation);
void cssprsr_selector_insert(CssParser* parser, CssSelector* selector, CssSelector* new_selector, CssSelectorRelation before, CssSelectorRelation after);
void cssprsr_selector_prepend_with_element_name(CssParser* parser, CssSelector* selector, CssParserString* tag);
CssArray* cssprsr_new_selector_list(CssParser* parser);
CssArray* cssprsr_reusable_selector_list(CssParser* parser);
void cssprsr_selector_list_shink(CssParser* parser, int capacity, CssArray* list);
void cssprsr_selector_list_add(CssParser* parser, CssSelector* selector, CssArray* list);
void cssprsr_selector_set_value(CssParser* parser, CssSelector* selector, CssParserString* value);
void cssprsr_selector_set_argument(CssParser* parser, CssSelector* selector, CssParserString* argument);
void cssprsr_selector_set_argument_with_number(CssParser* parser, CssSelector* selector, int sign, CssParserNumber* value);
bool cssprsr_parse_attribute_match_type(CssParser* parser, CssAttributeMatchType, CssParserString* attr);
bool cssprsr_selector_is_simple(CssParser* parser, CssSelector* selector);
void cssprsr_selector_extract_pseudo_type(CssSelector* selector);
void cssprsr_add_rule(CssParser* parser, CssRule* rule);
void cssprsr_start_rule(CssParser* parser);
void cssprsr_end_rule(CssParser* parser, bool ended);
void cssprsr_start_rule_header(CssParser* parser, CssRuleType type);
void cssprsr_end_rule_header(CssParser* parser);
void cssprsr_end_invalid_rule_header(CssParser* parser);
void cssprsr_start_rule_body(CssParser* parser);
bool cssprsr_string_is_function(CssParserString* string);
void cssprsr_string_clear(CssParser* parser, CssParserString* string);
void cssprsr_parse_internal_rule(CssParser* parser, CssRule* e);
void cssprsr_parse_internal_keyframe_rule(CssParser* parser, CssKeyframe* e);
void cssprsr_parse_internal_keyframe_key_list(CssParser* parser, CssArray* e);
void cssprsr_parse_internal_value(CssParser* parser, CssArray* e);
void cssprsr_parse_internal_media_list(CssParser* parser, CssArray* e);
void cssprsr_parse_internal_declaration_list(CssParser* parser, bool e);
void cssprsr_parse_internal_selector(CssParser* parser, CssArray* e);

// Bison error
void cssprsr_error(CSSPARSERLTYPE* yyloc, void* scanner, CssParser * parser, char*);

// Bison parser location
CssSourcePosition* cssprsr_parser_current_location(CssParser* parser, CSSPARSERLTYPE* yylloc);

// Log
void cssprsr_parser_log(CssParser* parser, const char * format, ...);

// Error
void cssprsr_parser_resume_error_logging();
void cssprsr_parser_report_error(CssParser* parser, CssSourcePosition* pos, const char *, ...);

// print
void cssprsr_print(const char * format, ...);

void cssprsr_print_stylesheet(CssParser* parser, CssStylesheet* sheet);
void cssprsr_print_rule(CssParser* parser, CssRule* rule);
void cssprsr_print_font_face_rule(CssParser* parser, CssFontFaceRule* rule);
void cssprsr_print_import_rule(CssParser* parser, CssImportRule* rule);

void cssprsr_print_media_query_exp(CssParser* parser, CssMediaQueryExp* exp);
void cssprsr_print_media_query(CssParser* parser, CssMediaQuery* query);
void cssprsr_print_media_list(CssParser* parser, CssArray* medias);
void cssprsr_print_media_rule(CssParser* parser, CssMediaRule* rule);

void cssprsr_print_keyframes_rule(CssParser* parser, CssKeyframesRule* rule);
void cssprsr_print_keyframe(CssParser* parser, CssKeyframe* keyframe);

void cssprsr_print_style_rule(CssParser* parser, CssStyleRule* rule);
void cssprsr_print_selector(CssParser* parser, CssSelector* selector);
void cssprsr_print_selector_list(CssParser* parser, CssArray* selectors);
void cssprsr_print_declaration(CssParser* parser, CssDeclaration* decl);
void cssprsr_print_declaration_list(CssParser* parser, CssArray* declarations);
void cssprsr_print_value_list(CssParser* parser, CssArray* values);

int cssprsr_tokenize(CSSPARSERSTYPE* lval , CSSPARSERLTYPE* loc, yyscan_t scanner, CssParser* parser, int tok);

#ifdef __cplusplus
}
#endif

#endif /* _CSS_PARSER_I_H_ */
