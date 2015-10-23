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

#ifndef GLUTIL_DETAIL_FULLSCREENQUADIMPL_H
#define GLUTIL_DETAIL_FULLSCREENQUADIMPL_H

#include <oglp/oglp.h>
#include <memory>
#include <mutex>

namespace glutil {
namespace detail {

class FullscreenQuadImpl
{
public:
    static std::shared_ptr<FullscreenQuadImpl> GetInstance (void) {
        static std::weak_ptr<FullscreenQuadImpl> instance;
        static std::mutex mutex;
        const std::lock_guard<std::mutex> lock (mutex);
        std::shared_ptr<FullscreenQuadImpl> result = instance.lock ();
        if (!result) instance = (result = std::make_shared<FullscreenQuadImpl> ());
        return result;
    }
    FullscreenQuadImpl(void);
	FullscreenQuadImpl(const FullscreenQuadImpl &) = delete;
	~FullscreenQuadImpl(void);
	FullscreenQuadImpl &operator= (const FullscreenQuadImpl &) = delete;
	void Render (void) const;
    const gl::Program &GetVertexProgram (void) const {
		return program;
	}
private:
 	gl::Program program;
	gl::VertexArray vao;
	gl::Buffer buffer;
};

} /* namespace detail */
} /* namespace glutil */

#endif /* !defined GLUTIL_DETAIL_FULLSCREENQUADIMPL_H */
