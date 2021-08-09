/**
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
 */
#include <strings.h>

#include "selector.h"

#include "cssparser_i.h"

#define breakpoint

#define CSS_PARSER_STRING(literal) { literal, sizeof(literal) - 1 }


typedef void (*CssArrayDeallocator)(CssParser* parser, void* e);

#undef  cssprsr_destroy_array
#define cssprsr_destroy_array(parser, callback, e) \
        cssprsr_destroy_array_using_deallocator((parser), \
                    (CssArrayDeallocator)(&(callback)), (e))


void cssprsr_destroy_stylesheet(CssParser* parser, CssStylesheet* e);
void cssprsr_destroy_rule(CssParser* parser, CssRule* e);
void cssprsr_destroy_font_face_rule(CssParser* parser, CssFontFaceRule* e);
void cssprsr_destroy_import_rule(CssParser* parser, CssImportRule* e);
void cssprsr_destroy_media_rule(CssParser* parser, CssMediaRule* e);
void cssprsr_destroy_media_query(CssParser* parser, CssMediaQuery* e);
void cssprsr_destroy_media_query_exp(CssParser* parser, CssMediaQueryExp* e);
void cssprsr_destroy_keyframes_rule(CssParser* parser, CssKeyframesRule* e);
void cssprsr_destroy_keyframe(CssParser* parser, CssKeyframe* e);
void cssprsr_destroy_media_list(CssParser* parser, CssArray* e);
void cssprsr_destroy_rule_list(CssParser* parser, CssArray* e);
void cssprsr_destroy_style_rule(CssParser* parser, CssStyleRule* e);
void cssprsr_destroy_qualified_name(CssParser* parser, CssQualifiedName* e);
void cssprsr_destroy_selector(CssParser* parser, CssSelector* e);
void cssprsr_destroy_rare_data(CssParser* parser, CssSelectorRareData* e);
void cssprsr_destroy_declaration(CssParser* parser, CssDeclaration* e);
void cssprsr_destroy_value(CssParser* parser, CssValue* e);
void cssprsr_destroy_function(CssParser* parser, CssValueFunction* e);

void cssprsr_destroy_array_using_deallocator(CssParser* parser,
                          CssArrayDeallocator deallocator, CssArray* array);



extern int cssparse(void* scanner, struct CssInternalParser * parser);

static CssOutput* cssprsr_parse_with_options(const CssOptions* options,
                                               yyconst char* bytes,
                                               size_t len,
                                               CssParserMode mode);

static CssOutput* cssprsr_parse_fragment(const char* prefix,
                                           size_t pre_len,
                                           const char* string,
                                           size_t str_len,
                                           CssParserMode mode);

static const char* cssprsr_stringify_value_list(CssParser* parser, CssArray* value_list);
static const char* cssprsr_stringify_value(CssParser* parser, CssValue* value);

static void* malloc_wrapper(void* unused, size_t size) {
    return malloc(size);
}

static void free_wrapper(void* unused, void* ptr) {
    free(ptr);
}

const CssOptions kCssDefaultOptions = {
    &malloc_wrapper,
    &free_wrapper,
    NULL
};

static void output_init(CssParser* parser, CssParserMode mode)
{
    CssOutput* output = cssprsr_parser_allocate(parser, sizeof(CssOutput));
    output->stylesheet = cssprsr_new_stylesheet(parser);
    output->mode = mode;
    cssprsr_array_init(parser, 0, &output->errors);
    parser->output = output;
}

void css_destroy_output(CssOutput* output)
{
    if ( NULL == output )
        return;

    CssParser parser;
    parser.options = &kCssDefaultOptions;
    switch (output->mode) {
        case CssParserModeStylesheet:
            break;
        case CssParserModeRule:
            if ( NULL != output->rule ) {
                cssprsr_destroy_rule(&parser, output->rule);
            }
            break;
        case CssParserModeKeyframeRule:
            if ( NULL != output->keyframe ) {
                cssprsr_destroy_keyframe(&parser, output->keyframe);
            }
            break;
        case CssParserModeKeyframeKeyList:
            if ( NULL != output->keyframe_keys ) {
                cssprsr_destroy_array(&parser, cssprsr_destroy_value, output->keyframe_keys);
                cssprsr_parser_deallocate(&parser, (void*) output->keyframe_keys);
                output->keyframe_keys = NULL;
            }
            break;
        case CssParserModeMediaList:
            if ( NULL != output->medias ) {
                cssprsr_destroy_media_list(&parser, output->medias);
            }
            break;
        case CssParserModeValue:
            if ( NULL != output->values ) {
                cssprsr_destroy_array(&parser, cssprsr_destroy_value, output->values);
                cssprsr_parser_deallocate(&parser, (void*) output->values);
                output->values = NULL;
            }
            break;
        case CssParserModeSelector:
            if ( NULL != output->selectors ) {
                cssprsr_destroy_array(&parser, cssprsr_destroy_selector, output->selectors);
                cssprsr_parser_deallocate(&parser, (void*) output->selectors);
                output->selectors = NULL;
            }
            break;
        case CssParserModeDeclarationList:
            if ( NULL != output->declarations ) {
                cssprsr_destroy_array(&parser, cssprsr_destroy_declaration, output->declarations);
                cssprsr_parser_deallocate(&parser, (void*) output->declarations);
                output->declarations = NULL;
            }
            break;
    }
    cssprsr_destroy_stylesheet(&parser, output->stylesheet);
    cssprsr_array_destroy(&parser, &output->errors);
    cssprsr_parser_deallocate(&parser, output);
}


static const CssParserString kCssParserModePrefixs[] = {
    CSS_PARSER_STRING(""),
    CSS_PARSER_STRING("@-internal-rule "), // 16
    CSS_PARSER_STRING("@-internal-keyframe-rule "), // 25
    CSS_PARSER_STRING("@-internal-keyframe-key-list "), // 29
    CSS_PARSER_STRING("@-internal-media-list "), // 22
    CSS_PARSER_STRING("@-internal-value "), // 17
    CSS_PARSER_STRING("@-internal-selector "), // 20
    CSS_PARSER_STRING("@-internal-decls "), // 17
};

CssOutput* css_parse_string(const char* str, size_t len, CssParserMode mode)
{
    switch (mode) {
        case CssParserModeStylesheet:
            return cssprsr_parse_with_options(&kCssDefaultOptions, (yyconst char*)str, len, mode);
        case CssParserModeRule:
        case CssParserModeKeyframeRule:
        case CssParserModeKeyframeKeyList:
        case CssParserModeMediaList:
        case CssParserModeValue:
        case CssParserModeSelector:
        case CssParserModeDeclarationList: {
            CssParserString prefix = kCssParserModePrefixs[mode];
            return cssprsr_parse_fragment(prefix.data, prefix.length, str, len, mode);
        }
        default:
            cssprsr_print("Whoops, not support yet!");
            return NULL;
    }
}

CssOutput* css_parse_file(FILE* fp)
{
    assert(NULL != fp);
    if ( NULL == fp )
        return NULL;
    
    yyscan_t scanner;
    if (cssprsr_lex_init(&scanner)) {
        cssprsr_print("no scanning today!");
        return NULL;
    }
    
    cssprsr_set_in(fp, scanner);
    
    CssParser parser;
    parser.options = &kCssDefaultOptions;
    parser.scanner = &scanner;
    parser.default_namespace = cssAsteriskString;
    parser.parsed_declarations = cssprsr_new_array(&parser);
#if CSSPRSR_RPARSER_DEBUG
    parser.parsed_selectors = cssprsr_new_array(&parser);
#endif // #if CSSPRSR_RPARSER_DEBUG
    parser.position = cssprsr_parser_allocate(&parser, sizeof(CssSourcePosition));
    output_init(&parser, CssParserModeStylesheet);
    cssparse(scanner, &parser);
    cssprsr_lex_destroy(scanner);
    cssprsr_parser_clear_declarations(&parser);
    cssprsr_parser_deallocate(&parser, parser.position);
#if CSSPRSR_RPARSER_DEBUG
    cssprsr_destroy_array(&parser, cssprsr_destroy_selector, parser.parsed_selectors);
    cssprsr_parser_deallocate(&parser, parser.parsed_selectors);
#endif // #if CSSPRSR_RPARSER_DEBUG
    parser.scanner = NULL;
    CssOutput* output = parser.output;
    return output;
}


static CssOutput* cssprsr_parse_fragment(const char* prefix,
                                           size_t pre_len,
                                           const char* str,
                                           size_t str_len,
                                           CssParserMode mode) {
    size_t len = pre_len + str_len + 1;
    char * source = malloc_wrapper(NULL, len);
    if ( source == NULL )
        return NULL;
    memcpy(source, prefix, pre_len);
    memcpy(source+pre_len, str, str_len);
    source[pre_len + str_len] = '\0';
    CssOutput * output = cssprsr_parse_with_options(&kCssDefaultOptions, (void*)source, len, mode);
    free_wrapper(NULL, source);
    return output;
}

static CssOutput* cssprsr_parse_with_options(const CssOptions* options,
                                               yyconst char* bytes,
                                               size_t len,
                                               CssParserMode mode) {
    assert(NULL != bytes);
    if ( NULL == bytes )
        return NULL;
    
    yyscan_t scanner;
    if (cssprsr_lex_init(&scanner)) {
        cssprsr_print("no scanning today!");
        return NULL;
    }
        
    cssprsr_scan_bytes(bytes, len, scanner);
    
    CssParser parser;
    parser.options = options;
    parser.scanner = &scanner;
    parser.default_namespace = cssAsteriskString;
    parser.parsed_declarations = cssprsr_new_array(&parser);
#if CSSPRSR_RPARSER_DEBUG
    parser.parsed_selectors = cssprsr_new_array(&parser);
#endif // #if CSSPRSR_RPARSER_DEBUG
    parser.position = cssprsr_parser_allocate(&parser, sizeof(CssSourcePosition));
    output_init(&parser, mode);
    cssparse(scanner, &parser);
    cssprsr_lex_destroy(scanner);
    if ( CssParserModeDeclarationList != mode ) {
        cssprsr_parser_clear_declarations(&parser);
    }
    cssprsr_parser_deallocate(&parser, parser.position);
#if CSSPRSR_RPARSER_DEBUG
    cssprsr_destroy_array(&parser, cssprsr_destroy_selector, parser.parsed_selectors);
    cssprsr_parser_deallocate(&parser, parser.parsed_selectors);
#endif // #if CSSPRSR_RPARSER_DEBUG
    parser.scanner = NULL;
    CssOutput* output = parser.output;
    return output;
}


void cssprsr_parse_internal_rule(CssParser* parser, CssRule* e)
{
    parser->output->rule = e;
}

void cssprsr_parse_internal_keyframe_rule(CssParser* parser, CssKeyframe* e)
{
    parser->output->keyframe = e;
}

void cssprsr_parse_internal_keyframe_key_list(CssParser* parser, CssArray* e)
{
    parser->output->keyframe_keys = e;
}

void cssprsr_parse_internal_value(CssParser* parser, CssArray* e)
{
    parser->output->values = e;
}

void cssprsr_parse_internal_media_list(CssParser* parser, CssArray* e)
{
    parser->output->medias = e;
}

void cssprsr_parse_internal_declaration_list(CssParser* parser, bool e)
{
    parser->output->declarations = parser->parsed_declarations;
}

void cssprsr_parse_internal_selector(CssParser* parser, CssArray* e)
{
    parser->output->selectors = e;
}


CssArray* cssprsr_new_array(CssParser* parser) {
    CssArray* array = cssprsr_parser_allocate(parser, sizeof(CssArray));
    cssprsr_array_init(parser, 0, array);
    return array;
}

void cssprsr_destroy_array_using_deallocator(CssParser* parser,
                          CssArrayDeallocator callback, CssArray* array) {
    //assert(NULL != array);
    if ( NULL == array )
        return;
    for (size_t i = 0; i < array->length; ++i) {
        callback(parser, array->data[i]);
    }
    cssprsr_array_destroy(parser, array);
}


CssStylesheet* cssprsr_new_stylesheet(CssParser* parser) {
    CssStylesheet* stylesheet =
        cssprsr_parser_allocate(parser, sizeof(CssStylesheet));
    stylesheet->encoding = NULL;
    cssprsr_array_init(parser, 0, &stylesheet->rules);
    cssprsr_array_init(parser, 0, &stylesheet->imports);
    return stylesheet;
}

void cssprsr_destroy_stylesheet(CssParser* parser, CssStylesheet* e)
{
    assert(NULL != e);
    if ( NULL == e )
        return;
    
    // free encoding
    if ( e->encoding )
        cssprsr_parser_deallocate(parser, (void*) e->encoding);

    // free imports
    for (size_t i = 0; i < e->imports.length; ++i) {
        cssprsr_destroy_import_rule(parser, e->imports.data[i]);
    }
    cssprsr_parser_deallocate(parser, (void*) e->imports.data);

    // free rules
    for (size_t i = 0; i < e->rules.length; ++i) {
        cssprsr_destroy_rule(parser, e->rules.data[i]);
    }
    cssprsr_parser_deallocate(parser, (void*) e->rules.data);

    // free e
    cssprsr_parser_deallocate(parser, (void*) e);
}

void cssprsr_destroy_rule(CssParser* parser, CssRule* rule)
{
    switch (rule->type) {
        case CssRuleStyle:
            cssprsr_destroy_style_rule(parser, (CssStyleRule*)rule);
            break;
        case CssRuleImport:
            cssprsr_destroy_import_rule(parser, (CssImportRule*)rule);
            break;
        case CssRuleFontFace:
            cssprsr_destroy_font_face_rule(parser, (CssFontFaceRule*)rule);
            break;
        case CssRuleKeyframes:
            cssprsr_destroy_keyframes_rule(parser, (CssKeyframesRule*)rule);
            break;
        case CssRuleMedia:
            cssprsr_destroy_media_rule(parser, (CssMediaRule*)rule);
            break;
            
        default:
            break;
    }
}

void cssprsr_destroy_rule_list(CssParser* parser, CssArray* rules)
{
    cssprsr_destroy_array(parser, cssprsr_destroy_rule, rules);
    cssprsr_parser_deallocate(parser, (void*) rules);
}


CssRule* cssprsr_new_style_rule(CssParser* parser, CssArray* selectors)
{
    assert(NULL != selectors);
    if ( NULL == selectors )
        return NULL;
    
    CssStyleRule* rule = cssprsr_parser_allocate(parser, sizeof(CssStyleRule));
    rule->base.name = "style";
    rule->base.type = CssRuleStyle;
    rule->selectors = selectors;
    // Do not check parser->parsed_declarations, when we encounter something like `selectors {}`, treat it as valid.
    rule->declarations = parser->parsed_declarations;
    cssprsr_parser_reset_declarations(parser);
    
    return (CssRule*)rule;
}

void cssprsr_destroy_style_rule(CssParser* parser, CssStyleRule* e)
{
    assert(e->selectors->length);

    cssprsr_destroy_array(parser, cssprsr_destroy_selector, e->selectors);
    cssprsr_parser_deallocate(parser, (void*) e->selectors);

    cssprsr_destroy_array(parser, cssprsr_destroy_declaration, e->declarations);
    cssprsr_parser_deallocate(parser, (void*) e->declarations);
    
    // cssprsr_parser_deallocate(parser, (void*) e->base.name);
    cssprsr_parser_deallocate(parser, (void*) e);
}


void cssprsr_add_namespace(CssParser* parser, CssParserString* prefix, CssParserString* uri)
{
    // TODO: No need for right now
}


CssRule* cssprsr_new_font_face(CssParser* parser)
{
    CssFontFaceRule* rule = cssprsr_parser_allocate(parser, sizeof(CssFontFaceRule));
    rule->base.name = "font-face";
    rule->base.type = CssRuleFontFace;
    rule->declarations = parser->parsed_declarations;

    cssprsr_parser_reset_declarations(parser);
    
    return (CssRule*)rule;
}

void cssprsr_destroy_font_face_rule(CssParser* parser, CssFontFaceRule* e)
{
    cssprsr_destroy_array(parser, cssprsr_destroy_declaration, e->declarations);
    cssprsr_parser_deallocate(parser, (void*) e->declarations);
    // cssprsr_parser_deallocate(parser, (void*) e->base.name);
    cssprsr_parser_deallocate(parser, (void*) e);
}


CssRule* cssprsr_new_keyframes_rule(CssParser* parser, CssParserString* name, CssArray* keyframes, bool isPrefixed)
{
    CssKeyframesRule * rule = cssprsr_parser_allocate(parser, sizeof(CssKeyframesRule));
    rule->base.name = "keyframes";
    rule->base.type = CssRuleKeyframes;
    rule->name = cssprsr_string_to_characters(parser, name);
    rule->keyframes = keyframes;
    return (CssRule*)rule;
}

void cssprsr_destroy_keyframes_rule(CssParser* parser, CssKeyframesRule * e)
{
    cssprsr_parser_clear_keyframes(parser, e->keyframes);
    cssprsr_parser_deallocate(parser, (void*) e->name);
    cssprsr_parser_deallocate(parser, (void*) e);
}

CssKeyframe* cssprsr_new_keyframe(CssParser* parser, CssArray* selectors)
{
    CssKeyframe* keyframe = cssprsr_parser_allocate(parser, sizeof(CssKeyframe));
    keyframe->selectors = selectors;
    keyframe->declarations = parser->parsed_declarations;
    cssprsr_parser_reset_declarations(parser);
    return keyframe;
}

void cssprsr_destroy_keyframe(CssParser* parser, CssKeyframe* e)
{
    cssprsr_destroy_array(parser, cssprsr_destroy_value, e->selectors);
    cssprsr_parser_deallocate(parser, (void*) e->selectors);
    
    cssprsr_destroy_array(parser, cssprsr_destroy_declaration, e->declarations);
    cssprsr_parser_deallocate(parser, (void*) e->declarations);

    cssprsr_parser_deallocate(parser, (void*) e);
}

CssArray* cssprsr_new_Keyframe_list(CssParser* parser)
{
    return cssprsr_new_array(parser);
}

void cssprsr_keyframe_rule_list_add(CssParser* parser, CssKeyframe* keyframe, CssArray* list)
{
    assert(keyframe);
    cssprsr_array_add(parser, keyframe, list);
}

void cssprsr_parser_clear_keyframes(CssParser* parser, CssArray* keyframes)
{
    cssprsr_destroy_array(parser, cssprsr_destroy_keyframe, keyframes);
    cssprsr_parser_deallocate(parser, (void*) keyframes);
}


void cssprsr_set_charset(CssParser* parser, CssParserString* charset)
{
//    parser->output->stylesheet->encoding = cssprsr_string_to_characters(parser, charset);
}


CssRule* cssprsr_new_import_rule(CssParser* parser, CssParserString* href, CssArray* media)
{
    CssImportRule* rule = cssprsr_parser_allocate(parser, sizeof(CssImportRule));
    rule->base.name = "import";
    rule->base.type = CssRuleImport;
    rule->href = cssprsr_string_to_characters(parser, href);
    rule->medias = media;
    return (CssRule*)rule;
}

void cssprsr_destroy_import_rule(CssParser* parser, CssImportRule* e)
{
    cssprsr_destroy_array(parser, cssprsr_destroy_media_query, e->medias);
    cssprsr_parser_deallocate(parser, (void*) e->medias);
    // cssprsr_parser_deallocate(parser, (void*) e->base.name);
    cssprsr_parser_deallocate(parser, (void*) e->href);
    cssprsr_parser_deallocate(parser, (void*) e);
}


CssValue* cssprsr_new_value(CssParser* parser)
{
    return cssprsr_parser_allocate(parser, sizeof(CssValue));
}

void cssprsr_destroy_value(CssParser* parser, CssValue* e)
{
    switch (e->unit) {
        case CSSPRSR_RVALUE_URI:
        case CSSPRSR_RVALUE_IDENT:
        case CSSPRSR_RVALUE_STRING:
        case CSSPRSR_RVALUE_DIMENSION:
        case CSSPRSR_RVALUE_UNICODE_RANGE:
        case CSSPRSR_RVALUE_PARSER_HEXCOLOR:
        {
            cssprsr_parser_deallocate(parser, (void*) e->string);
        }
            break;
        case CSSPRSR_RVALUE_PARSER_LIST:
        {
            cssprsr_destroy_array(parser, cssprsr_destroy_value, e->list);
            cssprsr_parser_deallocate(parser, (void*) e->list);
        }
            break;
        case CSSPRSR_RVALUE_PARSER_FUNCTION:
        {
            cssprsr_destroy_function(parser, e->function);
        }
            break;
        case CSSPRSR_RVALUE_NUMBER:
        case CSSPRSR_RVALUE_PERCENTAGE:
        case CSSPRSR_RVALUE_PX:
        case CSSPRSR_RVALUE_CM:
        case CSSPRSR_RVALUE_MM:
        case CSSPRSR_RVALUE_IN:
        case CSSPRSR_RVALUE_PT:
        case CSSPRSR_RVALUE_PC:
        case CSSPRSR_RVALUE_DEG:
        case CSSPRSR_RVALUE_RAD:
        case CSSPRSR_RVALUE_GRAD:
        case CSSPRSR_RVALUE_TURN:
        case CSSPRSR_RVALUE_MS:
        case CSSPRSR_RVALUE_S:
        case CSSPRSR_RVALUE_HZ:
        case CSSPRSR_RVALUE_KHZ:
        case CSSPRSR_RVALUE_EMS:
        case CSSPRSR_RVALUE_PARSER_Q_EMS:
        case CSSPRSR_RVALUE_EXS:
        case CSSPRSR_RVALUE_REMS:
        case CSSPRSR_RVALUE_CHS:
        case CSSPRSR_RVALUE_VW:
        case CSSPRSR_RVALUE_VH:
        case CSSPRSR_RVALUE_VMIN:
        case CSSPRSR_RVALUE_VMAX:
        case CSSPRSR_RVALUE_DPPX:
        case CSSPRSR_RVALUE_DPI:
        case CSSPRSR_RVALUE_DPCM:
        case CSSPRSR_RVALUE_FR:
            cssprsr_parser_deallocate(parser, (void*) e->raw);
            break;
        default:
            break;
    }
    
    cssprsr_parser_deallocate(parser, (void*) e);
}

CssValueFunction* cssprsr_new_function(CssParser* parser, CssParserString* name, CssArray* args)
{
    CssValueFunction* func = cssprsr_parser_allocate(parser, sizeof(CssValueFunction));
    func->name = cssprsr_string_to_characters(parser, name);
    func->args = args;
    return func;
}

void cssprsr_destroy_function(CssParser* parser, CssValueFunction* e)
{
    cssprsr_destroy_array(parser, cssprsr_destroy_value, e->args);
    cssprsr_parser_deallocate(parser, (void*) e->args);
    cssprsr_parser_deallocate(parser, (void*) e->name);
    cssprsr_parser_deallocate(parser, (void*) e);
}

CssValue* cssprsr_new_number_value(CssParser* parser, int sign, CssParserNumber* value, CssValueUnit unit)
{
    CssValue* v = cssprsr_new_value(parser);
    v->id = CssValueInvalid;
    v->isInt = false;
    v->fValue = sign * value->val;
    v->unit = unit;
    if ( 1 == sign ) {
        v->raw = cssprsr_string_to_characters(parser, &value->raw);
    } else {
        v->raw = cssprsr_string_to_characters_with_prefix_char(parser, &value->raw, '-');
    }
    return v;
}

CssValue* cssprsr_new_dimension_value(CssParser* parser, CssParserNumber* value, CssValueUnit unit)
{
    CssValue* v = cssprsr_new_value(parser);
    v->id = CssValueInvalid;
    v->isInt = false;
    v->fValue = value->val;
    v->raw = cssprsr_string_to_characters(parser, &value->raw);
    v->unit = unit;
    return v;
}

CssValue* cssprsr_new_operator_value(CssParser* parser, int value)
{
    CssValue* v = cssprsr_new_value(parser);
    v->id = CssValueInvalid;
    v->isInt = false;
    v->unit = CSSPRSR_RVALUE_PARSER_OPERATOR;
    v->iValue = value;
    return v;
}

CssValue* cssprsr_new_ident_value(CssParser* parser, CssParserString* value)
{
    CssValue* v = cssprsr_new_value(parser);
    // is it necessary to do this ?
    // v.id = cssValueKeywordID(string);
    v->id = CssValueCustom;
    v->isInt = false;
    v->unit = CSSPRSR_RVALUE_IDENT;
    v->string = cssprsr_string_to_characters(parser, value);
    return v;
}

CssValue* cssprsr_new_function_value(CssParser* parser, CssParserString* name, CssArray* args)
{
    CssValueFunction* func = cssprsr_new_function(parser, name, args);
    CssValue* value = cssprsr_new_value(parser);
    value->unit = CSSPRSR_RVALUE_PARSER_FUNCTION;
    value->function = func;
    return value;
}

CssValue* cssprsr_new_list_value(CssParser* parser, CssArray* list)
{
    CssValue* value = cssprsr_new_value(parser);
    value->unit = CSSPRSR_RVALUE_PARSER_LIST;
    value->list = list;
    return value;
}

void cssprsr_value_set_string(CssParser* parser, CssValue* value, CssParserString* string)
{
    value->string = cssprsr_string_to_characters(parser, string);
}

void cssprsr_value_set_sign(CssParser* parser, CssValue* value, int sign)
{
    value->fValue *= sign;
    
    if ( sign < 0 ) {
        const char* raw = value->raw;
        size_t len = strlen(raw);
        char* new_str = cssprsr_parser_allocate(parser, sizeof(char) * (len + 2));
        strcpy(new_str + 1, raw);
        new_str[0] = '-';
        new_str[len + 1] = '\0';
        value->raw = new_str;
        cssprsr_parser_deallocate(parser, (void*) raw);
    }
}


CssArray* cssprsr_new_value_list(CssParser* parser)
{
    return cssprsr_new_array(parser);
}

void cssprsr_value_list_insert(CssParser* parser, CssValue* value, int index, CssArray* list)
{
    assert(NULL != value);
    if ( value == NULL)
        return;
    cssprsr_array_insert_at(parser, value, index, list);
}

void cssprsr_value_list_add(CssParser* parser, CssValue* value, CssArray* list)
{
    assert(NULL != value);
    if ( value == NULL)
        return;
    cssprsr_array_add(parser, value, list);
}

void cssprsr_value_list_steal_values(CssParser* parser, CssArray* values, CssArray* list)
{
    assert(NULL != values && values->length);
    if ( values == NULL || 0 == values->length )
        return;
    for (size_t i = 0; i < values->length; ++i)
        cssprsr_value_list_add(parser, values->data[i], list);
    cssprsr_parser_deallocate(parser, (void*) values);
}


bool cssprsr_new_declaration(CssParser* parser, CssParserString* name, bool important, CssArray* values)
{
    CssDeclaration * decl = cssprsr_parser_allocate(parser, sizeof(CssDeclaration));
    decl->property = cssprsr_string_to_characters(parser, name);
    decl->important = important;
    decl->values = values;
    decl->raw = cssprsr_stringify_value_list(parser, values);
    cssprsr_array_add(parser, decl, parser->parsed_declarations);
    
    return true;
}

void cssprsr_destroy_declaration(CssParser* parser, CssDeclaration* e)
{
    cssprsr_destroy_array(parser, cssprsr_destroy_value, e->values);
    cssprsr_parser_deallocate(parser, (void*) e->values);
    cssprsr_parser_deallocate(parser, (void*) e->raw);
    cssprsr_parser_deallocate(parser, (void*) e->property);
    cssprsr_parser_deallocate(parser, (void*) e);
}

void cssprsr_parser_clear_declarations(CssParser* parser)
{
    cssprsr_destroy_array(parser, cssprsr_destroy_declaration, parser->parsed_declarations);
    cssprsr_parser_deallocate(parser, (void*) parser->parsed_declarations);
    parser->parsed_declarations = NULL;
}

void cssprsr_parser_reset_declarations(CssParser* parser)
{
    parser->parsed_declarations = cssprsr_new_array(parser);
}


CssRule* cssprsr_new_media_rule(CssParser* parser, CssArray* medias, CssArray* rules)
{
//	assert(NULL != medias && NULL != rules);
    
    if ( medias == NULL || rules == NULL )
        return NULL;
    
    CssMediaRule* rule = cssprsr_parser_allocate(parser, sizeof(CssMediaRule));
    rule->base.name = "media";
    rule->base.type = CssRuleMedia;
    rule->medias = medias;
    rule->rules = rules;
    return (CssRule*)rule;
}

void cssprsr_destroy_media_rule(CssParser* parser, CssMediaRule* e)
{
    cssprsr_destroy_media_list(parser, (void*) e->medias);
    cssprsr_destroy_rule_list(parser,  (void*) e->rules),
    // cssprsr_parser_deallocate(parser,  (void*) e->base.name);
    cssprsr_parser_deallocate(parser,  (void*) e);
}


CssArray* cssprsr_new_media_list(CssParser* parser)
{
    return cssprsr_new_array(parser);
}

void cssprsr_media_list_add(CssParser* parser, CssMediaQuery* media_query, CssArray* medias)
{
    // debug here
//    cssprsr_print_media_query(parser, media_query);
    if ( NULL != media_query ) {
        cssprsr_array_add(parser, media_query, medias);
    }
}

void cssprsr_destroy_media_list(CssParser* parser, CssArray* medias)
{
    cssprsr_destroy_array(parser, cssprsr_destroy_media_query, medias);
    cssprsr_parser_deallocate(parser, (void*) medias);
}


CssMediaQuery* cssprsr_new_media_query(CssParser* parser, CssMediaQueryRestrictor r, CssParserString *type, CssArray* exps)
{
    CssMediaQuery* media_query = cssprsr_parser_allocate(parser, sizeof(CssMediaQuery));
    media_query->restrictor = r;
    media_query->type = type == NULL ? NULL : cssprsr_string_to_characters(parser, type);
    media_query->expressions = exps;
    return media_query;
}

void cssprsr_destroy_media_query(CssParser* parser, CssMediaQuery* e)
{
    cssprsr_destroy_array(parser, cssprsr_destroy_media_query_exp, e->expressions);
    cssprsr_parser_deallocate(parser, (void*) e->expressions);
    if ( NULL != e->type ) {
        cssprsr_parser_deallocate(parser, (void*) e->type);
    }
    cssprsr_parser_deallocate(parser, (void*) e);
}


CssMediaQueryExp * cssprsr_new_media_query_exp(CssParser* parser, CssParserString* feature, CssArray* values)
{
    assert( NULL != feature );
    if ( NULL == feature )
        return NULL;
    
    CssMediaQueryExp* exp = cssprsr_parser_allocate(parser, sizeof(CssMediaQueryExp));
    exp->feature = cssprsr_string_to_characters(parser, feature);
    exp->values = values;
    exp->raw = cssprsr_stringify_value_list(parser, values);
    return exp;
}

void cssprsr_destroy_media_query_exp(CssParser* parser, CssMediaQueryExp* e)
{
    if ( NULL != e->values ) {
        cssprsr_destroy_array(parser, cssprsr_destroy_value, e->values);
        cssprsr_parser_deallocate(parser, e->values);
    }
    cssprsr_parser_deallocate(parser, (void*) e->raw);
    cssprsr_parser_deallocate(parser, (void*) e->feature);
    cssprsr_parser_deallocate(parser, (void*) e);
}


void cssprsr_media_query_exp_list_add(CssParser* parser, CssMediaQueryExp* exp, CssArray* list)
{
    assert(NULL != exp);
    if ( NULL == exp )
        return;
    cssprsr_array_add(parser, exp, list);
}

CssArray* cssprsr_new_media_query_exp_list(CssParser* parser)
{
    return cssprsr_new_array(parser);
}


CssArray* cssprsr_new_rule_list(CssParser* parser)
{
    return cssprsr_new_array(parser);
}

CssArray* cssprsr_rule_list_add(CssParser* parser, CssRule* rule, CssArray* rule_list)
{
    if ( NULL != rule ) {
        if ( NULL == rule_list )
            rule_list = cssprsr_new_rule_list(parser);
        cssprsr_array_add(parser, rule, rule_list);
    }
    
    return rule_list;
}


void cssprsr_start_declaration(CssParser* parser)
{
    cssprsr_parser_log(parser, "cssprsr_start_declaration");   
}

void cssprsr_end_declaration(CssParser* parser, bool flag, bool ended)
{
    cssprsr_parser_log(parser, "cssprsr_end_declaration");
}

void cssprsr_set_current_declaration(CssParser* parser, CssParserString* tag)
{
    cssprsr_parser_log(parser, "cssprsr_set_current_declaration");
}


void cssprsr_start_selector(CssParser* parser)
{
    cssprsr_parser_log(parser, "cssprsr_start_selector");
}

void cssprsr_end_selector(CssParser* parser)
{
    cssprsr_parser_log(parser, "cssprsr_end_selector");
}

CssQualifiedName * cssprsr_new_qualified_name(CssParser* parser, CssParserString* prefix, CssParserString* local, CssParserString* uri)
{
    CssQualifiedName* name = cssprsr_parser_allocate(parser, sizeof(CssQualifiedName));
    name->prefix = prefix == NULL ? NULL : cssprsr_string_to_characters(parser, prefix);
    name->local = local == NULL ? NULL : cssprsr_string_to_characters(parser, local);
    name->uri = uri == NULL ? NULL : cssprsr_string_to_characters(parser, uri);
    return name;
}

void cssprsr_destroy_qualified_name(CssParser* parser,  CssQualifiedName* e)
{
    cssprsr_parser_deallocate(parser, (void*) e->local);
    cssprsr_parser_deallocate(parser, (void*) e->prefix);
    cssprsr_parser_deallocate(parser, (void*) e->uri);
    cssprsr_parser_deallocate(parser, (void*) e);
}

CssSelectorRareData* cssprsr_new_rare_data(CssParser* parser)
{
    CssSelectorRareData* data = cssprsr_parser_allocate(parser, sizeof(CssSelectorRareData));
    data->value = NULL;
    data->attribute = NULL;
    data->argument = NULL;
    data->selectors = NULL;
    return data;
}

void cssprsr_destroy_rare_data(CssParser* parser, CssSelectorRareData* e)
{
    if ( NULL != e->value )
        cssprsr_parser_deallocate(parser, (void*) e->value);
    
    if ( NULL != e->argument )
        cssprsr_parser_deallocate(parser, (void*) e->argument);
    
    if ( NULL != e->attribute )
        cssprsr_destroy_qualified_name(parser, e->attribute);

    if ( NULL != e->selectors ) {
        cssprsr_destroy_array(parser, cssprsr_destroy_selector, e->selectors);
        cssprsr_parser_deallocate(parser, (void*) e->selectors);
    }
    
    cssprsr_parser_deallocate(parser, e);
}

CssSelector* cssprsr_new_selector(CssParser* parser)
{
    CssSelector* selector = cssprsr_parser_allocate(parser, sizeof(CssSelector));
    selector->data = cssprsr_new_rare_data(parser);
    selector->tag = NULL;
    selector->match = 0;
    selector->pseudo = CssPseudoNotParsed;
    selector->relation = 0;
    selector->specificity = 0;
    selector->tag = NULL;
    selector->tagHistory = NULL;
#if CSSPRSR_RPARSER_DEBUG
    cssprsr_array_add(parser, selector, parser->parsed_selectors);
#endif // #if CSSPRSR_RPARSER_DEBUG
    return selector;
}

CssSelector* cssprsr_sink_floating_selector(CssParser* parser, CssSelector* selector)
{
#if CSSPRSR_RPARSER_DEBUG
    cssprsr_array_remove(parser, selector, parser->parsed_selectors);
#endif // #if CSSPRSR_RPARSER_DEBUG
    return selector;
}

void cssprsr_destroy_one_selector(CssParser* parser, CssSelector* e)
{
    cssprsr_destroy_rare_data(parser, e->data);
    
    if ( e->tag  != NULL )
        cssprsr_destroy_qualified_name(parser, e->tag);
    
    cssprsr_parser_deallocate(parser, e);
}

void cssprsr_destroy_selector(CssParser* parser, CssSelector* e)
{
    CssSelector *p = e, *q;
    while ( p ) {
        q = p->tagHistory;
        cssprsr_destroy_one_selector(parser, p);
        p = q;
    }
}

CssSelector* cssprsr_rewrite_specifier_with_element_name(CssParser* parser, CssParserString* tag, CssSelector* specifier)
{
    // TODO: (@QFish) check if css3 support
    bool supported = true;
    
    if ( supported ) {
        CssSelector* prepend = cssprsr_new_selector(parser);
        prepend->tag = cssprsr_new_qualified_name(parser, NULL, tag, &parser->default_namespace);
        prepend->match = CssSelectorMatchTag;
        prepend->tagHistory = cssprsr_sink_floating_selector(parser, specifier);
        prepend->relation = CssSelectorRelationSubSelector;
        return prepend;
    }
    
    return specifier;
}

CssSelector* cssprsr_rewrite_specifier_with_namespace_if_needed(CssParser* parser, CssSelector* specifiers)
{
    // TODO: @(QFish) add logic
    return specifiers;
}

CssSelector* cssprsr_rewrite_specifiers(CssParser* parser, CssSelector* specifiers, CssSelector* newSpecifier)
{
    if (cssprsr_selector_crosses_tree_scopes(newSpecifier)) {
        // Unknown pseudo element always goes at the top of selector chain.
        cssprsr_selector_append(parser, newSpecifier, cssprsr_sink_floating_selector(parser, specifiers), CssSelectorRelationShadowPseudo);
        return newSpecifier;
    }
    if (cssprsr_selector_is_content_pseudo_element(newSpecifier)) {
        cssprsr_selector_append(parser, newSpecifier, cssprsr_sink_floating_selector(parser, specifiers), CssSelectorRelationSubSelector);
        return newSpecifier;
    }
    if (cssprsr_selector_crosses_tree_scopes(specifiers)) {
        // Specifiers for unknown pseudo element go right behind it in the chain.
        cssprsr_selector_insert(parser, specifiers, cssprsr_sink_floating_selector(parser, newSpecifier), CssSelectorRelationSubSelector, CssSelectorRelationShadowPseudo);
        return specifiers;
    }
    if (cssprsr_selector_is_content_pseudo_element(specifiers)) {
        cssprsr_selector_insert(parser, specifiers, cssprsr_sink_floating_selector(parser, newSpecifier), CssSelectorRelationSubSelector, CssSelectorRelationSubSelector);
        return specifiers;
    }

    cssprsr_selector_append(parser, specifiers, cssprsr_sink_floating_selector(parser, newSpecifier), CssSelectorRelationSubSelector);
    return specifiers;
}

void cssprsr_adopt_selector_list(CssParser* parser, CssArray* selectors, CssSelector* selector)
{
    cssprsr_parser_log(parser, "cssprsr_adopt_selector_list");
    selector->data->selectors = selectors;
}

void cssprsr_selector_append(CssParser* parser, CssSelector* selector, CssSelector* new_selector, CssSelectorRelation relation)
{
    cssprsr_parser_log(parser, "cssprsr_selector_append");
    CssSelector* end = selector;
    while (NULL != end->tagHistory)
        end = end->tagHistory;
    end->relation = relation;
    end->tagHistory = new_selector;
}

void cssprsr_selector_insert(CssParser* parser, CssSelector* selector, CssSelector* new_selector, CssSelectorRelation before, CssSelectorRelation after)
{
    cssprsr_parser_log(parser, "cssprsr_selector_insert");

    if (selector->tagHistory)
        new_selector->tagHistory = selector;
    selector->relation = before;
    new_selector->relation = after;
    selector->tagHistory = selector;
}

void cssprsr_selector_prepend_with_element_name(CssParser* parser, CssSelector* selector, CssParserString* tag)
{
    cssprsr_parser_log(parser, "cssprsr_selector_prepend_with_element_name");

    CssSelector* prev = cssprsr_new_selector(parser);
    prev->tag = cssprsr_new_qualified_name(parser, NULL, tag, &parser->default_namespace);
    selector->tagHistory = prev;
    selector->relation = CssSelectorRelationSubSelector;
}

CssArray* cssprsr_new_selector_list(CssParser* parser)
{
    return cssprsr_new_array(parser);
}

CssArray* cssprsr_reusable_selector_list(CssParser* parser)
{
    return cssprsr_new_array(parser);
}

void cssprsr_selector_list_shink(CssParser* parser, int capacity, CssArray* list)
{

}

void cssprsr_selector_list_add(CssParser* parser, CssSelector* selector, CssArray* list)
{
    assert(NULL != selector);
    if ( NULL == selector )
        return;
        
    cssprsr_array_add(parser, selector, list);
}

void cssprsr_selector_set_value(CssParser* parser, CssSelector* selector, CssParserString* value)
{
    selector->data->value = cssprsr_string_to_characters(parser, value);
}

void cssprsr_selector_set_argument_with_number(CssParser* parser, CssSelector* selector, int sign, CssParserNumber* value)
{
    if ( 1 == sign ) {
        selector->data->argument = cssprsr_string_to_characters(parser, &value->raw);
    } else {
        selector->data->argument = cssprsr_string_to_characters_with_prefix_char(parser, &value->raw, '-');
    }
}

void cssprsr_selector_set_argument(CssParser* parser, CssSelector* selector, CssParserString* argument)
{
    selector->data->argument = cssprsr_string_to_characters(parser, argument);
}

bool cssprsr_parse_attribute_match_type(CssParser* parser, CssAttributeMatchType type, CssParserString* attr)
{
    return true;
}

bool cssprsr_selector_is_simple(CssParser* parser, CssSelector* selector)
{
    if (NULL != selector->data->selectors)
        return false;
    
    if (NULL == selector->tagHistory)
        return true;
    // TODO: @(QFish) check more.
    return false;
}


void cssprsr_add_rule(CssParser* parser, CssRule* rule)
{
    assert( NULL != rule );
    if ( NULL == rule )
        return;
    
    switch ( rule->type ) {
        case CssRuleImport:
            cssprsr_array_add(parser, rule, &parser->output->stylesheet->imports);
            break;
        default:
            cssprsr_array_add(parser, rule, &parser->output->stylesheet->rules);
            break;
    }
}

void cssprsr_start_rule(CssParser* parser)
{
    cssprsr_parser_log(parser, "cssprsr_start_rule");
}

void cssprsr_end_rule(CssParser* parser, bool ended)
{
    cssprsr_parser_log(parser, "cssprsr_end_rule");
}

void cssprsr_start_rule_header(CssParser* parser, CssRuleType type)
{
    cssprsr_parser_log(parser, "cssprsr_start_rule_header");
}

void cssprsr_end_rule_header(CssParser* parser)
{
    cssprsr_parser_log(parser, "cssprsr_end_rule_header");
}

void cssprsr_end_invalid_rule_header(CssParser* parser)
{
    cssprsr_parser_log(parser, "cssprsr_end_invalid_rule_header");
}

void cssprsr_start_rule_body(CssParser* parser)
{
    cssprsr_parser_log(parser, "cssprsr_start_rule_body");
}


bool cssprsr_string_is_function(CssParserString* string)
{
    return string && (string->length > 0) && (string->data[string->length - 1] == '(');
}

void cssprsr_string_clear(CssParser* parser, CssParserString* string)
{
	printf("==%s==\n", string->data);
    cssprsr_parser_deallocate(parser, (void*) string->data);
    cssprsr_parser_deallocate(parser, (void*) string);
}


void cssprsr_error(YYLTYPE* yyloc, void* scanner, struct CssInternalParser * parser, char* error)
{
#ifdef CSSPRSR_RPARSER_DEBUG
#if CSSPRSR_RPARSER_DEBUG
    cssprsr_print("[Error] %d.%d - %d.%d: %s at %s\n",
           yyloc->first_line,
           yyloc->first_column,
           yyloc->last_line,
           yyloc->last_column,
           error,
           cssprsr_get_text(parser->scanner));

    YYSTYPE * s = cssprsr_get_lval(parser->scanner);

//	struct yy_buffer_state state = cssprsr_get_previous_state(parser->scanner);
//    s, (*yy_buffer_stack[0]).yy_ch_buf);
//    
//    cssprsr_print("%s", s->);
#endif // #if CSSPRSR_RPARSER_DEBUG
#endif // #ifdef CSSPRSR_RPARSER_DEBUG

    CssError *e = (CssError *)malloc(sizeof(CssError));
    e->type = CssParseError;
    e->first_line = yyloc->first_line;
    e->first_column = yyloc->first_column;
    e->last_line = yyloc->last_line;
    e->last_column = yyloc->last_column;
    snprintf(e->message, CSS_ERROR_MSG_SIZE, "%s at %s", error,
             cssprsr_get_text(parser->scanner));
    cssprsr_array_add(parser, e, &(parser->output->errors));
}

void cssprsr_parser_log(CssParser* parser, const char * format, ...)
{
#ifdef CSSPRSR_RPARSER_LOG_ENABLE
#if CSSPRSR_RPARSER_LOG_ENABLE
    va_list args;
    va_start(args, format);
    printf(" -> ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
    fflush(stdout);
#endif // #if CSSPRSR_RPARSER_LOG_ENABLE
#endif // #ifdef CSSPRSR_RPARSER_LOG_ENABLE
}

void cssprsr_parser_resume_error_logging()
{
    
}

void cssprsr_parser_report_error(CssParser* parser, CssSourcePosition* pos, const char* format, ...)
{
#ifdef CSSPRSR_RPARSER_DEBUG
#if CSSPRSR_RPARSER_DEBUG
    printf("[ERROR] %d.%d - %d.%d : ", pos->line, pos->column, cssprsr_get_lineno(*parser->scanner), cssprsr_get_column(*parser->scanner) );
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
#endif // #if CSSPRSR_RPARSER_DEBUG
#endif // #ifdef CSSPRSR_RPARSER_DEBUG
}


void cssprsr_print_position(YYLTYPE* yyloc)
{
    cssprsr_print(NULL,
                 "Loaction %d.%d - %d.%d",
                 yyloc->first_line,
                 yyloc->first_column,
                 yyloc->last_line,
                 yyloc->last_column
                 );
}

CssSourcePosition* cssprsr_parser_current_location(CssParser* parser, YYLTYPE* yylloc)
{
    parser->position->line = cssprsr_get_lineno(*parser->scanner);
    parser->position->column = cssprsr_get_column(*parser->scanner);
    //    cssprsr_print_position(yylloc);
    return parser->position;
}


void cssprsr_print(const char * format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
	printf("\n");
    va_end(args);
    fflush(stdout);
}

void cssprsr_print_stylesheet(CssParser* parser, CssStylesheet* sheet)
{
    cssprsr_print("stylesheet with ");
    cssprsr_print("%d rules.\n", sheet->rules.length);
    for (size_t i = 0; i < sheet->imports.length; ++i) {
        cssprsr_print_rule(parser, sheet->imports.data[i]);
    }
    for (size_t i = 0; i < sheet->rules.length; ++i) {
        cssprsr_print_rule(parser, sheet->rules.data[i]);
    }
    cssprsr_print("\n");
}

void cssprsr_print_rule(CssParser* parser, CssRule* rule)
{
    if ( NULL == rule ) {
        breakpoint;
        return;
    }
    
    switch (rule->type) {
        case CssRuleStyle:
            cssprsr_print_style_rule(parser, (CssStyleRule*)rule);
            break;
        case CssRuleImport:
            cssprsr_print_import_rule(parser, (CssImportRule*)rule);
            break;
        case CssRuleFontFace:
            cssprsr_print_font_face_rule(parser, (CssFontFaceRule*)rule);
            break;
        case CssRuleKeyframes:
            cssprsr_print_keyframes_rule(parser, (CssKeyframesRule*)rule);
            break;
        case CssRuleMedia:
            cssprsr_print_media_rule(parser, (CssMediaRule*)rule);
            break;
        case CssRuleSupports:
            break;
        case CssRuleUnkown:
            break;
            
        default:
            break;
    }
}

void cssprsr_print_import_rule(CssParser* parser, CssImportRule* rule)
{
    cssprsr_print("@%s ", rule->base.name);
    cssprsr_print("url(%s)", rule->href);
    cssprsr_print(";\n");
}

void cssprsr_print_keyframes_rule(CssParser* parser, CssKeyframesRule* rule)
{
    cssprsr_print("@%s ", rule->base.name);
    cssprsr_print("%s {\n", rule->name);
    for (size_t i = 0; i < rule->keyframes->length; ++i) {
        cssprsr_print_keyframe(parser, rule->keyframes->data[i]);
    }
    cssprsr_print("}\n");
}

void cssprsr_print_keyframe(CssParser* parser, CssKeyframe* keyframe)
{
    assert( NULL != keyframe );
    if ( NULL == keyframe )
        return;
    
    for (size_t i = 0; i < keyframe->selectors->length; ++i) {
        CssValue* value = keyframe->selectors->data[i];
        if ( value->unit == CSSPRSR_RVALUE_NUMBER ) {
            cssprsr_print("%s", value->raw);
        }
        if ( i != keyframe->selectors->length -1 ) {
            cssprsr_print(", ");
        }
    }
    cssprsr_print(" {\n");
    cssprsr_print_declaration_list(parser, keyframe->declarations);
    cssprsr_print("}\n");
}

void cssprsr_print_media_query_exp(CssParser* parser, CssMediaQueryExp* exp)
{
    cssprsr_print("(");
    if (NULL != exp->feature) {
        cssprsr_print("%s", exp->feature);
    }
    if ( exp->values && exp->values->length ) {
        const char* str = cssprsr_stringify_value_list(parser, exp->values);
        cssprsr_print(": %s", str);
        cssprsr_parser_deallocate(parser, (void*) str);
    }
    cssprsr_print(")");
}

void cssprsr_print_media_query(CssParser* parser, CssMediaQuery* query)
{
    // For now ignored is always false
//    if ( !query->ignored ) {
        // print restrictor
        switch ( query->restrictor ) {
            case CssMediaQueryRestrictorOnly:
                cssprsr_print("only ");
                break;
            case CssMediaQueryRestrictorNot:
                cssprsr_print("not ");
                break;
            case CssMediaQueryRestrictorNone:
                break;
        }
        
        if ( NULL == query->expressions || 0 == query->expressions->length ) {
            if ( NULL != query->type ) {
                cssprsr_print("%s", query->type);
            }
            return;
        }
        
        if ( (NULL != query->type && strcasecmp(query->type, "all")) || query->restrictor != CssMediaQueryRestrictorNone) {
            if ( NULL != query->type ) {
                cssprsr_print("%s", query->type);
            }
            cssprsr_print(" and ");
        }
        
        cssprsr_print_media_query_exp(parser, query->expressions->data[0]);
        
        for (size_t i = 1; i < query->expressions->length; ++i) {
            cssprsr_print(" and ");
            cssprsr_print_media_query_exp(parser, query->expressions->data[i]);
        }
//    } else {
//        // If query is invalid, serialized text should turn into "not all".
//        cssprsr_print("not all");
//    }
}

void cssprsr_print_media_list(CssParser* parser, CssArray* medias)
{
    bool first = true;
    for (size_t i = 0; i < medias->length; ++i) {
        if (!first)
            cssprsr_print(", ");
        else
            first = false;
        cssprsr_print_media_query(parser, (CssMediaQuery*)medias->data[i]);
    }
}

void cssprsr_print_media_rule(CssParser* parser, CssMediaRule* rule)
{
    cssprsr_print("@%s ", rule->base.name);
    
    if ( rule->medias->length ) {
        cssprsr_print_media_list(parser, rule->medias);
    }
    
    if ( rule->medias->length ) {
        cssprsr_print(" {\n");
        for (size_t i = 0; i < rule->rules->length; ++i) {
            cssprsr_print_style_rule(parser, rule->rules->data[i]);
        }
        cssprsr_print("}\n");
    } else {
        cssprsr_print(";\n");
    }
}

void cssprsr_print_selector(CssParser* parser, CssSelector* selector)
{
    CssParserString * string = cssprsr_selector_to_string(parser, selector, NULL);
    const char* text = cssprsr_string_to_characters(parser, string);
    cssprsr_parser_deallocate(parser, (void*) string->data);
    cssprsr_parser_deallocate(parser, (void*) string);
    cssprsr_print("%s", text);
    cssprsr_parser_deallocate(parser, (void*) text);
}

void cssprsr_print_selector_list(CssParser* parser, CssArray* selectors)
{
    for (size_t i = 0; i < selectors->length; ++i) {
        cssprsr_print_selector(parser, selectors->data[i]);
        if ( i != selectors->length -1 ) {
            cssprsr_print(",\n");
        }
    }
}

void cssprsr_print_style_rule(CssParser* parser, CssStyleRule* rule)
{
    cssprsr_print_selector_list(parser, rule->selectors);
    cssprsr_print(" {\n");
    
    if ( rule->declarations->length ) {
        cssprsr_print_declaration_list(parser, rule->declarations);
    } else {
        cssprsr_print("  /*no rule*/\n");
    }
    
    cssprsr_print("}\n");
}

void cssprsr_print_declaration(CssParser* parser, CssDeclaration* decl)
{
    const char* str = cssprsr_stringify_value_list(parser, decl->values);
    cssprsr_print("%s: %s", decl->property, str);
    cssprsr_parser_deallocate(parser, (void*) str);
    if ( decl->important ) {
        cssprsr_print(" !important");
    }
}

void cssprsr_print_declaration_list(CssParser* parser, CssArray* declarations)
{
    for (size_t i = 0; i < declarations->length; ++i) {
        cssprsr_print("  ");
        cssprsr_print_declaration(parser, declarations->data[i]);
        cssprsr_print(";\n");
    }
}

void cssprsr_print_value_list(CssParser* parser, CssArray* values)
{
    const char* str = cssprsr_stringify_value_list(parser, values);
    cssprsr_print("%s", str);
    cssprsr_parser_deallocate(parser, (void*) str);
}

void cssprsr_print_font_face_rule(CssParser* parser, CssFontFaceRule* rule)
{
    cssprsr_print("@%s {", rule->base.name);
    cssprsr_print_declaration_list(parser, rule->declarations);
    cssprsr_print("}\n");
}

CssOutput* css_dump_output(CssOutput* output)
{
    if ( NULL == output )
        return output;
    
    CssParser parser;
    parser.options = &kCssDefaultOptions;

    switch (output->mode) {
        case CssParserModeStylesheet:
            cssprsr_print_stylesheet(&parser, output->stylesheet);
            break;
        case CssParserModeRule:
            cssprsr_print_rule(&parser, output->rule);
            break;
        case CssParserModeKeyframeRule:
            cssprsr_print_keyframe(&parser, output->keyframe);
            break;
        case CssParserModeKeyframeKeyList:
            cssprsr_print_value_list(&parser, output->keyframe_keys);
            break;
        case CssParserModeMediaList:
            cssprsr_print_media_list(&parser, output->medias);
            break;
        case CssParserModeValue:
            cssprsr_print_value_list(&parser, output->values);
            break;
        case CssParserModeSelector:
            cssprsr_print_selector_list(&parser, output->selectors);
            break;
        case CssParserModeDeclarationList:
            cssprsr_print_declaration_list(&parser, output->declarations);
            break;
    }
    cssprsr_print("\n");
    return output;
}


static const char* cssprsr_stringify_value_list(CssParser* parser, CssArray* values)
{
    if (NULL == values)
        return NULL;
    CssParserString * buffer = cssprsr_parser_allocate(parser, sizeof(CssParserString));
    cssprsr_string_init(parser, buffer);
    for (size_t i = 0; i < values->length; ++i) {
        CssValue* value = values->data[i];
        const char* value_str = cssprsr_stringify_value(parser, value);
        cssprsr_string_append_characters(parser, value_str, buffer);
        cssprsr_parser_deallocate(parser, (void*) value_str);
        value_str = NULL;
        if ( i < values->length - 1 ) {
            if ( value->unit != CSSPRSR_RVALUE_PARSER_OPERATOR ) {
                if ( i < values->length - 2 ) {
                    value = values->data[i+1];
                    if ( value->unit != CSSPRSR_RVALUE_PARSER_OPERATOR ) {
                        cssprsr_string_append_characters(parser, " ", buffer);
                    }
                } else {
                    cssprsr_string_append_characters(parser, " ", buffer);
                }
            }
        }
    }
    const char * str = cssprsr_string_to_characters(parser, (CssParserString*)buffer);
    cssprsr_parser_deallocate(parser, buffer->data);
    cssprsr_parser_deallocate(parser, (void*) buffer);
    return str;
}

static const char* cssprsr_stringify_value(CssParser* parser, CssValue* value)
{
    // TODO: @(QFish) Handle this more gracefully X).
    char str[256];
    
    switch (value->unit) {
        case CSSPRSR_RVALUE_NUMBER:
        case CSSPRSR_RVALUE_PERCENTAGE:
        case CSSPRSR_RVALUE_EMS:
        case CSSPRSR_RVALUE_EXS:
        case CSSPRSR_RVALUE_REMS:
        case CSSPRSR_RVALUE_CHS:
        case CSSPRSR_RVALUE_PX:
        case CSSPRSR_RVALUE_CM:
        case CSSPRSR_RVALUE_DPPX:
        case CSSPRSR_RVALUE_DPI:
        case CSSPRSR_RVALUE_DPCM:
        case CSSPRSR_RVALUE_MM:
        case CSSPRSR_RVALUE_IN:
        case CSSPRSR_RVALUE_PT:
        case CSSPRSR_RVALUE_PC:
        case CSSPRSR_RVALUE_DEG:
        case CSSPRSR_RVALUE_RAD:
        case CSSPRSR_RVALUE_GRAD:
        case CSSPRSR_RVALUE_MS:
        case CSSPRSR_RVALUE_S:
        case CSSPRSR_RVALUE_HZ:
        case CSSPRSR_RVALUE_KHZ:
        case CSSPRSR_RVALUE_TURN:
            snprintf(str, sizeof(str), "%s", value->raw);
            break;
        case CSSPRSR_RVALUE_IDENT:
            snprintf(str, sizeof(str), "%s", value->string);
            break;
        case CSSPRSR_RVALUE_STRING:
            // FIXME: @(QFish) Do we need double quote or not ?
//            snprintf(str, sizeof(str), "\"%s\"", value->string);
            snprintf(str, sizeof(str), "%s", value->string);
            break;
        case CSSPRSR_RVALUE_PARSER_FUNCTION:
        {
            const char* args_str = cssprsr_stringify_value_list(parser, value->function->args);
            snprintf(str, sizeof(str), "%s%s)", value->function->name, args_str);
            cssprsr_parser_deallocate(parser, (void*) args_str);
            break;
        }
        case CSSPRSR_RVALUE_PARSER_OPERATOR:
            if (value->iValue != '=') {
                snprintf(str, sizeof(str), " %c ", value->iValue);
            } else {
                snprintf(str, sizeof(str), " %c", value->iValue);
            }
            break;
        case CSSPRSR_RVALUE_PARSER_LIST:
            return cssprsr_stringify_value_list(parser, value->list);
            break;
        case CSSPRSR_RVALUE_PARSER_HEXCOLOR:
            snprintf(str, sizeof(str), "#%s", value->string);
            break;
        case CSSPRSR_RVALUE_URI:
            snprintf(str, sizeof(str), "url(%s)", value->string);
            break;
        default:
            cssprsr_print("CSSPARSER: Unknown Value unit.");
            break;
    }

    size_t len = strlen(str);
    char* dest = cssprsr_parser_allocate(parser, len+1);
    strcpy(dest, str);
    dest[len] = '\0';
    return dest;
}
