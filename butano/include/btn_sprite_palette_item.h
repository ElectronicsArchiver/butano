/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BTN_SPRITE_PALETTE_ITEM_H
#define BTN_SPRITE_PALETTE_ITEM_H

#include "btn_span.h"
#include "btn_color.h"
#include "btn_optional_fwd.h"
#include "btn_palette_bpp_mode.h"

namespace btn
{

class sprite_palette_ptr;

/**
 * @brief Contains the required information to generate sprite color palettes.
 *
 * The assets conversion tools generate an object of this type in the build folder for each *.bmp file.
 *
 * The colors are not copied but referenced, so they should outlive sprite_palette_item to avoid dangling references.
 *
 * @ingroup sprite
 * @ingroup palette
 * @ingroup tool
 */
class sprite_palette_item
{

public:
    /**
     * @brief Constructor.
     * @param colors Reference to an array of multiples of 16 colors.
     *
     * The colors are not copied but referenced, so they should outlive sprite_palette_item to avoid dangling references.
     *
     * @param bpp_mode Bits per pixel of the color palettes to create.
     */
    constexpr sprite_palette_item(const span<const color>& colors, palette_bpp_mode bpp_mode) :
        _colors(colors),
        _bpp_mode(bpp_mode)
    {
        BTN_ASSERT((bpp_mode == palette_bpp_mode::BPP_4 && _colors.size() == 16) ||
                   (bpp_mode == palette_bpp_mode::BPP_8 && _colors.size() >= 16 && _colors.size() <= 256 &&
                            _colors.size() % 16 == 0),
                   "Invalid colors count: ", _colors.size());
    }

    /**
     * @brief Returns the reference to an array of multiples of 16 colors.
     *
     * The colors are not copied but referenced, so they should outlive sprite_palette_item to avoid dangling references.
     */
    [[nodiscard]] constexpr const span<const color>& colors() const
    {
        return _colors;
    }

    /**
     * @brief Returns the bits per pixel of the color palettes to create.
     */
    [[nodiscard]] constexpr palette_bpp_mode bpp_mode() const
    {
        return _bpp_mode;
    }

    /**
     * @brief Search for a sprite_palette_ptr which contains the colors referenced by this item.
     * @return sprite_palette_ptr which contains the colors referenced by this item if it has been found;
     * <b>nullopt</b> otherwise.
     */
    [[nodiscard]] optional<sprite_palette_ptr> find_palette() const;

    /**
     * @brief Search for a sprite_palette_ptr which contains the colors referenced by this item.
     * If it is not found, it creates a sprite_palette_ptr which contains them.
     * @return sprite_palette_ptr which contains the colors referenced by this item if it has been found;
     * otherwise it returns a new sprite_palette_ptr which contains them.
     */
    [[nodiscard]] sprite_palette_ptr create_palette() const;

    /**
     * @brief Creates a new sprite_palette_ptr which contains the colors referenced by this item.
     * @return sprite_palette_ptr which contains the colors referenced by this item.
     */
    [[nodiscard]] sprite_palette_ptr create_new_palette() const;

    /**
     * @brief Search for a sprite_palette_ptr which contains the colors referenced by this item.
     * If it is not found, it creates a sprite_palette_ptr which contains them.
     * @return sprite_palette_ptr which contains the colors referenced by this item if it has been found;
     * otherwise it returns a new sprite_palette_ptr which contains them it if it can be allocated;
     * <b>nullopt</b> otherwise.
     */
    [[nodiscard]] optional<sprite_palette_ptr> create_palette_optional() const;

    /**
     * @brief Creates a new sprite_palette_ptr which contains the colors referenced by this item.
     * @return sprite_palette_ptr which contains the colors referenced by this item it if it can be allocated;
     * <b>nullopt</b> otherwise.
     */
    [[nodiscard]] optional<sprite_palette_ptr> create_new_palette_optional() const;

    /**
     * @brief Default equal operator.
     */
    [[nodiscard]] constexpr friend bool operator==(const sprite_palette_item& a,
                                                   const sprite_palette_item& b) = default;

private:
    span<const color> _colors;
    palette_bpp_mode _bpp_mode;
};

}

#endif

