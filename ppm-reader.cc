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

// Very simple ppm reader.

#include "ppm-reader.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *skipWhitespace(const char *buffer, const char *end) {
    for (;;) {
        while (buffer < end && isspace(*buffer)) ++buffer;
        if (buffer >= end) return nullptr;
        if (*buffer == '#') {
            while (buffer < end && *buffer != '\n') ++buffer; // EOL comment
            continue;  // Back to whitespace eating.
        }
        return buffer;
    }
}

// Read next number. Returns end of number on success or nullptr if no number
// could be read. If start is already nullptr, returns nullptr.
static const char* readNextNumber(const char *start, const char *end,
                                  int *result) {
    if (!start) return nullptr;
    const char *start_number = skipWhitespace(start, end);
    if (!start_number) return nullptr;
    char *end_number = nullptr;
    *result = strtol(start_number, &end_number, 10);
    if (end_number == start_number) return nullptr;
    return end_number;
}

const rgb_t *ReadImageData(const char *in_buffer, size_t buf_len,
                           struct ImageMetaInfo *info) {
    if (in_buffer[0] != 'P' || in_buffer[1] != '6' ||
        (!isspace(in_buffer[2]) && in_buffer[2] != '#')) {
        fprintf(stderr, "Only raw PPM files are supported (P6 header)\n");
        return nullptr;  // No P6 magic header.
    }
    const char *const end = in_buffer + buf_len;
    const char *parse_buffer = in_buffer + 2;  // Skipping P6 header
    parse_buffer = readNextNumber(parse_buffer, end, &info->width);
    parse_buffer = readNextNumber(parse_buffer, end, &info->height);
    parse_buffer = readNextNumber(parse_buffer, end, &info->range);
    if (!parse_buffer) return nullptr;
    if (!isspace(*parse_buffer++)) return nullptr;  // one space before data
    // Now make sure that the rest of the buffer still makes sense
    const size_t expected_image_data = info->width * info->height * 3;
    const size_t actual_data = end - parse_buffer;
    if (actual_data < expected_image_data)
        return nullptr;   // Uh, not enough data.
    return reinterpret_cast<const rgb_t*>(parse_buffer);
}
