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

#include "CircularBuffer.h"

namespace glutil {

CircularBuffer::CircularBuffer (const GLsizeiptr &_size) : size (_size), head (0), fences { 0, 0, 0 }
{
    buffer.Storage (size * 3, NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    ptr = buffer.MapRange (0, size * 3, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

CircularBuffer::CircularBuffer (CircularBuffer &&b) : size (b.size), head (b.head), buffer (std::move (b.buffer)) {
    for (auto i = 0; i < 3; i++) {
        fences[i] = b.fences[i];
        b.fences[i] = 0;
    }
    b.size = 0; b.head = 0; b.ptr = nullptr;
}

CircularBuffer::~CircularBuffer (void)
{
    for (auto i = 0; i < 3; i++)
        if (fences[i] != 0)
            gl::DeleteSync (fences[i]);
}

CircularBuffer& CircularBuffer::operator=(CircularBuffer &&b)
{
    buffer = std::move (b.buffer);
    size = b.size; b.size = 0;
    head = b.head; b.head = 0;
    ptr = b.ptr; b.ptr = nullptr;
    for (auto i = 0; i < 3; i++) {
        fences[i] = b.fences[i];
        b.fences[i] = 0;
    }
    return *this;
}

void *CircularBuffer::GetPtr (void)
{
    if (fences[head] != 0) {
        gl::ClientWaitSync (fences[head], 0, GL_TIMEOUT_IGNORED);
        gl::DeleteSync (fences[head]);
        fences[head] = 0;
    }
    return reinterpret_cast<void*> (reinterpret_cast<intptr_t> (ptr) + head * size);
}

void CircularBuffer::BindBase (const GLenum &target, const GLuint &index) const
{
    buffer.BindRange (target, index, head * size, size);
}

void CircularBuffer::Bind (const GLenum &target) const
{
	buffer.Bind (target);
}

void CircularBuffer::Advance (void)
{
	if (fences[head]) gl::DeleteSync (fences[head]);
    fences[head] = gl::FenceSync (GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    head++;
    head %= 3;
}

} /* namespace glutil */
