// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
// (c) 2015 Henner Zeller <h.zeller@acm.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 2.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://gnu.org/licenses/gpl-2.0.txt>

#include <stdlib.h>
#include <stdint.h>

struct rgb_t {
    uint8_t r, g, b;
} __attribute__((packed));

struct ImageMetaInfo {
    int width;
    int height;
    int range;  // Range of gray-levels. We only handle 255 correctly(=1byte)
};

// Given an input buffer + size with a raw PPM file, extract the image
// meta information and return it in "info_out".
// Return pointer to raw image data with uint8 [r,g,b] tuples (memory is
// still owned by "in_buffer".
// Returns nullptr if reading failed.
const rgb_t *ReadImageData(const char *in_buffer, size_t buf_len,
                           struct ImageMetaInfo *info_out);
