#include "prx/libcairo/include/InventoryExports.hpp"

#include <cstdio>
#include <cstdlib>
#include <type_traits>

using GlyphFree = void (APS5_VABI*)(CairoGlyph*);
static_assert(std::is_same_v<decltype(&cairo_glyph_free), GlyphFree>);

int main() {
    cairo_glyph_free(nullptr);

    auto* glyphs = static_cast<CairoGlyph*>(std::malloc(sizeof(std::uint64_t) * 3 * 3));
    if (glyphs == nullptr) {
        std::fputs("could not allocate test glyph array\n", stderr);
        return 1;
    }

    cairo_glyph_free(glyphs);
    std::puts("guest cairo glyph-free tests passed");
    return 0;
}
