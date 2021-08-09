//
//  fragment.c
//  CssParser
//
//  Created by QFish on 4/6/15.
//  Copyright (c) 2015 QFish. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#include "cssparser.h"

#define StopWatchBegin(begin) struct timeb t1; ftime(&t1);
#define StopWatchEnd(begin) struct timeb t2; ftime(&t2); printf("<" #begin "> costs %dms.\n", (t2.millitm - t1.millitm));

void help() {
    printf("Usage: fragment \"<fragment css string>\" <mode>.\n");
    printf("Mode: 0:Stylesheet\n");
    printf("      1:Rule\n");
    printf("      2:KeyframeRule\n");
    printf("      3:KeyframeKeyList\n");
    printf("      4:MediaList\n");
    printf("      5:Value\n");
    printf("      6:Selector\n");
    printf("      7:DeclarationList\n");
}

// export PKG_CONFIG_PATH=/usr/share/pkgconfig:/usr/lib/pkgconfig:/usr/local/lib/pkgconfig
// gcc examples/fragment.c `pkg-config --cflags --libs cssparser` -o fragment
// ./fragment "selector {property:value}" 0;

int main(int argc, const char * argv[]) {
    
    if (argc != 3) {
        help();
        exit(0);
    }
    
    const char * s = argv[1]; // "@import url(xxx);";
    int m = (const char)*argv[2] - '0';
    if ( m < 0 || m > 7 )
        help();
    StopWatchBegin(CssParseFile);
    CssOutput* output = css_parse_string(s, strlen(s), m);
    StopWatchEnd(CssParseFile);
    css_dump_output(output);
    css_destroy_output(output);
}


void test() {
    char * str = NULL;
    str = "(min-width: 992px) and (max-width: 1199px)";
    CssOutput * output_medialist = css_parse_string(str, strlen(str), CssParserModeMediaList);
    str = "@import url(xxx.css)";
    CssOutput * output_rule_import = css_parse_string(str, strlen(str), CssParserModeRule);
    str = "selector{a:b}";
    CssOutput * output_rule_style = css_parse_string(str, strlen(str), CssParserModeRule);
    str = "1px #333 dash";
    CssOutput * output_value = css_parse_string(str, strlen(str), CssParserModeValue);
    str = "tag .class #id";
    CssOutput * output_selectors = css_parse_string(str, strlen(str), CssParserModeSelector);
    str = "key1: val1; key2: val2; key3: val3;";
    CssOutput * output_declarations = css_parse_string(str, strlen(str), CssParserModeDeclarationList);
    
    css_destroy_output(css_dump_output(output_medialist));
    css_destroy_output(css_dump_output(output_rule_import));
    css_destroy_output(css_dump_output(output_rule_style));
    css_destroy_output(css_dump_output(output_value));
    css_destroy_output(css_dump_output(output_selectors));
    css_destroy_output(css_dump_output(output_declarations));
}
