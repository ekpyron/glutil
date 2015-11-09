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

#include "FullscreenQuadImpl.h"
#include "../LoadProgram.h"
#include <limits>

GLUTIL_IMPORT_SHADER(shader, fsquad)

namespace glutil {
namespace detail {

FullscreenQuadImpl::FullscreenQuadImpl(void)
{
	program.Parameter (GL_PROGRAM_SEPARABLE, GL_TRUE);

    LoadProgram (program, "FSQUAD", "", {
		{ "FSQUAD_VERTEX", GL_VERTEX_SHADER, shader::fsquad }
	});

	const GLshort data[] = { std::numeric_limits<short>::min (), std::numeric_limits<short>::min (),
							 std::numeric_limits<short>::max (), std::numeric_limits<short>::min (),
							 std::numeric_limits<short>::min (), std::numeric_limits<short>::max (),
							 std::numeric_limits<short>::max (), std::numeric_limits<short>::max () };
	buffer.Data (sizeof (data), data, GL_STATIC_DRAW);

	vao.AttribFormat (0, 2, GL_SHORT, GL_TRUE, 0);
	vao.AttribBinding (0, 0);
	vao.EnableAttrib (0);

	vao.VertexBuffer (0, buffer, 0, 2 * sizeof (GLshort));

#ifndef NDEBUG
	buffer.Label ("FullscreenQuad vertex buffer.");
#endif
}

FullscreenQuadImpl::~FullscreenQuadImpl(void)
{
}

void FullscreenQuadImpl::Render (void) const
{
    gl::Disable (GL_DEPTH_TEST);
    vao.Bind ();
    gl::DrawArrays (GL_TRIANGLE_STRIP, 0, 4);
    gl::Enable (GL_DEPTH_TEST);
}

} /* namespace detail */
} /* namespace glutil */
