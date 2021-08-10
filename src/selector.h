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
#ifndef _CSS__SELECTOR_
#define _CSS__SELECTOR_

#include "cssparser.h"
#include "cssparser_i.h"

#ifdef __cplusplus
extern "C" {
#endif

CssParserString* cssprsr_selector_to_string(CssParser* parser, CssSelector* selector, CssParserString* next);
    
bool cssprsr_selector_crosses_tree_scopes(const CssSelector* selector);
bool cssprsr_selector_matches_pseudo_element(CssSelector* selector);
bool cssprsr_selector_is_custom_pseudo_element(CssSelector* selector);
bool cssprsr_selector_is_direct_adjacent(CssSelector* selector);
bool cssprsr_selector_is_adjacent(CssSelector* selector);
bool cssprsr_selector_is_shadow(CssSelector* selector);
bool cssprsr_selector_is_sibling(CssSelector* selector);
bool cssprsr_selector_is_attribute(const CssSelector* selector);
bool cssprsr_selector_is_content_pseudo_element(CssSelector* selector);
bool cssprsr_selector_is_shadow_pseudo_element(CssSelector* selector);
bool cssprsr_selector_is_host_pseudo_class(CssSelector* selector);
bool cssprsr_selector_is_tree_boundary_crossing(CssSelector* selector);
bool cssprsr_selector_is_insertion_point_crossing(CssSelector* selector);
    
#ifdef __cplusplus
}
#endif

#endif /* defined(_CSS__SELECTOR_) */
