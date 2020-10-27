/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BTN_SPRITES_MOSAIC_H
#define BTN_SPRITES_MOSAIC_H

#include "btn_fixed_fwd.h"

/**
 * @brief Functions related with the mosaic applied to the sprites.
 *
 * @ingroup mosaic
 */
namespace btn::sprites_mosaic
{
    /**
     * @brief Returns the horizontal stretch of the mosaic applied to the sprites.
     */
    [[nodiscard]] fixed horizontal_stretch();

    /**
     * @brief Sets the horizontal stretch of the mosaic applied to the sprites.
     * @param horizontal_stretch Horizontal stretch in the range [0..1].
     */
    void set_horizontal_stretch(fixed horizontal_stretch);

    /**
     * @brief Returns the vertical stretch of the mosaic applied to the sprites.
     */
    [[nodiscard]] fixed vertical_stretch();

    /**
     * @brief Sets the vertical stretch of the mosaic applied to the sprites.
     * @param vertical_stretch Vertical stretch in the range [0..1].
     */
    void set_vertical_stretch(fixed vertical_stretch);

    /**
     * @brief Sets the stretch of the mosaic applied to the sprites.
     * @param stretch Stretch in the range [0..1].
     */
    void set_stretch(fixed stretch);

    /**
     * @brief Sets the stretch of the mosaic applied to the sprites.
     * @param horizontal_stretch Horizontal stretch in the range [0..1].
     * @param vertical_stretch Vertical stretch in the range [0..1].
     */
    void set_stretch(fixed horizontal_stretch, fixed vertical_stretch);
}

#endif
