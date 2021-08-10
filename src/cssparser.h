/**
 * @file cssparser.h
 * @brief libcssparser public api.
 *
 * @author zhang
 * @version
 * @date
 * @note
 * @since
 * CSS Spec: http://www.w3.org/TR/css-syntax-3/
 *
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
#ifndef _CSS_PARSER_PULIC_API_
#define _CSS_PARSER_PULIC_API_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CSS_ERROR_MSG_SIZE 128

#if defined(CSSPARSER_DLL)
/* win32 dynamic dll */
# ifdef CSSPARSER_EXPORTS
#   define CSSPARSER_API __declspec(dllexport)
# else
#   define CSSPARSER_API __declspec(dllimport)
# endif
#else
/* static lib or linux so */
# define CSSPARSER_API  extern
#endif


typedef enum {
    CssRuleUnkown,
    CssRuleStyle,
    CssRuleImport,
    CssRuleMedia,
    CssRuleFontFace,
    CssRuleSupports,
    CssRuleKeyframes,
    CssRuleCharset,
    CssRuleHost,
} CssRuleType;


typedef enum {
    CssMediaQueryResNone,
    CssMediaQueryResOnly,
    CssMediaQueryResNot,
} CssMediaQueryRestrictor;


typedef enum {
    CssSelMatchUnknown = 0,
    CssSelMatchTag,                 // Example: div
    CssSelMatchId,                  // Example: #id
    CssSelMatchClass,               // example: .class
    CssSelMatchPseudoClass,         // Example:  :nth-child(2)
    CssSelMatchPseudoElement,       // Example: ::first-line
    CssSelMatchPagePseudoClass,     // ??
    CssSelMatchAttrExact,           // Example: E[foo="bar"]
    CssSelMatchAttrSet,             // Example: E[foo]
    CssSelMatchAttrList,            // Example: E[foo~="bar"]
    CssSelMatchAttrHyphen,          // Example: E[foo|="bar"]
    CssSelMatchAttrContain,         // css3: E[foo*="bar"]
    CssSelMatchAttrBegin,           // css3: E[foo^="bar"]
    CssSelMatchAttrEnd,             // css3: E[foo$="bar"]
    CssSelMatchFirstAttr = CssSelMatchAttrExact,
} CssSelectorMatch;


typedef enum {
    CssSelRelSubSelector,      // "No space" combinator
    CssSelRelDescendant,       // "Space" combinator
    CssSelRelChild,            // > combinator
    CssSelRelDirectAdjacent,   // + combinator
    CssSelRelIndirectAdjacent, // ~ combinator
    CssSelRelShadowPseudo,     // Special case of shadow DOM pseudo elements / shadow pseudo element
    CssSelRelShadowDeep        // /shadow-deep/ combinator
} CssSelectorRelation;


typedef enum {
    CssPseudoNotParsed,
    CssPseudoUnknown,
    CssPseudoEmpty,
    CssPseudoFirstChild,
    CssPseudoFirstOfType,
    CssPseudoLastChild,
    CssPseudoLastOfType,
    CssPseudoOnlyChild,
    CssPseudoOnlyOfType,
    CssPseudoFirstLine,
    CssPseudoFirstLetter,
    CssPseudoNthChild,
    CssPseudoNthOfType,
    CssPseudoNthLastChild,
    CssPseudoNthLastOfType,
    CssPseudoLink,
    CssPseudoVisited,
    CssPseudoAny,
    CssPseudoAnyLink,
    CssPseudoAutofill,
    CssPseudoHover,
    CssPseudoDrag,
    CssPseudoFocus,
    CssPseudoActive,
    CssPseudoChecked,
    CssPseudoEnabled,
    CssPseudoFullPageMedia,
    CssPseudoDefault,
    CssPseudoDisabled,
    CssPseudoOptional,
    CssPseudoRequired,
    CssPseudoReadOnly,
    CssPseudoReadWrite,
    CssPseudoValid,
    CssPseudoInvalid,
    CssPseudoIndeterminate,
    CssPseudoTarget,
    CssPseudoBefore,
    CssPseudoAfter,
    CssPseudoBackdrop,
    CssPseudoLang,
    CssPseudoNot, // :not(selector), selector is Kind of CssSelector
    CssPseudoResizer,
    CssPseudoRoot,
    CssPseudoScope,
    CssPseudoScrollbar,
    CssPseudoScrollbarButton,
    CssPseudoScrollbarCorner,
    CssPseudoScrollbarThumb,
    CssPseudoScrollbarTrack,
    CssPseudoScrollbarTrackPiece,
    CssPseudoWindowInactive,
    CssPseudoCornerPresent,
    CssPseudoDecrement,
    CssPseudoIncrement,
    CssPseudoHorizontal,
    CssPseudoVertical,
    CssPseudoStart,
    CssPseudoEnd,
    CssPseudoDoubleButton,
    CssPseudoSingleButton,
    CssPseudoNoButton,
    CssPseudoSelection,
    CssPseudoLeftPage,
    CssPseudoRightPage,
    CssPseudoFirstPage,
    CssPseudoFullScreen,
    CssPseudoFullScreenDocument,
    CssPseudoFullScreenAncestor,
    CssPseudoInRange,
    CssPseudoOutOfRange,
    CssPseudoWebKitCustomElement,
    CssPseudoCue,
    CssPseudoFutureCue,
    CssPseudoPastCue,
    CssPseudoUnresolved,
    CssPseudoContent,
    CssPseudoHost,
    CssPseudoHostContext,
    CssPseudoShadow,
    CssPseudoSpatialNavFocus,
    CssPseudoListBox
} CssPseudoType;


typedef enum {
    CssAMTCaseSensitive,
    CssAMTCaseInsensitive,
} CssAttributeMatchType;


typedef enum {
    CSS_VALUE_UNKNOWN = 0,
    CSS_VALUE_NUMBER = 1,
    CSS_VALUE_PERCENTAGE = 2,
    CSS_VALUE_EMS = 3,
    CSS_VALUE_EXS = 4,
    CSS_VALUE_PX = 5,
    CSS_VALUE_CM = 6,
    CSS_VALUE_MM = 7,
    CSS_VALUE_IN = 8,
    CSS_VALUE_PT = 9,
    CSS_VALUE_PC = 10,
    CSS_VALUE_DEG = 11,
    CSS_VALUE_RAD = 12,
    CSS_VALUE_GRAD = 13,
    CSS_VALUE_MS = 14,
    CSS_VALUE_S = 15,
    CSS_VALUE_HZ = 16,
    CSS_VALUE_KHZ = 17,
    CSS_VALUE_DIMENSION = 18,
    CSS_VALUE_STRING = 19,
    CSS_VALUE_URI = 20,
    CSS_VALUE_IDENT = 21,
    CSS_VALUE_ATTR = 22,
    CSS_VALUE_COUNTER = 23,
    CSS_VALUE_RECT = 24,
    CSS_VALUE_RGBCOLOR = 25,
    CSS_VALUE_VW = 26,
    CSS_VALUE_VH = 27,
    CSS_VALUE_VMIN = 28,
    CSS_VALUE_VMAX = 29,
    CSS_VALUE_DPPX = 30,
    CSS_VALUE_DPI = 31,
    CSS_VALUE_DPCM = 32,
    CSS_VALUE_FR = 33,
    CSS_VALUE_UNICODE_RANGE = 102,
    CSS_VALUE_PARSER_OPERATOR = 103,
    CSS_VALUE_PARSER_INTEGER = 104,
    CSS_VALUE_PARSER_HEXCOLOR = 105,
    CSS_VALUE_PARSER_FUNCTION = 0x100001,
    CSS_VALUE_PARSER_LIST     = 0x100002,
    CSS_VALUE_PARSER_Q_EMS    = 0x100003,
    CSS_VALUE_PARSER_IDENTIFIER = 106,
    CSS_VALUE_TURN = 107,
    CSS_VALUE_REMS = 108,
    CSS_VALUE_CHS = 109,
    CSS_VALUE_COUNTER_NAME = 110,
    CSS_VALUE_SHAPE = 111,
    CSS_VALUE_QUAD = 112,
    CSS_VALUE_CALC = 113,
    CSS_VALUE_CALC_PERCENTAGE_WITH_NUMBER = 114,
    CSS_VALUE_CALC_PERCENTAGE_WITH_LENGTH = 115,
    CSS_VALUE_VARIABLE_NAME = 116,
    CSS_VALUE_PROPERTY_ID = 117,
    CSS_VALUE_VALUE_ID = 118
} CssValueUnit;


typedef enum {
    CssValueInvalid = 0,
    CssValueInherit = 1,
    CssValueInitial = 2,
    CssValueNone = 3,
    CssValueCustom = 0x100010,
} CssValueID;


typedef enum { CssParseError } CssErrorType;


typedef struct {
    const char* local; // tag local name
    const char* prefix; // namesapce identifier
    const char* uri; // namesapce uri
} CssQualifiedName;


typedef struct {
  /** Data elements. This points to a dynamically-allocated array of capacity
   * elements, each a void* to the element itself, remember free each element.
   */
  void** data;

  /** Number of elements currently in the array. */
  unsigned int length;

  /** Current array capacity. */
  unsigned int capacity;
} CssArray;


typedef struct {
    const char* encoding;
    CssArray /* CssRule */ rules;
    CssArray /* CssImportRule */ imports;
} CssStylesheet;


typedef struct {
    const char* name;
    CssRuleType type;
} CssRule;


typedef struct {
    CssRule base;
    CssArray* /* CssSelector */ selectors;
    CssArray* /* CssDeclaration */ declarations;
} CssStyleRule;


typedef struct {
    const char* comment;
} CssComment; // unused for right


/**
 * The `@font-face` at-rule.
 */
typedef struct {
    CssRule base;
    CssArray* /* CssDeclaration */ declarations;
} CssFontFaceRule;


/**
 * The `@host` at-rule.
 */
typedef struct {
    CssRule base;
    CssArray* /* CssRule */ host;
} CssHostRule;


/**
 * The `@import` at-rule.
 */
typedef struct {
    CssRule base;
    /**
     * The part following `@import `
     */
    const char* href;
    /**
     * The media list belonging to this import rule
     */
    CssArray* /* CssMediaQuery* */ medias;
} CssImportRule;


/**
 * The `@keyframes` at-rule.
 * Spec: http://www.w3.org/TR/css3-animations/#keyframes
 */
typedef struct {
    CssRule base;
    /**
     * The vendor prefix in `@keyframes`, or `undefined` if there is none.
     */
    const char* name;
    CssArray* /* CssKeyframe */ keyframes;
} CssKeyframesRule;


typedef struct {
    CssArray* /* CssValue: `percentage`, `from`, `to` */ selectors;
    CssArray* /* CssDeclaration */ declarations;
} CssKeyframe;


/**
 * The `@media` at-rule.
 */
typedef struct {
    CssRule base;
    /**
     * The part following `@media `
     */
    CssArray* medias;
    /**
     * An `Array` of nodes with the types `rule`, `comment` and any of the
     at-rule types.
     */
    CssArray* /* CssRule */ rules;
} CssMediaRule;


/**
 * Media Query Exp List
 * Spec: http://www.w3.org/TR/mediaqueries-4/
 */
typedef struct {
    CssMediaQueryRestrictor restrictor;
    const char* type;
    CssArray* expressions;
    bool ignored;
} CssMediaQuery;


typedef struct {
    const char* feature;
    CssArray* values;
    const char* raw;
} CssMediaQueryExp;


typedef struct {
    const char* value;
    union {
        struct {
            int a; // Used for :nth-*
            int b; // Used for :nth-*
        } nth;
        CssAttributeMatchType attrMatchType; // used for attribute selector (with value)
    } bits;
    CssQualifiedName* attribute;
    const char* argument; // Used for :contains, :lang, :nth-*
    CssArray* selectors; // Used for :any and :not
} CssSelectorRareData;


typedef struct CssSelector {
    size_t specificity;
    CssSelectorMatch match;
    CssPseudoType pseudo;
    CssSelectorRelation relation;
    CssQualifiedName* tag;
    CssSelectorRareData* data;
    struct CssSelector* tagHistory;
} CssSelector;


unsigned css_calc_specificity_for_selector(CssSelector* selector);


typedef struct {
    // property name
    const char* property;
    
    // property value
    CssArray* /* CssValue */ values;
    const char* string;

    // is this property marked important
    bool important;

    // origin css text of the property
    const char* raw;
} CssDeclaration;


typedef struct {
    const char* name;
    CssArray* args;
} CssValueFunction;


typedef struct CssValue {
    CssValueID id;
    bool isInt;
    union {
        int iValue;
        double fValue;
        const char* string;
        CssValueFunction* function;
        CssArray* list;
    };
    CssValueUnit unit;
    const char* raw;
} CssValue;


/**
 * The `@charset` at-rule.
 */
typedef struct {
    CssRule base;
    const char* encoding;
} CssCharsetRule;
    
typedef struct {
    CssErrorType type;
    int first_line;
    int first_column;
    int last_line;
    int last_column;
    char message[CSS_ERROR_MSG_SIZE];
} CssError;


/**
 * Parser mode
 */
typedef enum CssParserMode {
    // Normal CSS content used in External CSS files or Internal CSS,
    //  may include more than 1 css rules.
    CssParserModeStylesheet,

    // Single CSS rule like "@import", "selector{...}"
    CssParserModeRule,

    CssParserModeKeyframeRule,
    CssParserModeKeyframeKeyList,
    CssParserModeMediaList,

    // CSS property value like "1px", "1em", "#eee"
    CssParserModeValue,

    // CSS selector like ".pages.active"
    CssParserModeSelector,

    // Inline stylesheet like "width: 20px; height: 20px;"
    CssParserModeDeclarationList,
} CssParserMode;
    
typedef struct CssInternalOutput {
    // Complete CSS string
    CssStylesheet* stylesheet;
    union {
        // fragmental CSS string
        CssRule* rule;
        CssKeyframe* keyframe;
        CssArray* keyframe_keys;
        CssArray* values;
        CssArray* medias;
        CssArray* /* CssDeclaration */ declarations;
        CssArray* selectors;
    };
    CssParserMode mode;
    CssArray /* CssError */ errors;
} CssOutput;


/**
 *  Parse a complete or fragmental CSS string
 *
 *  @param str  Input CSS string
 *  @param len  Length of the input CSS string
 *  @param mode Parser mode, depends on the input
 *
 *  @return The result of parsing
 */
CSSPARSER_API CssOutput* css_parse_string(const char* str, size_t len, CssParserMode mode);


/**
 *  Parse a complete CSS file
 *
 *  @param fp `FILE` point to the CSS file
 *
 *  @return The result of parsing
 */
CSSPARSER_API CssOutput* css_parse_file(FILE* fp);


/**
 *  Free the output
 *
 *  @param output The result of parsing
 */
CSSPARSER_API void css_destroy_output(CssOutput* output);


/**
 *  Print the formatted CSS string
 *
 *  @param output The result of parsing
 *
 *  @return The origin output
 */
CSSPARSER_API CssOutput* css_dump_output(CssOutput* output);

#ifdef __cplusplus
}
#endif

#endif /* defined(_CSS_PARSER_PULIC_API_) */
