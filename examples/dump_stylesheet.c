//
//  dump.c
//  CssParser
//
//  Created by QFish on 4/6/15.
//  Copyright (c) 2015 QFish. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>

#include "cssparser.h"

#define StopWatchBegin(begin) struct timeb t1; ftime(&t1);
#define StopWatchEnd(begin) struct timeb t2; ftime(&t2); printf("<" #begin "> costs %dms.\n", (t2.millitm - t1.millitm));

void dump_stylesheet(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("File %s not found!\n", filename);
        exit(0);
    }
    StopWatchBegin(CssParseFile);
    CssOutput* output = css_parse_file(fp);
    StopWatchEnd(CssParseFile);
    css_dump_output(output);
    css_destroy_output(output);
}

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        printf("Usage: css_dump <CSS filename>.\n");
        exit(0);
    }
    dump_stylesheet(argv[1]);
    return 0;
}