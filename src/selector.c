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

#include "selector.h"
#include <string.h>
#include <assert.h>
#include <strings.h>

#undef	assert
#define assert(x)

// Refs:
// http://www.w3.org/TR/css3-selectors/
//

static CssPseudoType name_to_pseudo_type(const char* name, bool hasArguments);

bool cssprsr_selector_crosses_tree_scopes(const CssSelector* selector)
{
    // TODO: To be supported
    return false;
}

// bool cssprsr_is_attribute_selector(const CssSelector* selector)
// {
//     return selector->match == CssSelectorMatchAttributeExact
//     || selector->match == CssSelectorMatchAttributeSet
//     || selector->match == CssSelectorMatchAttributeList
//     || selector->match == CssSelectorMatchAttributeHyphen
//     || selector->match == CssSelectorMatchAttributeContain
//     || selector->match == CssSelectorMatchAttributeBegin
//     || selector->match == CssSelectorMatchAttributeEnd;
// }

CssPseudoType cssprsr_parse_pseudo_type(const char* name, bool hasArguments)
{
    CssPseudoType pseudoType = name_to_pseudo_type(name, hasArguments);
    if (pseudoType != CssPseudoUnknown)
        return pseudoType;
    
    if (cssprsr_string_has_prefix(name, "-webkit-"))
        return CssPseudoWebKitCustomElement;
    
    return CssPseudoUnknown;
}

void cssprsr_selector_extract_pseudo_type(CssSelector* selector)
{
    if (selector->pseudo == CssPseudoNotParsed)
        selector->pseudo = CssPseudoUnknown;
    
    if (selector->match != CssSelectorMatchPseudoClass && selector->match != CssSelectorMatchPseudoElement && selector->match != CssSelectorMatchPagePseudoClass)
        return;
    bool hasArguments = (NULL != selector->data->argument) || (NULL != selector->data->selectors);    
    selector->pseudo = cssprsr_parse_pseudo_type(selector->data->value, hasArguments);
    
    bool element = false; // pseudo-element
    bool compat = false; // single colon compatbility mode
    bool isPagePseudoClass = false; // Page pseudo-class
    
    switch (selector->pseudo) {
        case CssPseudoAfter:
        case CssPseudoBefore:
        case CssPseudoFirstLetter:
        case CssPseudoFirstLine:
            compat = true;
        case CssPseudoBackdrop:
        case CssPseudoCue:
        case CssPseudoResizer:
        case CssPseudoScrollbar:
        case CssPseudoScrollbarCorner:
        case CssPseudoScrollbarButton:
        case CssPseudoScrollbarThumb:
        case CssPseudoScrollbarTrack:
        case CssPseudoScrollbarTrackPiece:
        case CssPseudoSelection:
        case CssPseudoWebKitCustomElement:
        case CssPseudoContent:
        case CssPseudoShadow:
            element = true;
            break;
        case CssPseudoUnknown:
        case CssPseudoEmpty:
        case CssPseudoFirstChild:
        case CssPseudoFirstOfType:
        case CssPseudoLastChild:
        case CssPseudoLastOfType:
        case CssPseudoOnlyChild:
        case CssPseudoOnlyOfType:
        case CssPseudoNthChild:
        case CssPseudoNthOfType:
        case CssPseudoNthLastChild:
        case CssPseudoNthLastOfType:
        case CssPseudoLink:
        case CssPseudoVisited:
        case CssPseudoAny:
        case CssPseudoAnyLink:
        case CssPseudoAutofill:
        case CssPseudoHover:
        case CssPseudoDrag:
        case CssPseudoFocus:
        case CssPseudoActive:
        case CssPseudoChecked:
        case CssPseudoEnabled:
        case CssPseudoFullPageMedia:
        case CssPseudoDefault:
        case CssPseudoDisabled:
        case CssPseudoOptional:
        case CssPseudoRequired:
        case CssPseudoReadOnly:
        case CssPseudoReadWrite:
        case CssPseudoScope:
        case CssPseudoValid:
        case CssPseudoInvalid:
        case CssPseudoIndeterminate:
        case CssPseudoTarget:
        case CssPseudoLang:
        case CssPseudoNot:
        case CssPseudoRoot:
        case CssPseudoWindowInactive:
        case CssPseudoCornerPresent:
        case CssPseudoDecrement:
        case CssPseudoIncrement:
        case CssPseudoHorizontal:
        case CssPseudoVertical:
        case CssPseudoStart:
        case CssPseudoEnd:
        case CssPseudoDoubleButton:
        case CssPseudoSingleButton:
        case CssPseudoNoButton:
        case CssPseudoNotParsed:
        case CssPseudoFullScreen:
        case CssPseudoFullScreenDocument:
        case CssPseudoFullScreenAncestor:
        case CssPseudoInRange:
        case CssPseudoOutOfRange:
        case CssPseudoFutureCue:
        case CssPseudoPastCue:
        case CssPseudoHost:
        case CssPseudoHostContext:
        case CssPseudoUnresolved:
        case CssPseudoSpatialNavigationFocus:
        case CssPseudoListBox:
            break;
        case CssPseudoFirstPage:
        case CssPseudoLeftPage:
        case CssPseudoRightPage:
            isPagePseudoClass = true;
            break;
    }
    
    bool matchPagePseudoClass = (selector->match == CssSelectorMatchPagePseudoClass);
    if (matchPagePseudoClass != isPagePseudoClass)
        selector->pseudo = CssPseudoUnknown;
    else if (selector->match == CssSelectorMatchPseudoClass && element) {
        if (!compat)
            selector->pseudo = CssPseudoUnknown;
        else
            selector->match = CssSelectorMatchPseudoElement;
    } else if (selector->match == CssSelectorMatchPseudoElement && !element)
        selector->pseudo = CssPseudoUnknown;
}

bool cssprsr_selector_matches_pseudo_element(CssSelector* selector)
{
    if (selector->pseudo == CssPseudoUnknown)
        cssprsr_selector_extract_pseudo_type(selector);
    return selector->match == CssSelectorMatchPseudoElement;
}

bool cssprsr_selector_is_custom_pseudo_element(CssSelector* selector)
{
    return selector->match == CssSelectorMatchPseudoElement && selector->pseudo == CssPseudoWebKitCustomElement;
}

bool cssprsr_selector_is_direct_adjacent(CssSelector* selector)
{
    return selector->relation == CssSelectorRelationDirectAdjacent || selector->relation == CssSelectorRelationIndirectAdjacent;
}

bool cssprsr_selector_is_adjacent(CssSelector* selector)
{
    return selector->relation == CssSelectorRelationDirectAdjacent;
}

bool cssprsr_selector_is_shadow(CssSelector* selector)
{
    return selector->relation == CssSelectorRelationShadowPseudo || selector->relation == CssSelectorRelationShadowDeep;
}

bool cssprsr_selector_is_sibling(CssSelector* selector)
{
    cssprsr_selector_extract_pseudo_type(selector);

    CssPseudoType type = selector->pseudo;
    return selector->relation == CssSelectorRelationDirectAdjacent
        || selector->relation == CssSelectorRelationIndirectAdjacent
        || type == CssPseudoEmpty
        || type == CssPseudoFirstChild
        || type == CssPseudoFirstOfType
        || type == CssPseudoLastChild
        || type == CssPseudoLastOfType
        || type == CssPseudoOnlyChild
        || type == CssPseudoOnlyOfType
        || type == CssPseudoNthChild
        || type == CssPseudoNthOfType
        || type == CssPseudoNthLastChild
        || type == CssPseudoNthLastOfType;
}

bool cssprsr_selector_is_attribute(const CssSelector* selector)
{
    return selector->match >= CssSelectorMatchFirstAttribute;
}

bool cssprsr_selector_is_content_pseudo_element(CssSelector* selector)
{
    cssprsr_selector_extract_pseudo_type(selector);
    return selector->match == CssSelectorMatchPseudoElement && selector->pseudo == CssPseudoContent;
}

bool cssprsr_selector_is_shadow_pseudo_element(CssSelector* selector)
{
    return selector->match == CssSelectorMatchPseudoElement
            && selector->pseudo == CssPseudoShadow;
}

bool cssprsr_selector_is_host_pseudo_class(CssSelector* selector)
{
    return selector->match == CssSelectorMatchPseudoClass && (selector->pseudo == CssPseudoHost || selector->pseudo == CssPseudoHostContext);
}

bool cssprsr_selector_is_tree_boundary_crossing(CssSelector* selector)
{
    cssprsr_selector_extract_pseudo_type(selector);
    return selector->match == CssSelectorMatchPseudoClass && (selector->pseudo == CssPseudoHost || selector->pseudo == CssPseudoHostContext);
}

bool cssprsr_selector_is_insertion_point_crossing(CssSelector* selector)
{
    cssprsr_selector_extract_pseudo_type(selector);
    return (selector->match == CssSelectorMatchPseudoClass && selector->pseudo == CssPseudoHostContext)
        || (selector->match == CssSelectorMatchPseudoElement && selector->pseudo == CssPseudoContent);
}

CssParserString* cssprsr_build_relation_selector_string(CssParser* parser, const char* relation, CssParserString* string, CssParserString* next, CssSelector* tagHistory)
{
    if ( NULL != relation ) {
        cssprsr_string_prepend_characters(parser, relation, string);
    }
    
    if ( NULL != next ) {
        cssprsr_string_append_string(parser, next, string);
    }
    
    CssParserString * result = cssprsr_selector_to_string(parser, tagHistory, (CssParserString*)string);
    cssprsr_parser_deallocate(parser, (void*) string->data);
    cssprsr_parser_deallocate(parser, (void*) string);
    return result;
}

CssParserString* cssprsr_selector_to_string(CssParser* parser, CssSelector* selector, CssParserString* next)
{
    CssParserString* string = cssprsr_parser_allocate(parser, sizeof(CssParserString));
    cssprsr_string_init(parser, string);
    
    bool tag_is_implicit = true;
    
    if (selector->match == CssSelectorMatchTag && tag_is_implicit)
    {
        if ( NULL == selector->tag->prefix )
            cssprsr_string_append_characters(parser, selector->tag->local, string);
        else {
            cssprsr_string_append_characters(parser, selector->tag->prefix, string);
            cssprsr_string_append_characters(parser, "|", string);
            cssprsr_string_append_characters(parser, selector->tag->local, string);
        }
    }

    const CssSelector* cs = selector;

    while (true) {
        if (cs->match == CssSelectorMatchId) {
            cssprsr_string_append_characters(parser, "#", string);
            cssprsr_string_append_characters(parser, cs->data->value, string);
        } else if (cs->match == CssSelectorMatchClass) {
            cssprsr_string_append_characters(parser, ".", string);
            cssprsr_string_append_characters(parser, cs->data->value, string);
        } else if (cs->match == CssSelectorMatchPseudoClass || cs->match == CssSelectorMatchPagePseudoClass) {
            cssprsr_string_append_characters(parser, ":", string);
            cssprsr_string_append_characters(parser, cs->data->value, string);
            
            switch (cs->pseudo) {
                case CssPseudoAny:
                case CssPseudoNot:
                case CssPseudoHost:
                case CssPseudoHostContext: {
                    if ( cs->data->selectors ) {
                        CssArray* sels = cs->data->selectors;
                        for (size_t i=0; i<sels->length; i++) {
                            CssParserString* str = cssprsr_selector_to_string(parser, sels->data[i], NULL);
                            cssprsr_string_append_string(parser, str, string);
                            cssprsr_parser_deallocate(parser, (void*) str->data);
                            cssprsr_parser_deallocate(parser, (void*) str);
                            if ( i != sels->length -1 ) {
                                cssprsr_string_append_characters(parser, ", ", string);
                            }
                        }
                        cssprsr_string_append_characters(parser, ")", string);
                    }
                }
                    break;
                case CssPseudoLang:
                case CssPseudoNthChild:
                case CssPseudoNthLastChild:
                case CssPseudoNthOfType:
                case CssPseudoNthLastOfType: {
                    cssprsr_string_append_characters(parser, cs->data->argument, string);
                    cssprsr_string_append_characters(parser, ")", string);
                }
                    break;
                default:
                    break;
            }
        } else if (cs->match == CssSelectorMatchPseudoElement) {
            cssprsr_string_append_characters(parser, "::", string);
            cssprsr_string_append_characters(parser, cs->data->value, string);
        } else if (cssprsr_selector_is_attribute(cs)) {
            cssprsr_string_append_characters(parser, "[", string);
            if (NULL != cs->data->attribute->prefix) {
                cssprsr_string_append_characters(parser, cs->data->attribute->prefix, string);
                cssprsr_string_append_characters(parser, "|", string);
            }
            cssprsr_string_append_characters(parser, cs->data->attribute->local, string);
            switch (cs->match) {
                case CssSelectorMatchAttributeExact:
                    cssprsr_string_append_characters(parser, "=", string);
                    break;
                case CssSelectorMatchAttributeSet:
                    cssprsr_string_append_characters(parser, "]", string);
                    break;
                case CssSelectorMatchAttributeList:
                    cssprsr_string_append_characters(parser, "~=", string);
                    break;
                case CssSelectorMatchAttributeHyphen:
                    cssprsr_string_append_characters(parser, "|=", string);
                    break;
                case CssSelectorMatchAttributeBegin:
                    cssprsr_string_append_characters(parser, "^=", string);
                    break;
                case CssSelectorMatchAttributeEnd:
                    cssprsr_string_append_characters(parser, "$=", string);
                    break;
                case CssSelectorMatchAttributeContain:
                    cssprsr_string_append_characters(parser, "*=", string);
                    break;
                default:
                    break;
            }
            if (cs->match != CssSelectorMatchAttributeSet) {
                cssprsr_string_append_characters(parser, "\"", string);
                cssprsr_string_append_characters(parser, cs->data->value, string);
                cssprsr_string_append_characters(parser, "\"", string);
                if (cs->data->bits.attributeMatchType == CssAttributeMatchTypeCaseInsensitive)
                    cssprsr_string_append_characters(parser, " i", string);
                cssprsr_string_append_characters(parser, "]", string);
            }
        }
        if (cs->relation != CssSelectorRelationSubSelector || !cs->tagHistory)
            break;
        cs = cs->tagHistory;
    }

    CssSelector* tagHistory = cs->tagHistory;

    if ( NULL != tagHistory ) {
        switch (cs->relation) {
            case CssSelectorRelationDescendant:
            {
                return cssprsr_build_relation_selector_string(parser, " ", string, next, tagHistory);
            }
            case CssSelectorRelationChild:
            {
                return cssprsr_build_relation_selector_string(parser, " > ", string, next, tagHistory);
            }
            case CssSelectorRelationShadowDeep:
            {
                return cssprsr_build_relation_selector_string(parser, " /deep/ ", string, next, tagHistory);
            }
            case CssSelectorRelationDirectAdjacent:
            {
                return cssprsr_build_relation_selector_string(parser, " + ", string, next, tagHistory);
            }
            case CssSelectorRelationIndirectAdjacent:
            {
                return cssprsr_build_relation_selector_string(parser, " ~ ", string, next, tagHistory);
            }
            case CssSelectorRelationSubSelector:
            {
                return NULL;
            }
            case CssSelectorRelationShadowPseudo:
            {
                return cssprsr_build_relation_selector_string(parser, NULL, string, next, tagHistory);
            }
        }
    }
    
    if ( NULL != next ) {
        cssprsr_string_append_string(parser, (CssParserString*)next, string);
    }
    
    return (CssParserString*)string;
}

unsigned calc_specificity_for_one_selector(const CssSelector* selector)
{
    switch ( selector->match ) {
        case CssSelectorMatchId:
            return 0x10000;
            
        case CssSelectorMatchPseudoClass:
        case CssSelectorMatchAttributeExact:
        case CssSelectorMatchClass:
        case CssSelectorMatchAttributeSet:
        case CssSelectorMatchAttributeList:
        case CssSelectorMatchAttributeHyphen:
        case CssSelectorMatchPseudoElement:
        case CssSelectorMatchAttributeContain:
        case CssSelectorMatchAttributeBegin:
        case CssSelectorMatchAttributeEnd:
            return 0x100;
            
        case CssSelectorMatchTag:
            return !strcasecmp(selector->tag->local, "*") ? 0 : 1;
        case CssSelectorMatchUnknown:
        case CssSelectorMatchPagePseudoClass:
            return 0;
    }
    
    return 0;
}

unsigned css_calc_specificity_for_selector(CssSelector* selector)
{
    if ( NULL == selector ) {
        return 0;
    }
    
    static const unsigned idMask = 0xff0000;
    static const unsigned classMask = 0xff00;
    static const unsigned elementMask = 0xff;
    
    unsigned total = 0;
    unsigned temp = 0;
    
    for (const CssSelector * next = selector; next; next = next->tagHistory)
    {
        temp = total + calc_specificity_for_one_selector(next);

        if ((temp & idMask) < (total & idMask))
            total |= idMask;
        else if ((temp & classMask) < (total & classMask))
            total |= classMask;
        else if ((temp & elementMask) < (total & elementMask))
            total |= elementMask;
        else
            total = temp;
    }
        
    return total;
}

// Could be made smaller and faster by replacing pointer with an
// offset into a string buffer and making the bit fields smaller but
// that could not be maintained by hand.
typedef struct {
    const char* string;
    unsigned type:8;
} CssNameToPseudoStruct;

// These tables should be kept sorted.
const static CssNameToPseudoStruct kPseudoTypeWithoutArgumentsMap[] = {
    {"-internal-list-box",            CssPseudoListBox},
    {"-internal-media-controls-cast-button", CssPseudoWebKitCustomElement},
    {"-internal-media-controls-overlay-cast-button", CssPseudoWebKitCustomElement},
    {"-internal-spatial-navigation-focus", CssPseudoSpatialNavigationFocus},
    {"-webkit-any-link",              CssPseudoAnyLink},
    {"-webkit-autofill",              CssPseudoAutofill},
    {"-webkit-drag",                  CssPseudoDrag},
    {"-webkit-full-page-media",       CssPseudoFullPageMedia},
    {"-webkit-full-screen",           CssPseudoFullScreen},
    {"-webkit-full-screen-ancestor",  CssPseudoFullScreenAncestor},
    {"-webkit-full-screen-document",  CssPseudoFullScreenDocument},
    {"-webkit-resizer",               CssPseudoResizer},
    {"-webkit-scrollbar",             CssPseudoScrollbar},
    {"-webkit-scrollbar-button",      CssPseudoScrollbarButton},
    {"-webkit-scrollbar-corner",      CssPseudoScrollbarCorner},
    {"-webkit-scrollbar-thumb",       CssPseudoScrollbarThumb},
    {"-webkit-scrollbar-track",       CssPseudoScrollbarTrack},
    {"-webkit-scrollbar-track-piece", CssPseudoScrollbarTrackPiece},
    {"active",                        CssPseudoActive},
    {"after",                         CssPseudoAfter},
    {"backdrop",                      CssPseudoBackdrop},
    {"before",                        CssPseudoBefore},
    {"checked",                       CssPseudoChecked},
    {"content",                       CssPseudoContent},
    {"corner-present",                CssPseudoCornerPresent},
    {"cue",                           CssPseudoWebKitCustomElement},
    {"decrement",                     CssPseudoDecrement},
    {"default",                       CssPseudoDefault},
    {"disabled",                      CssPseudoDisabled},
    {"double-button",                 CssPseudoDoubleButton},
    {"empty",                         CssPseudoEmpty},
    {"enabled",                       CssPseudoEnabled},
    {"end",                           CssPseudoEnd},
    {"first",                         CssPseudoFirstPage},
    {"first-child",                   CssPseudoFirstChild},
    {"first-letter",                  CssPseudoFirstLetter},
    {"first-line",                    CssPseudoFirstLine},
    {"first-of-type",                 CssPseudoFirstOfType},
    {"focus",                         CssPseudoFocus},
    {"future",                        CssPseudoFutureCue},
    {"horizontal",                    CssPseudoHorizontal},
    {"host",                          CssPseudoHost},
    {"hover",                         CssPseudoHover},
    {"in-range",                      CssPseudoInRange},
    {"increment",                     CssPseudoIncrement},
    {"indeterminate",                 CssPseudoIndeterminate},
    {"invalid",                       CssPseudoInvalid},
    {"last-child",                    CssPseudoLastChild},
    {"last-of-type",                  CssPseudoLastOfType},
    {"left",                          CssPseudoLeftPage},
    {"link",                          CssPseudoLink},
    {"no-button",                     CssPseudoNoButton},
    {"only-child",                    CssPseudoOnlyChild},
    {"only-of-type",                  CssPseudoOnlyOfType},
    {"optional",                      CssPseudoOptional},
    {"out-of-range",                  CssPseudoOutOfRange},
    {"past",                          CssPseudoPastCue},
    {"read-only",                     CssPseudoReadOnly},
    {"read-write",                    CssPseudoReadWrite},
    {"required",                      CssPseudoRequired},
    {"right",                         CssPseudoRightPage},
    {"root",                          CssPseudoRoot},
    {"scope",                         CssPseudoScope},
    {"selection",                     CssPseudoSelection},
    {"shadow",                        CssPseudoShadow},
    {"single-button",                 CssPseudoSingleButton},
    {"start",                         CssPseudoStart},
    {"target",                        CssPseudoTarget},
    {"unresolved",                    CssPseudoUnresolved},
    {"valid",                         CssPseudoValid},
    {"vertical",                      CssPseudoVertical},
    {"visited",                       CssPseudoVisited},
    {"window-inactive",               CssPseudoWindowInactive},
};

const static CssNameToPseudoStruct kPseudoTypeWithArgumentsMap[] = {
    {"-webkit-any(",      CssPseudoAny},
    {"cue(",              CssPseudoCue},
    {"host(",             CssPseudoHost},
    {"host-context(",     CssPseudoHostContext},
    {"lang(",             CssPseudoLang},
    {"not(",              CssPseudoNot},
    {"nth-child(",        CssPseudoNthChild},
    {"nth-last-child(",   CssPseudoNthLastChild},
    {"nth-last-of-type(", CssPseudoNthLastOfType},
    {"nth-of-type(",      CssPseudoNthOfType},
};

static const CssNameToPseudoStruct* lower_bound(const CssNameToPseudoStruct *map,
                                                   size_t count, const char* key);

static CssPseudoType name_to_pseudo_type(const char* name, bool hasArguments)
{
    if (NULL == name)
        return CssPseudoUnknown;
    
    const CssNameToPseudoStruct* pseudoTypeMap;
    size_t count;
    
    if (hasArguments) {
        pseudoTypeMap = kPseudoTypeWithArgumentsMap;
        count = sizeof(kPseudoTypeWithArgumentsMap) / sizeof(CssNameToPseudoStruct);
    } else {
        pseudoTypeMap = kPseudoTypeWithoutArgumentsMap;
        count = sizeof(kPseudoTypeWithoutArgumentsMap) / sizeof(CssNameToPseudoStruct);
    }
    
    const CssNameToPseudoStruct* match = lower_bound(pseudoTypeMap, count, name);
    if ( match == (pseudoTypeMap + count)
         || 0 != strcasecmp(match->string, name) )
        return CssPseudoUnknown;
    
    return match->type;
}

static const CssNameToPseudoStruct* lower_bound(const CssNameToPseudoStruct *array,
                                                   size_t size, const char* key) {
    const CssNameToPseudoStruct* it;
    const CssNameToPseudoStruct* first = array;
    size_t count = size, step;
    while (count > 0) {
        it = first;
        step = count / 2;
        it += step;
        if (strncmp(it->string, key, strlen(key)) < 0) {
            first = ++it;
            count -= step + 1;
        } else count = step;
    }
    return first;
}

#if CSSPRSR_RPARSER_DEBUG

void test_lower_bound()
{
    const CssNameToPseudoStruct* pseudoTypeMap;
    size_t count;
    
    pseudoTypeMap = kPseudoTypeWithArgumentsMap;
    count = sizeof(kPseudoTypeWithArgumentsMap) / sizeof(CssNameToPseudoStruct);
    
    for ( size_t i = 0; i < count; i++ ) {
        const CssNameToPseudoStruct* res = lower_bound(pseudoTypeMap, count, pseudoTypeMap[i].string);
        assert(pseudoTypeMap[i].type == res->type);
    }
    
    pseudoTypeMap = kPseudoTypeWithoutArgumentsMap;
    count = sizeof(kPseudoTypeWithoutArgumentsMap) / sizeof(CssNameToPseudoStruct);
    
    for ( size_t i = 0; i < count; i++ ) {
        const CssNameToPseudoStruct* res = lower_bound(pseudoTypeMap, count, pseudoTypeMap[i].string);
        assert(pseudoTypeMap[i].type == res->type);
    }
}

#endif // #if CSSPRSR_RPARSER_DEBUG
