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

#ifndef GLUTIL_ATTRIBPACKER_H
#define GLUTIL_ATTRIBPACKER_H

#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>
#include <initializer_list>

namespace glutil {

struct INT_2_10_10_10_REV {
    INT_2_10_10_10_REV (int _r, int _g, int _b, int _a) : r (_r), g (_g), b (_b), a (_a) {
    }
    INT_2_10_10_10_REV (glm::vec3 v) : r (v.x * 511), g (v.y * 511), b (v.z * 511), a (0) {
    }
    INT_2_10_10_10_REV (glm::vec4 v) : r (v.x * 511), g (v.y * 511), b (v.z * 511), a (v.w) {
    }
    union {
        int value;
        struct {
            int a : 2;
            int b : 10;
            int g : 10;
            int r : 10;
        };
    };
};

struct UNSIGNED_INT_2_10_10_10_REV {
    UNSIGNED_INT_2_10_10_10_REV (unsigned int _r, unsigned int _g, unsigned int _b, unsigned int _a)
            : r (_r), g (_g), b (_b), a (_a) {
    }
    UNSIGNED_INT_2_10_10_10_REV (glm::vec3 v) : r (v.x * 1023), g (v.y * 1023), b (v.z * 1023), a (0) {
    }
    UNSIGNED_INT_2_10_10_10_REV (glm::vec4 v) : r (v.x * 1023), g (v.y * 1023), b (v.z * 1023), a (v.w * 3) {
    }
    union {
        unsigned int value;
        struct {
            unsigned int a : 2;
            unsigned int b : 10;
            unsigned int g : 10;
            unsigned int r : 10;
        };
    };
};

class AttribPacker {
public:
    AttribPacker (std::initializer_list<size_t> _sizes) : stride (0), current (0) {
        offsets.reserve (_sizes.size () + 1);
        for (auto &size : _sizes) {
            offsets.push_back (stride);
            stride += size;
        }
        offsets.push_back (stride);
    }
    ~AttribPacker (void) {
    }
    void *GetData (void) {
        return data.data ();
    }
    size_t GetSize (void) {
        return data.size ();
    }
    size_t GetCount (void) {
        return data.size () / stride;
    }
    template<typename U>
    AttribPacker &operator<< (const U &u) {
        if (offsets[current + 1] - offsets[current] != sizeof (U))
            throw std::runtime_error ("invalid attrib size");
        const uint8_t *ptr = reinterpret_cast<const uint8_t*> (&u);
        data.insert (data.end (), ptr, ptr + sizeof (U));
        current++;
        current %= (offsets.size () - 1);
        return *this;
    };

    const size_t &GetStride (void) {
        return stride;
    }
    const size_t &GetOffset (size_t i) const {
        return offsets[i];
    }
private:
    size_t stride;
    size_t current;
    std::vector<size_t> offsets;
    std::vector<uint8_t> data;
};

} /* namespace glutil */

#endif /* !defined GLUTIL_ATTRIBPACKER_H */
