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

#include "LoadProgram.h"
#include "lz4.h"

namespace glutil {

shaderdesc::shaderdesc (const std::string &_name, const GLenum &_type, const shadersource_t &_source)
        : name (_name), type (_type)
{
    std::vector<char> src;
    src.resize (_source.length);
    if (LZ4_decompress_fast (reinterpret_cast<const char*> (_source.data), &src[0], _source.length) < 0)
        throw std::runtime_error (std::string ("Failed to load shader ") + name + ": invalid lz4 stream.");
    source = std::string (src.data (), _source.length);
    if (_source.version) version = std::string (_source.version);
}

shaderdesc::shaderdesc (const std::string &_name, const GLenum &_type, const std::string &_source, const std::string &_version)
        : name (_name), type (_type), source (_source), version (_version)
{
}

void LoadProgram (gl::Program &program, const std::string &name, const std::string &definitions,
        const std::initializer_list<shaderdesc_t> &shaders)
{
    std::vector<gl::Shader> shaderobjs;
    shaderobjs.reserve (shaders.size ());
    for (auto &shader : shaders) {
        shaderobjs.emplace_back (shader.type);
        std::vector<std::string> sources;
        if (!shader.version.empty ()) sources.push_back ("#version " + shader.version + "\n");
        if (!definitions.empty ()) sources.push_back (definitions);
        sources.push_back (shader.source);
        shaderobjs.back ().Source (sources);

        if (!shaderobjs.back ().Compile ())
            throw std::runtime_error ("Failed to compile shader " + shader.name + ": " + shaderobjs.back ().GetInfoLog ());

        program.Attach (shaderobjs.back ());
    }

    if (!program.Link ())
        throw std::runtime_error ("Failed to link program " + name + ": " + program.GetInfoLog() + ".");

    for (auto &obj : shaderobjs) {
        program.Detach (obj);
    }
}

} /* namespace glutil */
