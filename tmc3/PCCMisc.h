/* The copyright in this software is being made available under the BSD
 * Licence, included below.  This software may be subject to other third
 * party and contributor rights, including patent rights, and no such
 * rights are granted under this licence.
 *
 * Copyright (c) 2017-2018, ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * * Neither the name of the ISO/IEC nor the names of its contributors
 *   may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PCCMisc_h
#define PCCMisc_h

#include <vector>

//#define PCC_UNDEFINED_INDEX (uint32_t(-1))

namespace pcc {
const uint32_t PCC_UNDEFINED_INDEX = -1;

enum PCCEndianness { PCC_BIG_ENDIAN = 0, PCC_LITTLE_ENDIAN = 1 };

struct PCCBitstream {
  uint8_t *buffer;
  uint64_t size;
  uint64_t capacity;
};

inline PCCEndianness PCCSystemEndianness() {
  uint32_t num = 1;
  return (*(reinterpret_cast<char *>(&num)) == 1) ? PCC_LITTLE_ENDIAN : PCC_BIG_ENDIAN;
}
template <typename T>
const T PCCEndianSwap(const T u) {
  union {
    T u;
    uint8_t u8[sizeof(T)];
  } source, dest;

  source.u = u;

  for (size_t k = 0; k < sizeof(T); k++) dest.u8[k] = source.u8[sizeof(T) - k - 1];

  return dest.u;
}
template <typename T>
const T PCCToLittleEndian(const T u) {
  return (PCCSystemEndianness() == PCC_BIG_ENDIAN) ? PCCEndianSwap(u) : u;
}
template <typename T>
const T PCCFromLittleEndian(const T u) {
  return (PCCSystemEndianness() == PCC_BIG_ENDIAN) ? PCCEndianSwap(u) : u;
}
template <typename T>
void PCCWriteToBuffer(const T u, uint8_t *const buffer, uint64_t &size) {
  union {
    T u;
    uint8_t u8[sizeof(T)];
  } source;
  source.u = u;
  if (PCCSystemEndianness() == PCC_LITTLE_ENDIAN) {
    for (size_t k = 0; k < sizeof(T); k++) {
      buffer[size++] = source.u8[k];
    }
  } else {
    for (size_t k = 0; k < sizeof(T); k++) {
      buffer[size++] = source.u8[sizeof(T) - k - 1];
    }
  }
}
template <typename T>
void PCCReadFromBuffer(const uint8_t *const buffer, T &u, uint64_t &size) {
  union {
    T u;
    uint8_t u8[sizeof(T)];
  } dest;

  if (PCCSystemEndianness() == PCC_LITTLE_ENDIAN) {
    for (size_t k = 0; k < sizeof(T); k++) {
      dest.u8[k] = buffer[size++];
    }
  } else {
    for (size_t k = 0; k < sizeof(T); k++) {
      dest.u8[sizeof(T) - k - 1] = buffer[size++];
    }
  }
  u = dest.u;
}

//---------------------------------------------------------------------------
// Population count -- return the number of bits set in @x.
//
static int popcnt(uint32_t x) {
  x = x - ((x >> 1) & 0x55555555u);
  x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
  return ((x + (x >> 4) & 0xF0F0F0Fu) * 0x1010101u) >> 24;
}

//---------------------------------------------------------------------------
// Round @x up to next power of two.
//
static uint32_t ceilpow2(uint32_t x) {
  x--;
  x = x | (x >> 1);
  x = x | (x >> 2);
  x = x | (x >> 4);
  x = x | (x >> 8);
  x = x | (x >> 16);
  return x+1;
}

//---------------------------------------------------------------------------
// Compute \left\floor \text{log}_2(x) \right\floor.
// NB: ilog2(0) = -1.

static int ilog2(uint32_t x) {
  x = ceilpow2(x + 1) - 1;
  return popcnt(x) - 1;
}

//---------------------------------------------------------------------------
// Compute \left\ceil \text{log}_2(x) \right\ceil.
// NB: ceillog2(0) = 32.

static int ceillog2(uint32_t x) {
  return ilog2(x-1) + 1;
}

//---------------------------------------------------------------------------

}

#endif /* PCCMisc_h */
