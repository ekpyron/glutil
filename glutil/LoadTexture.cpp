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

#include "LoadTexture.h"
#include <cstring>
#include <iostream>

namespace glutil {

typedef struct {
	 uint8_t identifier[12];
	 uint32_t endianness;
	 uint32_t glType;
	 uint32_t glTypeSize;
	 uint32_t glFormat;
	 uint32_t glInternalFormat;
	 uint32_t glBaseInternalFormat;
	 uint32_t pixelWidth;
	 uint32_t pixelHeight;
	 uint32_t pixelDepth;
	 uint32_t numberOfArrayElements;
	 uint32_t numberOfFaces;
	 uint32_t numberOfMipmapLevels;
	 uint32_t bytesOfKeyValueData;
} ktx_header_t;

const uint8_t ktx_identifier[12] = {
	0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
};

GLint StringToWrapMode (const std::string &value)
{
	if (!value.compare ("CLAMP_TO_EDGE"))
		return GL_CLAMP_TO_EDGE;
	else if (!value.compare ("CLAMP_TO_BORDER"))
		return GL_CLAMP_TO_BORDER;
	else if (!value.compare ("MIRRORED_REPEAT"))
		return GL_MIRRORED_REPEAT;
	else if (!value.compare ("REPEAT"))
		return GL_REPEAT;
	else if (!value.compare ("MIRROR_CLAMP_TO_EDGE"))
		return GL_MIRROR_CLAMP_TO_EDGE;
	else throw std::runtime_error ("Unable to load texture: invalid texture wrap mode.");
}

gl::Texture LoadTexture (std::istream &stream)
{
	ktx_header_t header;
	if (!stream.read (reinterpret_cast<char*> (&header), sizeof (ktx_header_t)))
		throw std::runtime_error ("Unable to load texture: cannot read KTX header.");
	if (memcmp (header.identifier, ktx_identifier, 12) || header.endianness != 0x04030201)
		throw std::runtime_error ("Unable to load texture: invalid file format.");

	if (header.pixelDepth != 0 || header.numberOfArrayElements != 0
        || (header.numberOfFaces != 1 && header.numberOfFaces != 6))
		throw std::runtime_error ("Unable to load texture: format currently unsupported");

	gl::Texture texture ((header.numberOfFaces > 1) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);

	if (header.numberOfMipmapLevels == 0)
	{
        std::cerr << "Automatic mipmap generation currently doesn't work. Falling back to linear filtering." << std::endl;
		header.numberOfMipmapLevels = 1;
	}

    texture.Storage2D (header.numberOfMipmapLevels, header.glInternalFormat, header.pixelWidth, header.pixelHeight);
    if (header.numberOfFaces > 1) {
        texture.Parameter (GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        texture.Parameter (GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        texture.Parameter (GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    {
		int32_t read = 0;
		while (read < header.bytesOfKeyValueData)
		{
			uint32_t size = 0;
			std::vector<char> data;
			if (!stream.read (reinterpret_cast<char*> (&size), sizeof (uint32_t)))
				throw std::runtime_error ("Unable to load texture: cannot read key value data size.");
			data.resize (size + 1);
			if (!stream.read (data.data (), size))
				throw std::runtime_error ("Unable to load texture: cannot read key value data.");
			data[size] = 0;
			std::string key (data.data ());
			if (!key.compare ("WRAP_S"))
			{
				if (key.size () < size)
				{
					std::string value (&data[key.size () + 1]);
                    texture.Parameter (GL_TEXTURE_WRAP_S, StringToWrapMode (value));
				}
			}
			else if (!key.compare ("WRAP_T"))
			{
				if (key.size () < size)
				{
					std::string value (&data[key.size () + 1]);
                    texture.Parameter (GL_TEXTURE_WRAP_T, StringToWrapMode (value));
				}
			}
			else if (!key.compare ("WRAP_R"))
			{
				if (key.size () < size)
				{
					std::string value (&data[key.size () + 1]);
                    texture.Parameter (GL_TEXTURE_WRAP_R, StringToWrapMode (value));
				}
			}

			size_t skip = 3 - ((size + 3) % 4);
			if (skip) stream.ignore (skip);
			read += sizeof (uint32_t) + size + skip;
		}
	}

	gl::Buffer data;
	for (auto level = 0; level < header.numberOfMipmapLevels; level++)
	{
		uint32_t size;
		if (!stream.read (reinterpret_cast<char*> (&size), sizeof (uint32_t)))
			throw std::runtime_error ("Unable to load texture: cannot read block size");

		if (level == 0)
		{
			data.Storage (size, NULL, GL_MAP_WRITE_BIT | GL_CLIENT_STORAGE_BIT);
#ifndef NDEBUG
			data.Label ("Temporary texture data buffer.");
#endif
		}
		for (auto face = 0; face < header.numberOfFaces; face++)
		{
			void *ptr = data.MapRange (0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			stream.read (reinterpret_cast<char*> (ptr), size);
			data.Unmap ();
			if (!stream) throw std::runtime_error ("Unable to load texture: cannot read texture data");
			gl::MemoryBarrier (GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);

			data.Bind (GL_PIXEL_UNPACK_BUFFER);
			if (header.glType)
			{
                if (header.numberOfFaces > 1) {
                    texture.SubImage3D (level, 0, 0, 0, header.pixelWidth >> level, header.pixelHeight >> level, face,
                                        header.glFormat, header.glType, NULL);
                } else {
                    texture.SubImage2D (level, 0, 0, header.pixelWidth >> level, header.pixelHeight >> level,
                                        header.glFormat, header.glType, NULL);
                }
			}
			else
			{
                if (header.numberOfFaces > 1) {
                    texture.CompressedSubImage3D (level, 0, 0, 0, header.pixelWidth >> level,
                                                  header.pixelHeight >> level, header.glInternalFormat,
                                                  face, size, NULL);
                } else {
                    texture.CompressedSubImage2D (level, 0, 0, header.pixelWidth >> level,
                                                  header.pixelHeight >> level, header.glInternalFormat,
                                                  size, NULL);
                }
			}
			gl::Buffer::Unbind (GL_PIXEL_UNPACK_BUFFER);
			{
				uint32_t skip = ((size + 3) & ~3) - size;
				if (face < header.numberOfFaces - 1 && skip > 0)
				{
					stream.ignore (skip);
				}
			}
		}

		stream.ignore (3 - ((size + 3) % 4));
	}
	return texture;
}

} /* namespace glutil */
