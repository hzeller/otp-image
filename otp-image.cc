// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
// (c) 2022 Henner Zeller <h.zeller@acm.org>
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

#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "ppm-reader.h"

#include "bit-cut-template.ps.rawstring"

int usage(const char *prog, const char *message = nullptr) {
    if (message) fprintf(stderr, "== %s ==\n", message);
    fprintf(stderr, "Usage:\n%s <key> <ppm-file>\n", prog);
    fprintf(stderr, "Reasonable image size is somewhere around <= 60x40px\n");
    return 1;
}

// Create a sequence of random-ish bits; repeating after Reset().
// (This is demo, so _not_ cryptographically sound)
class KeySequence {
public:
    KeySequence(const std::string &key) : is_empty_(key.empty()),
                                          seed_(std::hash<std::string>{}(key)) {
        if (is_empty_) fprintf(stderr, "== empty key! ==\n");
    }

    void Reset() { srand(seed_); }
    bool NextBool() { return is_empty_ ? 0 : rand() % 2;  }

private:
    const bool is_empty_;
    const size_t seed_;
};

// Convert mm to PostScript points
int operator "" _mm(unsigned long long x) { return roundf(72.0 * x / 25.4); }

static constexpr bool kInvert = true;  // TODO: make flag.

int main(int argc, char **argv) {
    if (argc < 2) return usage(argv[0]);
    FILE *out = stdout;  // TODO: make flag

    KeySequence key_sequence(argv[1]);

    const char *filename = argv[2];
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());
    ImageMetaInfo meta;
    const rgb_t *image = ReadImageData(content.data(), content.size(), &meta);
    if (!image) {
        return usage(argv[0], "Could not read image.");
    }

    // Derive bounding box (also see /dx,/dy,/border... constants in PS).
    const int dx = 4_mm;
    const int dy = 4_mm;
    const int border = 5_mm;
    const int bb_width = 2 * border + dx * meta.width;
    const int bb_height = 2 * border + 10_mm + dy * meta.height;
    fprintf(out, "%%!PS-Adobe-2.0\n"
            "%%%%BoundingBox: 0 0 %d %d\n", bb_width + 50, bb_height + 50);
    fprintf(out, "\n/top-edge %d def\n", bb_height);
    fwrite(kbit_cut_template_ps, sizeof(kbit_cut_template_ps) - 1, 1, out);

    fprintf(out, "\n\n%% Image size\n"
            "/width %d def\n/height %d def\n\n", meta.width, meta.height);

    fprintf(out, "%s %d\n", "%%Pages:", 2);
    for (int p = 0; p < 2; ++p) {
        fprintf(out, "%s %s %d\n", "%%Page:", p == 0 ? "Key" : "Image", p + 1);
        key_sequence.Reset();
        for (int y = 0; y < meta.height; ++y) {
            fprintf(out, y == 0 ? "/data [" : "\n       ");
            for (int x = 0; x < meta.width; ++x) {
                bool bit = key_sequence.NextBool();
                if (p == 1) {  // The 'image' part.
                    bit ^= (image[y * meta.width + x].r < 128) ^ kInvert;
                }
                fprintf(out, " %d", bit);
            }
        }
        fprintf(out, "\n] def\n\n");

        fprintf(out, "page-setup cut-array page-end\nshowpage\n\n");
    }

    fprintf(out, "%s", "%%EOF\n");
    fclose(out);
}
