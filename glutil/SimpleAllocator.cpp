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

#include "SimpleAllocator.h"
#include <stdexcept>

namespace glutil {

SimpleAllocator::SimpleAllocator (unsigned long size)
{
	root = new Chunk;
	root->size = size;
	root->status = true;
	root->left = nullptr;
	root->right = nullptr;
}

SimpleAllocator::~SimpleAllocator (void)
{
	Chunk *chunk = root;
	while (chunk != nullptr)
	{
		Chunk *next = chunk->right;
		delete chunk;
		chunk = next;
	}
}

void SimpleAllocator::AddMemory (unsigned long size)
{
	Chunk *last = root;
	while (last->right != nullptr) last = last->right;
	if (last->status)
	{
		last->size += size;
	}
	else
	{
		Chunk *newchunk = new Chunk;
		newchunk->left = last;
		newchunk->size = size;
		newchunk->status = true;
		newchunk->right = nullptr;
		last->right = newchunk;
	}
}

bool SimpleAllocator::IsEmpty (void) const
{
	return root->status && (root->left == nullptr) && (root->right == nullptr);
}

long SimpleAllocator::Alloc (unsigned long size, unsigned long alignment)
{
	if (size == 0)
		throw std::runtime_error ("Attempt to allocate a block with zero size.");

	unsigned long offset = 0;
	if (alignment == 0) alignment = 1;
	for (Chunk *chunk = root; chunk != nullptr; chunk = chunk->right)
	{
		unsigned long misalignment = (alignment - offset) % alignment;
		if (chunk->status && chunk->size >= size + misalignment)
		{
			chunk->status = false;
			if (misalignment == 0 && (chunk->size == size))
				return offset;

			if (misalignment)
			{
				// TODO: verify that alignment works correctly
				Chunk *newchunk = new Chunk;
				newchunk->size = misalignment;
				newchunk->status = true;
				newchunk->left = chunk->left;
				if (newchunk->left != nullptr)
					newchunk->left->right = newchunk;
				newchunk->right = chunk;
				chunk->left = newchunk;
				offset += misalignment;
				chunk->size -= misalignment;
			}

			Chunk *newchunk = new Chunk;
			newchunk->size = chunk->size - size;
			newchunk->status = true;
			newchunk->left = chunk;
			newchunk->right = chunk->right;
			if (newchunk->right != nullptr)
				newchunk->right->left = newchunk;
			chunk->right = newchunk;
			chunk->size = size;
			return offset;
		}
		offset += chunk->size;
	}

	return -1;
}

void SimpleAllocator::Free (unsigned long offset, unsigned long size)
{
	unsigned long position = 0;
	for (Chunk *chunk = root; chunk != nullptr; chunk = chunk->right)
	{
		if (offset == position)
		{
			if (chunk->size != size || chunk->status)
				break;

			chunk->status = true;

			while (chunk->left != nullptr && chunk->left->status)
			{
				Chunk *left = chunk->left;
				chunk->size += left->size;
				chunk->left = left->left;
				if (chunk->left != nullptr)
					chunk->left->right = chunk;
				if (left == root)
					root = chunk;
				delete left;
			}
			while (chunk->right != nullptr && chunk->right->status)
			{
				Chunk *right = chunk->right;
				chunk->size += right->size;
				chunk->right = right->right;
				if (chunk->right != nullptr)
					chunk->right->left = chunk;
				delete right;
			}

			return;
		}
		if (position > offset)
			break;
		position += chunk->size;
	}
	throw std::runtime_error ("Attempt to free invalid memory block.");
}

} /* namespace glutil */
