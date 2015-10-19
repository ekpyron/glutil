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

#ifndef GLUTIL_CONTEXT_H
#define GLUTIL_CONTEXT_H

namespace glutil {

class FullscreenQuad;
class TextureManager;
class FramebufferManager;

class Context
{
public:
    Context (FullscreenQuad *fsquad, TextureManager *texturemanager,
             FramebufferManager *framebuffermanager) : programmanager_ (programmanager), fsquad_ (fsquad),
            texturemanager_ (texturemanager), framebuffermanager_ (framebuffermanager) {
    }

    FullscreenQuad *fsquad (void)  { return fsquad_; }
    TextureManager *texturemanager (void) { return texturemanager_; }
    FramebufferManager *framebuffermanager (void) { return framebuffermanager_; }
private:
    FullscreenQuad *fsquad_;
    TextureManager *texturemanager_;
    FramebufferManager *framebuffermanager_;
};

} /* namespace glutil */

#endif /* !defined GLUTIL_CONTEXT_H */
