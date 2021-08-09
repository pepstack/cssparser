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

// CSS Spec: http://www.w3.org/TR/css-syntax-3/

#ifndef _CSS_PARSER__API_
#define _CSS_PARSER__API_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CSS_ERROR_MSG_SIZE 128


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
    CssMediaQueryRestrictorNone,
    CssMediaQueryRestrictorOnly,
    CssMediaQueryRestrictorNot,
} CssMediaQueryRestrictor;

typedef enum {
    CssSelectorMatchUnknown = 0,
    CssSelectorMatchTag,                 // Example: div
    CssSelectorMatchId,                  // Example: #id
    CssSelectorMatchClass,               // example: .class
    CssSelectorMatchPseudoClass,         // Example:  :nth-child(2)
    CssSelectorMatchPseudoElement,       // Example: ::first-line
    CssSelectorMatchPagePseudoClass,     // ??
    CssSelectorMatchAttributeExact,      // Example: E[foo="bar"]
    CssSelectorMatchAttributeSet,        // Example: E[foo]
    CssSelectorMatchAttributeList,       // Example: E[foo~="bar"]
    CssSelectorMatchAttributeHyphen,     // Example: E[foo|="bar"]
    CssSelectorMatchAttributeContain,    // css3: E[foo*="bar"]
    CssSelectorMatchAttributeBegin,      // css3: E[foo^="bar"]
    CssSelectorMatchAttributeEnd,        // css3: E[foo$="bar"]
    CssSelectorMatchFirstAttribute = CssSelectorMatchAttributeExact,
} CssSelectorMatch;

typedef enum {
    CssSelectorRelationSubSelector,      // "No space" combinator
    CssSelectorRelationDescendant,       // "Space" combinator
    CssSelectorRelationChild,            // > combinator
    CssSelectorRelationDirectAdjacent,   // + combinator
    CssSelectorRelationIndirectAdjacent, // ~ combinator
    CssSelectorRelationShadowPseudo,     // Special case of shadow DOM pseudo elements / shadow pseudo element
    CssSelectorRelationShadowDeep        // /shadow-deep/ combinator
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
    CssPseudoSpatialNavigationFocus,
    CssPseudoListBox
} CssPseudoType;

typedef enum {
    CssAttributeMatchTypeCaseSensitive,
    CssAttributeMatchTypeCaseInsensitive,
} CssAttributeMatchType;

typedef enum {
    CSSPRSR_RVALUE_UNKNOWN = 0,
    CSSPRSR_RVALUE_NUMBER = 1,
    CSSPRSR_RVALUE_PERCENTAGE = 2,
    CSSPRSR_RVALUE_EMS = 3,
    CSSPRSR_RVALUE_EXS = 4,

	// double
    CSSPRSR_RVALUE_PX = 5,
    CSSPRSR_RVALUE_CM = 6,
    CSSPRSR_RVALUE_MM = 7,
    CSSPRSR_RVALUE_IN = 8,
    CSSPRSR_RVALUE_PT = 9,
    CSSPRSR_RVALUE_PC = 10,
    CSSPRSR_RVALUE_DEG = 11,
    CSSPRSR_RVALUE_RAD = 12,
    CSSPRSR_RVALUE_GRAD = 13,
    CSSPRSR_RVALUE_MS = 14,
    CSSPRSR_RVALUE_S = 15,
    CSSPRSR_RVALUE_HZ = 16,
    CSSPRSR_RVALUE_KHZ = 17,
    CSSPRSR_RVALUE_DIMENSION = 18,
    CSSPRSR_RVALUE_STRING = 19,
    CSSPRSR_RVALUE_URI = 20,
    CSSPRSR_RVALUE_IDENT = 21,
    CSSPRSR_RVALUE_ATTR = 22,
    CSSPRSR_RVALUE_COUNTER = 23,
    CSSPRSR_RVALUE_RECT = 24,
    CSSPRSR_RVALUE_RGBCOLOR = 25,

    CSSPRSR_RVALUE_VW = 26,
    CSSPRSR_RVALUE_VH = 27,
    CSSPRSR_RVALUE_VMIN = 28,
    CSSPRSR_RVALUE_VMAX = 29,
    CSSPRSR_RVALUE_DPPX = 30,
    CSSPRSR_RVALUE_DPI = 31,
    CSSPRSR_RVALUE_DPCM = 32,
    CSSPRSR_RVALUE_FR = 33,
    CSSPRSR_RVALUE_UNICODE_RANGE = 102,
    
    CSSPRSR_RVALUE_PARSER_OPERATOR = 103,
    CSSPRSR_RVALUE_PARSER_INTEGER = 104,
    CSSPRSR_RVALUE_PARSER_HEXCOLOR = 105,
    CSSPRSR_RVALUE_PARSER_FUNCTION = 0x100001,
    CSSPRSR_RVALUE_PARSER_LIST     = 0x100002,
    CSSPRSR_RVALUE_PARSER_Q_EMS    = 0x100003,
    
    CSSPRSR_RVALUE_PARSER_IDENTIFIER = 106,
    
    CSSPRSR_RVALUE_TURN = 107,
    CSSPRSR_RVALUE_REMS = 108,
    CSSPRSR_RVALUE_CHS = 109,
    
    CSSPRSR_RVALUE_COUNTER_NAME = 110,
    
    CSSPRSR_RVALUE_SHAPE = 111,
    
    CSSPRSR_RVALUE_QUAD = 112,
    
    CSSPRSR_RVALUE_CALC = 113,
    CSSPRSR_RVALUE_CALC_PERCENTAGE_WITH_NUMBER = 114,
    CSSPRSR_RVALUE_CALC_PERCENTAGE_WITH_LENGTH = 115,
    CSSPRSR_RVALUE_VARIABLE_NAME = 116,
    
    CSSPRSR_RVALUE_PROPERTY_ID = 117,
    CSSPRSR_RVALUE_VALUE_ID = 118
} CssValueUnit;

//typedef enum {
//    CSSPRSR_RVALUE_PARSER_OPERATOR = 0x100000,
//    CSSPRSR_RVALUE_PARSER_FUNCTION = 0x100001,
//    CSSPRSR_RVALUE_PARSER_LIST     = 0x100002,
//    CSSPRSR_RVALUE_PARSER_Q_EMS    = 0x100003,
//} CssParserValueUnit;

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
        CssAttributeMatchType attributeMatchType; // used for attribute selector (with value)
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
    /**
     * The encoding information
     */
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

// TODO: @document
// TODO: @page
// TODO: @supports
// TODO: custom-at-rule

/**
 * Parser mode
 */
typedef enum CssParserMode {
	// Normal CSS content used in External CSS files or Internal CSS, may include more than 1 css rules.
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
CssOutput* css_parse_string(const char* str, size_t len, CssParserMode mode);

/**
 *  Parse a complete CSS file
 *
 *  @param fp `FILE` point to the CSS file
 *
 *  @return The result of parsing
 */
CssOutput* css_parse_file(FILE* fp);

/**
 *  Free the output
 *
 *  @param output The result of parsing
 */
void css_destroy_output(CssOutput* output);

/**
 *  Print the formatted CSS string
 *
 *  @param output The result of parsing
 *
 *  @return The origin output
 */
CssOutput* css_dump_output(CssOutput* output);

#ifdef __cplusplus
}
#endif

#endif /* defined(_CSS_PARSER__API_) */
