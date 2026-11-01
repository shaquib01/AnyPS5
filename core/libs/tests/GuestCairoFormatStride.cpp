#include "prx/libcairo/include/InventoryExports.hpp"

#include <cstdint>
#include <cstdio>
#include <type_traits>

using StrideForWidth = int (APS5_VABI*)(cairo_format_t, int);
static_assert(std::is_same_v<decltype(&cairo_format_stride_for_width), StrideForWidth>);

struct TestCase {
    cairo_format_t format;
    int width;
    int expected;
};

int main() {
    constexpr TestCase cases[] = {
        {CAIRO_FORMAT_ARGB32, 1, 4},
        {CAIRO_FORMAT_RGB24, 3, 12},
        {CAIRO_FORMAT_A8, 1, 4},
        {CAIRO_FORMAT_A8, 5, 8},
        {CAIRO_FORMAT_A1, 1, 4},
        {CAIRO_FORMAT_A1, 33, 8},
        {CAIRO_FORMAT_RGB16_565, 3, 8},
        {CAIRO_FORMAT_RGB30, 2, 8},
        {CAIRO_FORMAT_RGB96F, 1, 12},
        {CAIRO_FORMAT_RGBA128F, 2, 32},
        {CAIRO_FORMAT_ARGB32, 0, 0},
    };

    for (const auto& test : cases) {
        const int actual = cairo_format_stride_for_width(test.format, test.width);
        if (actual != test.expected) {
            std::fprintf(stderr, "format %d width %d: expected %d, got %d\n",
                         static_cast<int>(test.format), test.width, test.expected, actual);
            return 1;
        }
    }

    if (cairo_format_stride_for_width(CAIRO_FORMAT_INVALID, 10) != -1 ||
        cairo_format_stride_for_width(static_cast<cairo_format_t>(99), 10) != -1 ||
        cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, -1) != -1 ||
        cairo_format_stride_for_width(CAIRO_FORMAT_RGBA128F, INT32_MAX) != -1) {
        std::fputs("invalid format or width was not rejected\n", stderr);
        return 1;
    }

    std::puts("guest cairo format-stride tests passed");
    return 0;
}
