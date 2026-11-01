#include "prx/libcairo/include/InventoryExports.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <type_traits>

struct _cairo_glyph {
    std::uint64_t index;
    double x;
    double y;
};
static_assert(sizeof(cairo_glyph_t) == 24);

using GlyphFree = void (APS5_VABI*)(cairo_glyph_t*);
static_assert(std::is_same_v<decltype(&cairo_glyph_free), GlyphFree>);

int main() {
    cairo_glyph_free(nullptr);

    auto* glyphs = static_cast<cairo_glyph_t*>(std::malloc(sizeof(cairo_glyph_t) * 3));
    if (glyphs == nullptr) {
        std::fputs("could not allocate test glyph array\n", stderr);
        return 1;
    }

    cairo_glyph_free(glyphs);
    std::puts("guest cairo glyph-free tests passed");
    return 0;
}
