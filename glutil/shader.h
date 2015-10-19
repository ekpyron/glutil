/*
 * Copyright (c) 2015 Daniel Kirchner
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
 *
 */

#ifndef GLUTIL_SHADER_H
#define GLUTIL_SHADER_H

#include <stdint.h>

namespace shader {

typedef struct source
{
    uint32_t length;
    const uint8_t *data;
} source_t;

} /* namespace shader */

#define _GLUTIL_IMPORT_SHADER_1(_1) extern const struct source _1;
#define _GLUTIL_IMPORT_SHADER_2(_1,_2) namespace _1 { _GLUTIL_IMPORT_SHADER_1 (_2) }
#define _GLUTIL_IMPORT_SHADER_3(_1,_2,_3) namespace _1 { _GLUTIL_IMPORT_SHADER_2 (_2,_3) }
#define _GLUTIL_IMPORT_SHADER_4(_1,_2,_3,_4) namespace _1 { _GLUTIL_IMPORT_SHADER_3 (_2,_3,_4) }
#define _GLUTIL_IMPORT_SHADER_5(_1,_2,_3,_4,_5) namespace _1 { _GLUTIL_IMPORT_SHADER_4 (_2,_3,_4,_5) }
#define _GLUTIL_IMPORT_SHADER_6(_1,_2,_3,_4,_5,_6) namespace _1 { _GLUTIL_IMPORT_SHADER_5 (_2,_3,_4,_5,_6) }
#define _GLUTIL_IMPORT_SHADER_7(_1,_2,_3,_4,_5,_6,_7) namespace _1 { _GLUTIL_IMPORT_SHADER_6 (_2,_3,_4,_5,_6,_7) }
#define _GLUTIL_IMPORT_SHADER_8(_1,_2,_3,_4,_5,_6,_7,_8) namespace _1 { _GLUTIL_IMPORT_SHADER_7 (_2,_3,_4,_5,_6,_7,_8) }
#define _GLUTIL_IMPORT_SHADER_GETMACRO(_1,_2,_3,_4,_5,_6,_7,_8,NAME,...) NAME
#define GLUTIL_IMPORT_SHADER(...) namespace shader { _GLUTIL_IMPORT_SHADER_GETMACRO(__VA_ARGS__,\
_GLUTIL_IMPORT_SHADER_8, _GLUTIL_IMPORT_SHADER_7, _GLUTIL_IMPORT_SHADER_6, _GLUTIL_IMPORT_SHADER_5,\
_GLUTIL_IMPORT_SHADER_4, _GLUTIL_IMPORT_SHADER_3, _GLUTIL_IMPORT_SHADER_2, _GLUTIL_IMPORT_SHADER_1)(__VA_ARGS__) }

#endif /* !defined GLUTIL_SHADER_H */
