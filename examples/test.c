//
//  test.c
//  CssParser
//
//  Created by cheungmine
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>

#include "cssparser.h"

#include "win32/mscrtdbg.h"

const char cssstr[] = "hr {color: sienna;}"
"p {margin-left: 20px;color:red;}"
"body {"
"    background-image: url(\"images/back40.gif\");"
"}"
"div {    color:black;}";


void dump_stylesheet(CssStylesheet *sheet);

void dump_stylerule(CssStyleRule *rule);

void dump_properties(CssArray *props);


int main(int argc, const char * argv[])
{
    WINDOWS_CRTDBG_ON

    CssOutput* output = NULL;

    if (argc == 2) {
        printf("css_parse_file: %s\n", argv[1]);
        FILE *fp = fopen(argv[1], "rb");
        if (fp) {
            output = css_parse_file(fp);
            fclose(fp);
        }
    } else {
        printf("css_parse_string:\n%s\n", cssstr);
        output = css_parse_string(cssstr, strlen(cssstr), CssParserModeStylesheet);
    }

    if (output) {
        if (output->stylesheet) {
            printf("dump_stylesheet\n");
            dump_stylesheet(output->stylesheet);
        } else {
            printf("error stylesheet!\n");
        }

        css_destroy_output(output);
    } else {
        printf("error!\n");
    }

    return 0;
}


void dump_stylesheet(CssStylesheet *sheet)
{
    printf("sheet imports:%d\n", sheet->imports.length);
    printf("sheet rules:%d\n", sheet->rules.length);

    for (unsigned i = 0; i < sheet->rules.length; ++i) {
        CssRule *rule = (CssRule *) sheet->rules.data[i];

        if (rule && rule->type == CssRuleStyle) {
            printf("dump_stylerule:\n");
            dump_stylerule((CssStyleRule*)rule);            
        }
    }
}


void dump_stylerule(CssStyleRule *rule)
{
    unsigned i;

    for (i = 0; i < rule->selectors->length; ++i) {
        CssSelector *sel = (CssSelector *)rule->selectors->data[i];

        switch (sel->match) {
        case CssSelMatchTag:
            printf("%s {\n", (sel->tag)? sel->tag->local : "UNNAMED");
            if (rule->declarations) {
                dump_properties(rule->declarations);
            }
            printf("}\n");
            break;
        case CssSelMatchId:
            break;
        case CssSelMatchClass:
            printf("Processing class selector: %s\n", (sel->data) ? sel->data->value : "UNNAMED");
            break;
        }
    }
}


void dump_properties(CssArray *props)
{
    unsigned i, j;

    for (i = 0; i < props->length; ++i) {
        CssDeclaration * prop = (CssDeclaration *)props->data[i];
        if (prop) {
            printf("    %s:", prop->property);

            if (prop->values) {
                for (j = 0; j < prop->values->length; j++) {
                    CssValue *value = (CssValue *)prop->values->data[j];
                    switch (value->unit) {
                    case CSS_VALUE_NUMBER:
                    case CSS_VALUE_PERCENTAGE:
                    case CSS_VALUE_EMS:
                    case CSS_VALUE_EXS:
                    case CSS_VALUE_REMS:
                    case CSS_VALUE_CHS:
                    case CSS_VALUE_PX:
                    case CSS_VALUE_CM:
                    case CSS_VALUE_DPPX:
                    case CSS_VALUE_DPI:
                    case CSS_VALUE_DPCM:
                    case CSS_VALUE_MM:
                    case CSS_VALUE_IN:
                    case CSS_VALUE_PT:
                    case CSS_VALUE_PC:
                    case CSS_VALUE_DEG:
                    case CSS_VALUE_RAD:
                    case CSS_VALUE_GRAD:
                    case CSS_VALUE_MS:
                    case CSS_VALUE_S:
                    case CSS_VALUE_HZ:
                    case CSS_VALUE_KHZ:
                    case CSS_VALUE_TURN:
                        printf("%s;\n", value->raw);
                        break;

                    case CSS_VALUE_IDENT:
                    case CSS_VALUE_STRING:
                        printf("%s;\n", value->string);
                        break;

                    case CSS_VALUE_PARSER_HEXCOLOR:
                        printf("#%s;\n", value->string);
                        break;

                    case CSS_VALUE_URI:
                        printf("url(%s);\n", value->string);
                        break;

                    default:
                        printf("CSSPARSER: Unknown Value Unit(%d).", value->unit);
                        break;
                    }
                }
            }
        }
    }
}