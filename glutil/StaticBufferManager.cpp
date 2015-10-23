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

#include "StaticBufferManager.h"

namespace glutil {

StaticBuffer::StaticBuffer (void)
	: parent (nullptr), offset (0), size (0)
{
}

StaticBuffer::StaticBuffer (StaticBufferManager *_parent, unsigned long _offset, unsigned long _size)
	: parent (_parent), offset (_offset), size (_size)
{
}

StaticBuffer::StaticBuffer (StaticBuffer &&buffer)
	: parent (buffer.parent), offset (buffer.offset), size (buffer.size)
{
	buffer.parent = nullptr;
	buffer.offset = 0;
	buffer.size = 0;
}

StaticBuffer::~StaticBuffer (void)
{
	if (parent != nullptr) {
		parent->Free (offset, size);
	}
}

StaticBuffer &StaticBuffer::operator= (StaticBuffer &&buffer) noexcept
{
	parent = buffer.parent; buffer.parent = nullptr;
	offset = buffer.offset; buffer.offset = 0;
	size = buffer.size; buffer.size = 0;
}

void StaticBuffer::SetData (gl::Buffer &src)
{
	if (parent == nullptr) throw std::runtime_error ("Attempt to set data of an unallocated buffer.");
	parent->SetData (offset, size, src);
}

void StaticBuffer::SetData (const void *data)
{
	gl::Buffer tmpbuffer;
	tmpbuffer.Storage (size, data, GL_CLIENT_STORAGE_BIT);
#ifndef NDEBUG
	tmpbuffer.Label ("Static buffer temporary copy buffer.");
#endif
	SetData (tmpbuffer);
}

StaticBufferManager::StaticBufferManager (unsigned long _blocksize, Allocator *_allocator)
	: blocksize (_blocksize), buffersize (0), allocator (_allocator)
{
}

StaticBufferManager::~StaticBufferManager (void)
{
}

StaticBufferManager::StaticBufferManager (StaticBufferManager &&manager)
		: blocksize (manager.blocksize), buffersize (manager.buffersize), allocator (std::move (manager.allocator)),
		  buffer (std::move (manager.buffer)) {
}

StaticBufferManager &StaticBufferManager::operator=(StaticBufferManager &&manager) {
	blocksize = manager.blocksize;
	buffersize = manager.buffersize;
	allocator = std::move (manager.allocator);
	buffer = std::move (manager.buffer);
	return *this;
}

StaticBuffer StaticBufferManager::Allocate (unsigned long size, unsigned long alignment)
{
	if (buffersize == 0) {
		buffersize = std::max (blocksize, size);
		allocator->AddMemory (buffersize);
		buffer.Storage (buffersize, NULL, 0);
#ifndef NDEBUG
		buffer.Label ("New static buffer manager buffer.");
#endif
	}
	long offset = allocator->Alloc (size);
	if (offset == -1)
	{
		unsigned long newsize = std::max (size + alignment, blocksize);
		allocator->AddMemory (newsize);
		gl::Buffer newbuffer;

		newbuffer.Storage (buffersize + newsize, NULL, 0);
		gl::Buffer::CopySubData (buffer, newbuffer, 0, 0, buffersize);

#ifndef NDEBUG
		newbuffer.Label ("New static buffer manager buffer.");
#endif

		buffer = std::move (newbuffer);
		buffersize += newsize;

		offset = allocator->Alloc (size);
		if (offset == -1) throw std::runtime_error ("Cannot allocate static buffer storage.");
	}

	return StaticBuffer (this, offset, size);
}

void StaticBufferManager::SetData (unsigned long offset, unsigned long size, gl::Buffer &src)
{
	gl::Buffer::CopySubData (src, buffer, 0, offset, size);
}


void StaticBufferManager::Free (unsigned long offset, unsigned long size)
{
	allocator->Free (offset, size);
}

} /* namespace glutil */
