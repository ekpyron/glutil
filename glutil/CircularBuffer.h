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

#ifndef GLUTIL_CIRCULARBUFFER_H
#define GLUTIL_CIRCULARBUFFER_H

#include <oglp/oglp.h>

namespace glutil {

class CircularBuffer
{
public:
    CircularBuffer (const GLsizeiptr &size);
    CircularBuffer (CircularBuffer &&buffer);
    CircularBuffer (const CircularBuffer&) = delete;
    ~CircularBuffer (void);

    CircularBuffer &operator= (CircularBuffer&) = delete;
    CircularBuffer &operator= (CircularBuffer &&buffer);
    const int &GetHead (void) const {
        return head;
    }

    const gl::Buffer &get (void) const {
        return buffer;
    }

#ifndef NDEBUG
	void SetDebugLabel (const std::string &name) {
		buffer.Label (name);
	}
#endif

    void *GetPtr (void);
    void BindBase (const GLenum &target, const GLuint &index) const;
    void Bind (const GLenum &target) const;
    void Advance (void);
    const GLsizeiptr &GetSize (void) const {
        return size;
    }
private:
    gl::Buffer buffer;
    void *ptr;
    int head;
    GLsizeiptr size;
    GLsync fences[3];
};

} /* namespace glutil */

#endif /* !defined GLUTIL_CIRCULARBUFFER_H */
