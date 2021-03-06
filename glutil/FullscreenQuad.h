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

#ifndef GLUTIL_FULLSCREENQUAD_H
#define GLUTIL_FULLSCREENQUAD_H

#include <oglp/oglp.h>
#include <memory>

namespace glutil {

namespace detail {
class FullscreenQuadImpl;
} /* namespace detail */

class FullscreenQuad {
public:
    FullscreenQuad (void);
    FullscreenQuad (const FullscreenQuad &fsquad);
    FullscreenQuad (FullscreenQuad &&fsquad);
    ~FullscreenQuad (void);
    FullscreenQuad &operator= (const FullscreenQuad &fsquad);
    FullscreenQuad &operator= (FullscreenQuad &&fsquad) noexcept;

    void Render (void) const;
    const gl::Program &GetVertexProgram (void) const;
private:
    std::shared_ptr<detail::FullscreenQuadImpl> impl;
};

} /* namespace glutil */

#endif /* !defined GLUTIL_FULLSCREENQUAD_H */
