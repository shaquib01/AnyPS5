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


    // Sweep ordinary widths for every supported format against an independent
    // bit-count reference, including widths that require row padding.
    constexpr struct {
        cairo_format_t format;
        std::uint32_t bits_per_pixel;
    } formats[] = {
        {CAIRO_FORMAT_ARGB32, 32},
        {CAIRO_FORMAT_RGB24, 32},
        {CAIRO_FORMAT_A8, 8},
        {CAIRO_FORMAT_A1, 1},
        {CAIRO_FORMAT_RGB16_565, 16},
        {CAIRO_FORMAT_RGB30, 32},
        {CAIRO_FORMAT_RGB96F, 96},
        {CAIRO_FORMAT_RGBA128F, 128},
    };

    for (const auto& format : formats) {
        for (std::uint32_t width = 0; width <= 1024; ++width) {
            const std::uint64_t row_bytes =
                (static_cast<std::uint64_t>(width) * format.bits_per_pixel + 7u) / 8u;
            const int expected = static_cast<int>((row_bytes + 3u) & ~std::uint64_t{3});
            const int actual = cairo_format_stride_for_width(
                format.format, static_cast<int>(width));
            if (actual != expected) {
                std::fprintf(stderr, "format %d width %u: expected %d, got %d\n",
                             static_cast<int>(format.format), width, expected, actual);
                return 1;
            }
        }

        const std::uint32_t first_rejected_width =
            static_cast<std::uint32_t>((INT32_MAX - 7) / format.bits_per_pixel);
        if (cairo_format_stride_for_width(
                format.format, static_cast<int>(first_rejected_width)) != -1) {
            std::fprintf(stderr, "format %d did not reject overflow boundary\n",
                         static_cast<int>(format.format));
            return 1;
        }
    }

    std::puts("guest cairo format-stride tests passed");
    return 0;
}
