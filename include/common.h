#ifndef COMMON_H_
#define COMMON_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#define true 1
#define false 0
extern int errorState;

typedef unsigned bool;
#define kMaxLen 32
#define STREQ(x, y) (strcmp((x), (y)) == 0)
#endif
