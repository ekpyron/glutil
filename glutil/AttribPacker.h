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

#ifndef GLUTIL_ATTRIBPACKER_H
#define GLUTIL_ATTRIBPACKER_H

#include <glm/glm.hpp>
#include <vector>

namespace glutil {

class AttribPacker {
public:
    AttribPacker (void);
    ~AttribPacker (void);
    template<typename T>
    AttribPacker &operator<< (const T &t) {
        pack (reinterpret_cast<uint8_t*> (&t), sizeof (t));
        return *this;
    }
    void *GetData (void);
    size_t GetSize (void);
private:
    void pack (uint8_t *ptr, size_t size);
    std::vector<uint8_t> data;
};

} /* namespace glutil */

#endif /* !defined GLUTIL_ATTRIBPACKER_H */
