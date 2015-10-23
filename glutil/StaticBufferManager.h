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

#ifndef GLUTIL_STATICBUFFERMANAGER_H
#define GLUTIL_STATICBUFFERMANAGER_H

#include <oglp/oglp.h>
#include <memory>
#include "Allocator.h"
#include "SimpleAllocator.h"

namespace glutil {

class StaticBufferManager;

class StaticBuffer
{
public:
	StaticBuffer (void);
	StaticBuffer (const StaticBuffer&) = delete;
	StaticBuffer (StaticBuffer &&buffer);
	~StaticBuffer (void);
	StaticBuffer &operator= (const StaticBuffer&) = delete;
	StaticBuffer &operator= (StaticBuffer &&buffer) noexcept;
	void SetData (gl::Buffer &src);
	void SetData (const void *src);
	const unsigned long &GetOffset (void) const {
		return offset;
	}
	const unsigned long &GetSize (void) const {
		return size;
	}
	operator bool (void) const {
		return parent != nullptr;
	}
private:
	StaticBuffer (StaticBufferManager *parent, unsigned long offset, unsigned long size);
	unsigned long offset;
	unsigned long size;
	StaticBufferManager *parent;
	friend class StaticBufferManager;
};

class StaticBufferManager
{
public:
	template<typename Allocator = SimpleAllocator, typename... Args>
	StaticBufferManager (unsigned long blocksize, Args... args) : StaticBufferManager (blocksize, new Allocator (args...)) {
	}
	StaticBufferManager (const StaticBufferManager&) = delete;
	StaticBufferManager (StaticBufferManager &&manager);
	~StaticBufferManager (void);

	StaticBufferManager &operator= (const StaticBufferManager&) = delete;
	StaticBufferManager &operator= (StaticBufferManager &&manager);
	StaticBuffer Allocate (unsigned long size, unsigned long alignment = 0);

	const gl::Buffer &GetBuffer (void) const {
		return buffer;
	}

	const unsigned long &GetSize (void) const {
		return buffersize;
	}

#ifndef NDEBUG
	void SetDebugLabel (const std::string &name) {
		buffer.Label (name);
	}
#endif
private:
	StaticBufferManager (unsigned long blocksize, Allocator *allocator);
	void Free (unsigned long offset, unsigned long size);
	void SetData (unsigned long offset, unsigned long size, gl::Buffer &src);
	gl::Buffer buffer;
	std::unique_ptr<Allocator> allocator;
	unsigned long buffersize;
	unsigned long blocksize;
	friend class StaticBuffer;
};

} /* namespace glutil */

#endif /* !defined GLUTIL_STATICBUFFERMANAGER_H */
