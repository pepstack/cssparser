/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef _CSSPARSER__TAB_H_
#define _CSSPARSER__TAB_H_

/* Debug traces.  */
#ifndef CSSPARSERDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define CSSPARSERDEBUG 1
#  else
#   define CSSPARSERDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define CSSPARSERDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined CSSPARSERDEBUG */
#if CSSPARSERDEBUG
extern int cssprsr_debug;
#endif
/* "%code requires" blocks.  */


/*
*  Copyright (C) 2002-2003 Lars Knoll (knoll@kde.org)
*  Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Apple Inc. All rights reserved.
*  Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
*  Copyright (C) 2008 Eric Seidel <eric@webkit.org>
*  Copyright (C) 2012 Intel Corporation. All rights reserved.
*  Copyright (C) 2015 QFish (im@qfi.sh)
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
*/

#include "foundation.h"
#include "cssparser.h"



/* Token type.  */
#ifndef CSSPARSERTOKENTYPE
# define CSSPARSERTOKENTYPE
  enum cssprsr_tokentype
  {
    TOKEN_EOF = 0,
    LOWEST_PREC = 258,
    UNIMPORTANT_TOK = 259,
    CSSPRSR_RCSS_SGML_CD = 260,
    CSSPRSR_RCSS_WHITESPACE = 261,
    CSSPRSR_RCSS_INCLUDES = 262,
    CSSPRSR_RCSS_DASHMATCH = 263,
    CSSPRSR_RCSS_BEGINSWITH = 264,
    CSSPRSR_RCSS_ENDSWITH = 265,
    CSSPRSR_RCSS_CONTAINS = 266,
    CSSPRSR_RCSS_STRING = 267,
    CSSPRSR_RCSS_IDENT = 268,
    CSSPRSR_RCSS_NTH = 269,
    CSSPRSR_RCSS_HEX = 270,
    CSSPRSR_RCSS_IDSEL = 271,
    CSSPRSR_RCSS_IMPORT_SYM = 272,
    CSSPRSR_RCSS_PAGE_SYM = 273,
    CSSPRSR_RCSS_MEDIA_SYM = 274,
    CSSPRSR_RCSS_SUPPORTS_SYM = 275,
    CSSPRSR_RCSS_FONT_FACE_SYM = 276,
    CSSPRSR_RCSS_CHARSET_SYM = 277,
    CSSPRSR_RCSS_NAMESPACE_SYM = 278,
    CSSPRSR_RINTERNAL_DECLS_SYM = 279,
    CSSPRSR_RINTERNAL_MEDIALIST_SYM = 280,
    CSSPRSR_RINTERNAL_RULE_SYM = 281,
    CSSPRSR_RINTERNAL_SELECTOR_SYM = 282,
    CSSPRSR_RINTERNAL_VALUE_SYM = 283,
    CSSPRSR_RINTERNAL_KEYFRAME_RULE_SYM = 284,
    CSSPRSR_RINTERNAL_KEYFRAME_KEY_LIST_SYM = 285,
    CSSPRSR_RINTERNAL_SUPPORTS_CONDITION_SYM = 286,
    CSSPRSR_RCSS_KEYFRAMES_SYM = 287,
    CSSPRSR_RCSS_ATKEYWORD = 288,
    CSSPRSR_RCSS_IMPORTANT_SYM = 289,
    CSSPRSR_RCSS_MEDIA_NOT = 290,
    CSSPRSR_RCSS_MEDIA_ONLY = 291,
    CSSPRSR_RCSS_MEDIA_AND = 292,
    CSSPRSR_RCSS_MEDIA_OR = 293,
    CSSPRSR_RCSS_SUPPORTS_NOT = 294,
    CSSPRSR_RCSS_SUPPORTS_AND = 295,
    CSSPRSR_RCSS_SUPPORTS_OR = 296,
    CSSPRSR_RCSS_REMS = 297,
    CSSPRSR_RCSS_CHS = 298,
    CSSPRSR_RCSS_QEMS = 299,
    CSSPRSR_RCSS_EMS = 300,
    CSSPRSR_RCSS_EXS = 301,
    CSSPRSR_RCSS_PXS = 302,
    CSSPRSR_RCSS_CMS = 303,
    CSSPRSR_RCSS_MMS = 304,
    CSSPRSR_RCSS_INS = 305,
    CSSPRSR_RCSS_PTS = 306,
    CSSPRSR_RCSS_PCS = 307,
    CSSPRSR_RCSS_DEGS = 308,
    CSSPRSR_RCSS_RADS = 309,
    CSSPRSR_RCSS_GRADS = 310,
    CSSPRSR_RCSS_TURNS = 311,
    CSSPRSR_RCSS_MSECS = 312,
    CSSPRSR_RCSS_SECS = 313,
    CSSPRSR_RCSS_HERTZ = 314,
    CSSPRSR_RCSS_KHERTZ = 315,
    CSSPRSR_RCSS_DIMEN = 316,
    CSSPRSR_RCSS_INVALIDDIMEN = 317,
    CSSPRSR_RCSS_PERCENTAGE = 318,
    CSSPRSR_RCSS_FLOATTOKEN = 319,
    CSSPRSR_RCSS_INTEGER = 320,
    CSSPRSR_RCSS_VW = 321,
    CSSPRSR_RCSS_VH = 322,
    CSSPRSR_RCSS_VMIN = 323,
    CSSPRSR_RCSS_VMAX = 324,
    CSSPRSR_RCSS_DPPX = 325,
    CSSPRSR_RCSS_DPI = 326,
    CSSPRSR_RCSS_DPCM = 327,
    CSSPRSR_RCSS_FR = 328,
    CSSPRSR_RCSS_URI = 329,
    CSSPRSR_RCSS_FUNCTION = 330,
    CSSPRSR_RCSS_ANYFUNCTION = 331,
    CSSPRSR_RCSS_CUEFUNCTION = 332,
    CSSPRSR_RCSS_NOTFUNCTION = 333,
    CSSPRSR_RCSS_CALCFUNCTION = 334,
    CSSPRSR_RCSS_MINFUNCTION = 335,
    CSSPRSR_RCSS_MAXFUNCTION = 336,
    CSSPRSR_RCSS_HOSTFUNCTION = 337,
    CSSPRSR_RCSS_HOSTCONTEXTFUNCTION = 338,
    CSSPRSR_RCSS_UNICODERANGE = 339
  };
#endif

/* Value type.  */
#if ! defined CSSPARSERSTYPE && ! defined CSSPARSERSTYPE_IS_DECLARED

union CSSPARSERSTYPE
{
    bool boolean;
    char character;
    int integer;
    CssParserNumber number;
    CssParserString string;

    CssRule* rule;
    // The content of the three below HeapVectors are guaranteed to be kept alive by
    // the corresponding parsedRules, floatingMediaQueryExpList, and parsedKeyFrames
    // lists
    CssArray* ruleList;
    CssArray* mediaQueryExpList;
    CssArray* keyframeRuleList;

    CssSelector* selector;
    CssArray* selectorList;
    // CSSSelector::MarginBoxType marginBox;
    CssSelectorRelation relation;
    CssAttributeMatchType attributeMatchType;
    CssArray* mediaList;
    CssMediaQuery* mediaQuery;
    CssMediaQueryRestrictor mediaQueryRestrictor;
    CssMediaQueryExp* mediaQueryExp;
    CssValue* value;
    CssArray* valueList;
    CssKeyframe* keyframe;
    CssSourcePosition* location;
};

typedef union CSSPARSERSTYPE CSSPARSERSTYPE;
# define CSSPARSERSTYPE_IS_TRIVIAL 1
# define CSSPARSERSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined CSSPARSERLTYPE && ! defined CSSPARSERLTYPE_IS_DECLARED
typedef struct CSSPARSERLTYPE CSSPARSERLTYPE;
struct CSSPARSERLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define CSSPARSERLTYPE_IS_DECLARED 1
# define CSSPARSERLTYPE_IS_TRIVIAL 1
#endif



int cssparse (void* scanner, struct CssInternalParser * parser);

#endif /* !_CSSPARSER__TAB_H_  */
