/*
 * Copyright (c) 2020-2022 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#include "bn_color_effect.h"

#include "bn_span.h"
#include "bn_math.h"
#include "bn_fixed.h"
#include "bn_memory.h"
#include "bn_alignment.h"
#include "../hw/include/bn_hw_palettes.h"

namespace bn::color_effect
{

void brightness(fixed brightness, span<color> colors_ref)
{
    int colors_count = colors_ref.size();
    BN_ASSERT(colors_count > 0, "Invalid colors count: ", colors_count);
    BN_ASSERT(brightness >= 0 && brightness <= 1, "Invalid brightness: ", brightness);

    if(int value = fixed_t<5>(brightness).data())
    {
        hw::palettes::brightness(colors_ref.data(), value, colors_count, colors_ref.data());
    }
}

void brightness(const span<const color>& source_colors_ref, fixed brightness, span<color> destination_colors_ref)
{
    int colors_count = source_colors_ref.size();
    BN_ASSERT(colors_count > 0, "Invalid source colors count: ", colors_count);
    BN_ASSERT(destination_colors_ref.size() >= colors_count, "Invalid destination colors count: ",
              destination_colors_ref.size(), " - ", colors_count);
    BN_ASSERT(brightness >= 0 && brightness <= 1, "Invalid brightness: ", brightness);

    int value = fixed_t<5>(brightness).data();
    hw::palettes::brightness(source_colors_ref.data(), value, colors_count, destination_colors_ref.data());
}

void contrast(fixed contrast, span<color> colors_ref)
{
    int colors_count = colors_ref.size();
    BN_ASSERT(colors_count > 0, "Invalid colors count: ", colors_count);
    BN_ASSERT(contrast >= 0 && contrast <= 1, "Invalid contrast: ", contrast);

    if(int value = fixed_t<5>(contrast).data())
    {
        hw::palettes::contrast(colors_ref.data(), value, colors_count, colors_ref.data());
    }
}

void contrast(const span<const color>& source_colors_ref, fixed contrast, span<color> destination_colors_ref)
{
    int colors_count = source_colors_ref.size();
    BN_ASSERT(colors_count > 0, "Invalid source colors count: ", colors_count);
    BN_ASSERT(destination_colors_ref.size() >= colors_count, "Invalid destination colors count: ",
              destination_colors_ref.size(), " - ", colors_count);
    BN_ASSERT(contrast >= 0 && contrast <= 1, "Invalid contrast: ", contrast);

    int value = fixed_t<5>(contrast).data();
    hw::palettes::contrast(source_colors_ref.data(), value, colors_count, destination_colors_ref.data());
}

void intensity(fixed intensity, span<color> colors_ref)
{
    int colors_count = colors_ref.size();
    BN_ASSERT(colors_count > 0, "Invalid colors count: ", colors_count);
    BN_ASSERT(intensity >= 0 && intensity <= 1, "Invalid intensity: ", intensity);

    if(int value = fixed_t<5>(intensity).data())
    {
        hw::palettes::intensity(colors_ref.data(), value, colors_count, colors_ref.data());
    }
}

void intensity(const span<const color>& source_colors_ref, fixed intensity, span<color> destination_colors_ref)
{
    int colors_count = source_colors_ref.size();
    BN_ASSERT(colors_count > 0, "Invalid source colors count: ", colors_count);
    BN_ASSERT(destination_colors_ref.size() >= colors_count, "Invalid destination colors count: ",
              destination_colors_ref.size(), " - ", colors_count);
    BN_ASSERT(intensity >= 0 && intensity <= 1, "Invalid intensity: ", intensity);

    int value = fixed_t<5>(intensity).data();
    hw::palettes::intensity(source_colors_ref.data(), value, colors_count, destination_colors_ref.data());
}

void invert(span<color> colors_ref)
{
    int colors_count = colors_ref.size();
    BN_ASSERT(colors_count > 0, "Invalid colors count: ", colors_count);

    color* colors_data = colors_ref.data();

    if(colors_count % 2 == 0 && aligned<sizeof(int)>(colors_data))
    {
        hw::palettes::aligned_invert(colors_data, colors_count, colors_data);
    }
    else
    {
        hw::palettes::invert(colors_data, colors_count, colors_data);
    }
}

void invert(const span<const color>& source_colors_ref, span<color> destination_colors_ref)
{
    int colors_count = source_colors_ref.size();
    BN_ASSERT(colors_count > 0, "Invalid source colors count: ", colors_count);
    BN_ASSERT(destination_colors_ref.size() >= colors_count, "Invalid destination colors count: ",
              destination_colors_ref.size(), " - ", colors_count);

    const color* source_colors_data = source_colors_ref.data();
    color* destination_colors_data = destination_colors_ref.data();

    if(colors_count % 2 == 0 && aligned<sizeof(int)>(source_colors_data) &&
            aligned<sizeof(int)>(destination_colors_data))
    {
        hw::palettes::aligned_invert(source_colors_data, colors_count, destination_colors_data);
    }
    else
    {
        hw::palettes::invert(source_colors_data, colors_count, destination_colors_data);
    }
}

void grayscale(fixed grayscale_intensity, span<color> colors_ref)
{
    int colors_count = colors_ref.size();
    BN_ASSERT(colors_count > 0 && colors_count <= hw::palettes::colors(),
              "Invalid colors count: ", colors_count);
    BN_ASSERT(grayscale_intensity >= 0 && grayscale_intensity <= 1,
              "Invalid grayscale intensity: ", grayscale_intensity);

    if(int value = fixed_t<5>(grayscale_intensity).data())
    {
        hw::palettes::grayscale(colors_ref.data(), value, colors_count, colors_ref.data());
    }
}

void grayscale(const span<const color>& source_colors_ref, fixed grayscale_intensity,
               span<color> destination_colors_ref)
{
    int colors_count = source_colors_ref.size();
    BN_ASSERT(colors_count > 0 && colors_count <= hw::palettes::colors(),
              "Invalid source colors count: ", colors_count);
    BN_ASSERT(destination_colors_ref.size() >= colors_count, "Invalid destination colors count: ",
              destination_colors_ref.size(), " - ", colors_count);
    BN_ASSERT(grayscale_intensity >= 0 && grayscale_intensity <= 1,
              "Invalid grayscale intensity: ", grayscale_intensity);

    int value = fixed_t<5>(grayscale_intensity).data();
    hw::palettes::grayscale(source_colors_ref.data(), value, colors_count, destination_colors_ref.data());
}

void hue_shift(fixed hue_shift_intensity, span<color> colors_ref)
{
    int colors_count = colors_ref.size();
    BN_ASSERT(colors_count > 0 && colors_count <= hw::palettes::colors(),
              "Invalid colors count: ", colors_count);
    BN_ASSERT(hue_shift_intensity >= 0 && hue_shift_intensity <= 1,
              "Invalid hue shift intensity: ", hue_shift_intensity);

    if(int value = fixed_t<5>(hue_shift_intensity).data())
    {
        hw::palettes::hue_shift(colors_ref.data(), value, colors_count, colors_ref.data());
    }
}

void hue_shift(const span<const color>& source_colors_ref, fixed hue_shift_intensity,
               span<color> destination_colors_ref)
{
    int colors_count = source_colors_ref.size();
    BN_ASSERT(colors_count > 0 && colors_count <= hw::palettes::colors(),
              "Invalid source colors count: ", colors_count);
    BN_ASSERT(destination_colors_ref.size() >= colors_count, "Invalid destination colors count: ",
              destination_colors_ref.size(), " - ", colors_count);
    BN_ASSERT(hue_shift_intensity >= 0 && hue_shift_intensity <= 1,
              "Invalid hue shift intensity: ", hue_shift_intensity);

    int value = fixed_t<5>(hue_shift_intensity).data();
    hw::palettes::hue_shift(source_colors_ref.data(), value, colors_count, destination_colors_ref.data());
}

void fade(color fade_color, fixed fade_intensity, span<color> colors_ref)
{
    int colors_count = colors_ref.size();
    BN_ASSERT(colors_count > 0, "Invalid colors count: ", colors_count);
    BN_ASSERT(fade_intensity >= 0 && fade_intensity <= 1, "Invalid fade intensity: ", fade_intensity);

    if(int value = fixed_t<5>(fade_intensity).data())
    {
        hw::palettes::fade(colors_ref.data(), fade_color, value, colors_count, colors_ref.data());
    }
}

void fade(const span<const color>& source_colors_ref, color fade_color, fixed fade_intensity,
          span<color> destination_colors_ref)
{
    int colors_count = source_colors_ref.size();
    BN_ASSERT(colors_count > 0, "Invalid source colors count: ", colors_count);
    BN_ASSERT(destination_colors_ref.size() >= colors_count, "Invalid destination colors count: ",
              destination_colors_ref.size(), " - ", colors_count);
    BN_ASSERT(fade_intensity >= 0 && fade_intensity <= 1, "Invalid fade intensity: ", fade_intensity);

    int value = fixed_t<5>(fade_intensity).data();
    hw::palettes::fade(source_colors_ref.data(), fade_color, value, colors_count, destination_colors_ref.data());
}

void rotate(int rotate_count, span<color> colors_ref)
{
    int colors_count = colors_ref.size();
    BN_ASSERT(colors_count > 0 && colors_count <= hw::palettes::colors(),
              "Invalid colors count: ", colors_count);
    BN_ASSERT(abs(rotate_count) < colors_count, "Invalid rotate count: ", rotate_count, " - ", colors_count);

    if(rotate_count)
    {
        color* colors_data = colors_ref.data();
        auto uint16_colors_data = reinterpret_cast<uint16_t*>(colors_data);
        uint16_t uint16_buffer[hw::palettes::colors()];
        memory::copy(*uint16_colors_data, colors_count, *uint16_buffer);

        auto color_buffer_ptr = reinterpret_cast<const color*>(uint16_buffer);
        hw::palettes::rotate(color_buffer_ptr, rotate_count, colors_count, colors_data);
    }
}

void rotate(const span<const color>& source_colors_ref, int rotate_count, span<color> destination_colors_ref)
{
    int colors_count = source_colors_ref.size();
    BN_ASSERT(colors_count > 0 && colors_count <= hw::palettes::colors(),
              "Invalid source colors count: ", colors_count);
    BN_ASSERT(destination_colors_ref.size() >= colors_count, "Invalid destination colors count: ",
              destination_colors_ref.size(), " - ", colors_count);
    BN_ASSERT(abs(rotate_count) < colors_count, "Invalid rotate count: ", rotate_count, " - ", colors_count);

    hw::palettes::rotate(source_colors_ref.data(), rotate_count, colors_count, destination_colors_ref.data());
}

}
