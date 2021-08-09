//
//  test.c
//  CssParser
//
//  Created by QFish on 4/6/15.
//  Copyright (c) 2015 QFish. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>

#include "cssparser.h"

#include "win32/mscrtdbg.h"


void dump_css() {
    CssOutput* output = css_parse_string("div { color: #123456 }", strlen("div { color: #123456 }"), CssParserModeStylesheet);

    css_dump_output(output);
    css_destroy_output(output);
}

int main(int argc, const char * argv[])
{
    WINDOWS_CRTDBG_ON

    dump_css();

    return 0;
}