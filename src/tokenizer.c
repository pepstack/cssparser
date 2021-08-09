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
#include "cssparser_i.h"

#undef	assert
#define assert(x)

static inline double cssprsr_characters_to_double(const char* data, size_t length, bool* ok);
static inline bool cssprsr_is_html_space(char c);
static inline char* cssprsr_normalize_text(yy_size_t* length, char *origin_text, yy_size_t origin_length, int tok);

#ifdef CSSPRSR_RFELX_DEBUG
#if CSSPRSR_RFELX_DEBUG
static char * cssprsr_token_string(int tok);
#endif // #if CSSPRSR_RFELX_DEBUG
#endif // #ifdef CSSPRSR_RFELX_DEBUG

/**
 *  A hook function of flex, processing tokens which will be passed to bison
 *
 *  @param yylval    the medium for flex and bison
 *  @param yyscanner flex state
 *  @param tok       the type of token
 *
 *  @return the type of token
 */
int css_tokenize(CSSPARSERSTYPE* lval , CSSPARSERLTYPE* loc, yyscan_t scanner, CssParser* parser, int tok)
{
    char* origin_text = cssprsr_get_text(scanner);
    
    yy_size_t len = 0;
    
    char* text = cssprsr_normalize_text(&len, origin_text, cssprsr_get_leng(scanner), tok);
    
#ifdef CSSPRSR_RFELX_DEBUG
#if CSSPRSR_RFELX_DEBUG
    if ( tok == CSSPRSR_RCSS_WHITESPACE ) {
        cssprsr_parser_log(parser, "【%30s】=>【%30s】: %s", "", "", cssprsr_token_string(tok));
    } else {
        cssprsr_parser_log(parser, "【%30s】=>【%30s】: %s", origin_text, text, cssprsr_token_string(tok));
    }
#endif // #if CSSPRSR_RFELX_DEBUG
#endif // #ifdef CSSPRSR_RFELX_DEBUG
    yy_size_t length = len;
    switch ( tok ) {
        case CSSPRSR_RCSS_INCLUDES:
        case CSSPRSR_RCSS_DASHMATCH:
        case CSSPRSR_RCSS_BEGINSWITH:
        case CSSPRSR_RCSS_ENDSWITH:
        case CSSPRSR_RCSS_CONTAINS:
            break;
        case CSSPRSR_RCSS_STRING:
        case CSSPRSR_RCSS_IDENT:
        case CSSPRSR_RCSS_NTH:
            
        case CSSPRSR_RCSS_HEX:
        case CSSPRSR_RCSS_IDSEL:
            
        case CSSPRSR_RCSS_DIMEN:
        case CSSPRSR_RCSS_INVALIDDIMEN:
        case CSSPRSR_RCSS_URI:
        case CSSPRSR_RCSS_FUNCTION:
        case CSSPRSR_RCSS_ANYFUNCTION:
        case CSSPRSR_RCSS_CUEFUNCTION:
        case CSSPRSR_RCSS_NOTFUNCTION:
        case CSSPRSR_RCSS_CALCFUNCTION:
        case CSSPRSR_RCSS_MINFUNCTION:
        case CSSPRSR_RCSS_MAXFUNCTION:
        case CSSPRSR_RCSS_HOSTFUNCTION:
        case CSSPRSR_RCSS_HOSTCONTEXTFUNCTION:
        case CSSPRSR_RCSS_UNICODERANGE:
        {
            lval->string.data = text;
            lval->string.length = length;
        }
            break;
            
        case CSSPRSR_RCSS_IMPORT_SYM:
        case CSSPRSR_RCSS_PAGE_SYM:
        case CSSPRSR_RCSS_MEDIA_SYM:
        case CSSPRSR_RCSS_SUPPORTS_SYM:
        case CSSPRSR_RCSS_FONT_FACE_SYM:
        case CSSPRSR_RCSS_CHARSET_SYM:
        case CSSPRSR_RCSS_NAMESPACE_SYM:
//        case CSSPRSR_RCSS_VIEWPORT_RULE_SYM:
        case CSSPRSR_RINTERNAL_DECLS_SYM:
        case CSSPRSR_RINTERNAL_MEDIALIST_SYM:
        case CSSPRSR_RINTERNAL_RULE_SYM:
        case CSSPRSR_RINTERNAL_SELECTOR_SYM:
        case CSSPRSR_RINTERNAL_VALUE_SYM:
        case CSSPRSR_RINTERNAL_KEYFRAME_RULE_SYM:
        case CSSPRSR_RINTERNAL_KEYFRAME_KEY_LIST_SYM:
        case CSSPRSR_RINTERNAL_SUPPORTS_CONDITION_SYM:
        case CSSPRSR_RCSS_KEYFRAMES_SYM:
            break;
        case CSSPRSR_RCSS_QEMS:
            length--;
        case CSSPRSR_RCSS_GRADS:
        case CSSPRSR_RCSS_TURNS:
            length--;
        case CSSPRSR_RCSS_DEGS:
        case CSSPRSR_RCSS_RADS:
        case CSSPRSR_RCSS_KHERTZ:
        case CSSPRSR_RCSS_REMS:
            length--;
        case CSSPRSR_RCSS_MSECS:
        case CSSPRSR_RCSS_HERTZ:
        case CSSPRSR_RCSS_EMS:
        case CSSPRSR_RCSS_EXS:
        case CSSPRSR_RCSS_PXS:
        case CSSPRSR_RCSS_CMS:
        case CSSPRSR_RCSS_MMS:
        case CSSPRSR_RCSS_INS:
        case CSSPRSR_RCSS_PTS:
        case CSSPRSR_RCSS_PCS:
            length--;
        case CSSPRSR_RCSS_SECS:
        case CSSPRSR_RCSS_PERCENTAGE:
            length--;
        case CSSPRSR_RCSS_FLOATTOKEN:
            lval->number.val = cssprsr_characters_to_double(text, length, NULL);
            lval->number.raw.data = text;
            lval->number.raw.length = len;
            break;
        case CSSPRSR_RCSS_INTEGER:
            lval->number.val = (int)cssprsr_characters_to_double(text, length, NULL);
            lval->number.raw.data = text;
            lval->number.raw.length = len;

            break;
        default:
            break;
    }
    
    return tok;
}

/**
 *  Format token
 *
 *  @param length
 *  @param origin_text   original text from the flex
 *  @param origin_length formatted length
 *  @param tok
 *
 *  @return normalized text
 */
static inline char* cssprsr_normalize_text(yy_size_t* length, char *origin_text, yy_size_t origin_length, int tok)
{
    char * start = origin_text;
    yy_size_t l = origin_length;
    switch ( tok ) {
        case CSSPRSR_RCSS_STRING:
            l--;
            /* nobreak */
        case CSSPRSR_RCSS_HEX:
        case CSSPRSR_RCSS_IDSEL:
            start++;
            l--;
            break;
        case CSSPRSR_RCSS_URI:
            // "url("{w}{string}{w}")"
            // "url("{w}{url}{w}")"
            // strip "url(" and ")"
            start += 4;
            l -= 5;
            // strip {w}
            while (l && cssprsr_is_html_space(*start)) {
                ++start;
                --l;
            }
            while (l && cssprsr_is_html_space(start[l - 1]))
                --l;
            if (l && (*start == '"' || *start == '\'')) {
                assert(l >= 2 && start[l - 1] == *start);
                ++start;
                l -= 2;
            }
            break;
        default:
            break;
    }
    
    *length = l;
    return start;
}

double cssprsr_characters_to_double(const char* data, size_t length, bool* ok)
{
    if (!length) {
        if (ok)
            *ok = false;
        return 0.0;
    }
    
    char* bytes = malloc(sizeof(char) * (length + 1));
    for (unsigned i = 0; i < length; ++i) {
        bytes[i] = data[i] < 0x7F ? data[i] : '?';
    }
    bytes[length] = '\0';
    char* end;
    double val = strtod(bytes, &end);
    if (ok) {
        *ok = (end == 0 || *end == '\0');
    }
	free(bytes);
    return val;
}

#ifdef CSSPRSR_RFELX_DEBUG
#if CSSPRSR_RFELX_DEBUG
static char * cssprsr_token_table[] = {
    "TOKEN_EOF", "LOWEST_PREC", "UNIMPORTANT_TOK",
    "CSSPRSR_RCSS_SGML_CD", "CSSPRSR_RCSS_WHITESPACE", "CSSPRSR_RCSS_INCLUDES",
    "CSSPRSR_RCSS_DASHMATCH", "CSSPRSR_RCSS_BEGINSWITH", "CSSPRSR_RCSS_ENDSWITH",
    "CSSPRSR_RCSS_CONTAINS", "CSSPRSR_RCSS_STRING", "CSSPRSR_RCSS_IDENT",
    "CSSPRSR_RCSS_NTH", "CSSPRSR_RCSS_HEX", "CSSPRSR_RCSS_IDSEL", "CSSPRSR_RCSS_IMPORT_SYM", "CSSPRSR_RCSS_PAGE_SYM",
    "CSSPRSR_RCSS_MEDIA_SYM", "CSSPRSR_RCSS_SUPPORTS_SYM",
    "CSSPRSR_RCSS_FONT_FACE_SYM", "CSSPRSR_RCSS_CHARSET_SYM",
    "CSSPRSR_RCSS_NAMESPACE_SYM", "CSSPRSR_RINTERNAL_DECLS_SYM",
    "CSSPRSR_RINTERNAL_MEDIALIST_SYM", "CSSPRSR_RINTERNAL_RULE_SYM",
    "CSSPRSR_RINTERNAL_SELECTOR_SYM", "CSSPRSR_RINTERNAL_VALUE_SYM",
    "CSSPRSR_RINTERNAL_KEYFRAME_RULE_SYM",
    "CSSPRSR_RINTERNAL_KEYFRAME_KEY_LIST_SYM",
    "CSSPRSR_RINTERNAL_SUPPORTS_CONDITION_SYM", "CSSPRSR_RCSS_KEYFRAMES_SYM",
    "CSSPRSR_RCSS_ATKEYWORD", "CSSPRSR_RCSS_IMPORTANT_SYM",
    "CSSPRSR_RCSS_MEDIA_NOT", "CSSPRSR_RCSS_MEDIA_ONLY", "CSSPRSR_RCSS_MEDIA_AND",
    "CSSPRSR_RCSS_MEDIA_OR", "CSSPRSR_RCSS_SUPPORTS_NOT",
    "CSSPRSR_RCSS_SUPPORTS_AND", "CSSPRSR_RCSS_SUPPORTS_OR", "CSSPRSR_RCSS_REMS",
    "CSSPRSR_RCSS_CHS", "CSSPRSR_RCSS_QEMS", "CSSPRSR_RCSS_EMS", "CSSPRSR_RCSS_EXS",
    "CSSPRSR_RCSS_PXS", "CSSPRSR_RCSS_CMS", "CSSPRSR_RCSS_MMS", "CSSPRSR_RCSS_INS",
    "CSSPRSR_RCSS_PTS", "CSSPRSR_RCSS_PCS", "CSSPRSR_RCSS_DEGS", "CSSPRSR_RCSS_RADS",
    "CSSPRSR_RCSS_GRADS", "CSSPRSR_RCSS_TURNS", "CSSPRSR_RCSS_MSECS",
    "CSSPRSR_RCSS_SECS", "CSSPRSR_RCSS_HERTZ", "CSSPRSR_RCSS_KHERTZ",
    "CSSPRSR_RCSS_DIMEN", "CSSPRSR_RCSS_INVALIDDIMEN", "CSSPRSR_RCSS_PERCENTAGE",
    "CSSPRSR_RCSS_FLOATTOKEN", "CSSPRSR_RCSS_INTEGER", "CSSPRSR_RCSS_VW",
    "CSSPRSR_RCSS_VH", "CSSPRSR_RCSS_VMIN", "CSSPRSR_RCSS_VMAX", "CSSPRSR_RCSS_DPPX",
    "CSSPRSR_RCSS_DPI", "CSSPRSR_RCSS_DPCM", "CSSPRSR_RCSS_FR", "CSSPRSR_RCSS_URI",
    "CSSPRSR_RCSS_FUNCTION", "CSSPRSR_RCSS_ANYFUNCTION",
    "CSSPRSR_RCSS_CUEFUNCTION", "CSSPRSR_RCSS_NOTFUNCTION",
    "CSSPRSR_RCSS_CALCFUNCTION", "CSSPRSR_RCSS_MINFUNCTION",
    "CSSPRSR_RCSS_MAXFUNCTION", "CSSPRSR_RCSS_HOSTFUNCTION",
    "CSSPRSR_RCSS_HOSTCONTEXTFUNCTION", "CSSPRSR_RCSS_UNICODERANGE" };

static char * cssprsr_token_string(int tok)
{
    if (tok > 257)
    {
        return cssprsr_token_table[tok-257];
    }
    else if ( 0 == tok )
    {
        return cssprsr_token_table[0];
    }
    else
    {
        char* unicode = malloc(2);
        unicode[0] = (char)tok;
        unicode[1] = '\0';
        return unicode;
    }
}
#endif // #if CSSPRSR_RFELX_DEBUG
#endif // #ifdef CSSPRSR_RFELX_DEBUG

inline bool cssprsr_is_html_space(char c)
{
    return c <= ' ' && (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f');
}

//inline int cssprsr_to_ascii_hex_value(char c)
//{
//    //    ASSERT(isASCIIHexDigit(c));
//    return c < 'A' ? c - '0' : (c - 'A' + 10) & 0xF;
//}

//inline bool cssprsr_is_ascii_hex_digit(char c)
//{
//    return (c >= '0' && c <= '9') || ((c | 0x20) >= 'a' && (c | 0x20) <= 'f');
//}
