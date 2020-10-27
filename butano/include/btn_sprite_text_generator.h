/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BTN_SPRITE_TEXT_GENERATOR_H
#define BTN_SPRITE_TEXT_GENERATOR_H

#include "btn_fixed.h"
#include "btn_vector.h"
#include "btn_sprite_font.h"
#include "btn_unordered_map.h"
#include "btn_config_sprite_text.h"
#include "btn_horizontal_alignment_type.h"

namespace btn
{

class sprite_ptr;
class fixed_point;

/**
 * @brief Generates sprites containing text from a given sprite_font.
 *
 * Currently, it only generates 16 colors (4 bits per pixel) sprites.
 *
 * Text can be printed in one sprite per character or multiple characters per sprite.
 *
 * Also, UTF-8 characters are supported.
 *
 * @ingroup sprite
 * @ingroup text
 */
class sprite_text_generator
{

public:
    using utf8_characters_map_type = unordered_map<int, int, BTN_CFG_SPRITE_TEXT_MAX_UTF8_CHARACTERS>;  //!< UTF-8 characters map type alias.

    /**
     * @brief Constructor.
     * @param font Sprite font for drawing text.
     */
    explicit sprite_text_generator(const sprite_font& font);

    /**
     * @brief Constructor.
     * @param font Sprite font for drawing text.
     * @param palette_item 16 colors (4 bits per pixel) sprite_palette_item
     * that generates the color palette used by the text sprites.
     */
    sprite_text_generator(const sprite_font& font, const sprite_palette_item& palette_item);

    /**
     * @brief Returns the sprite font for drawing text.
     */
    [[nodiscard]] const sprite_font& font() const
    {
        return _font;
    }

    /**
     * @brief Returns the sprite_palette_item that generates the color palette used by the text sprites.
     */
    [[nodiscard]] const sprite_palette_item& palette_item() const
    {
        return _palette_item;
    }

    /**
     * @brief Sets the 16 colors (4 bits per pixel) sprite_palette_item
     * that generates the color palette used by the text sprites.
     */
    void set_palette_item(const sprite_palette_item& palette_item);

    /**
     * @brief Returns the horizontal alignment of the output text sprites.
     */
    [[nodiscard]] horizontal_alignment_type alignment() const
    {
        return _alignment;
    }

    /**
     * @brief Sets the horizontal alignment of the output text sprites.
     */
    void set_alignment(horizontal_alignment_type alignment)
    {
        _alignment = alignment;
    }

    /**
     * @brief Returns the priority of the output sprites priority relative to backgrounds.
     *
     * Higher priorities are drawn first (and therefore can be covered by later sprites and backgrounds).
     * Sprites cover backgrounds of the same priority.
     */
    [[nodiscard]] int bg_priority() const
    {
        return _bg_priority;
    }

    /**
     * @brief Sets the priority of the output sprites relative to backgrounds.
     *
     * Higher priorities are drawn first (and therefore can be covered by later sprites and backgrounds).
     * Sprites cover backgrounds of the same priority.
     *
     * @param bg_priority Priority relative to backgrounds in the range [0..3].
     */
    void set_bg_priority(int bg_priority);

    /**
     * @brief Returns the priority of the output sprites relative to other sprites.
     *
     * Higher priorities are drawn first (and therefore can be covered by later sprites).
     */
    [[nodiscard]] int z_order() const
    {
        return _z_order;
    }

    /**
     * @brief Sets the priority of the output sprites relative to other sprites.
     *
     * Higher priorities are drawn first (and therefore can be covered by later sprites).
     *
     * @param z_order Priority relative to other sprites in the range [-32767..32767].
     */
    void set_z_order(int z_order);

    /**
     * @brief Indicates if this sprite_text_generator must generate one sprite per character or
     * print multiple characters in each output sprite, generating as less sprites as possible.
     */
    [[nodiscard]] bool one_sprite_per_character() const
    {
        return _one_sprite_per_character;
    }

    /**
     * @brief Sets if this sprite_text_generator must generate one sprite per character or
     * print multiple characters in each output sprite, generating as less sprites as possible.
     */
    void set_one_sprite_per_character(bool one_sprite_per_character)
    {
        _one_sprite_per_character = one_sprite_per_character;
    }

    /**
     * @brief Returns the map from UTF-8 characters to sprite tiles graphic indexes.
     */
    [[nodiscard]] const utf8_characters_map_type& utf8_characters_map() const
    {
        return _utf8_characters_map;
    }

    /**
     * @brief Returns the width in pixels of the given text.
     */
    [[nodiscard]] int width(const string_view& text) const;

    /**
     * @brief Generates text sprites for the given single line of text.
     * @tparam MaxSprites Maximum size of the returned sprite_ptr vector.
     * @param x Horizontal position of the first generated sprite, considering the current alignment.
     * @param y Vertical position of the first generated sprite, considering the current alignment.
     * @param text Single line of text to print.
     * @return sprite_ptr vector containing the generated text sprites.
     */
    template<int MaxSprites>
    [[nodiscard]] vector<sprite_ptr, MaxSprites> generate(fixed x, fixed y, const string_view& text) const
    {
        vector<sprite_ptr, MaxSprites> output_sprites;
        generate(x, y, text, output_sprites);
        return output_sprites;
    }

    /**
     * @brief Generates text sprites for the given single line of text.
     * @tparam MaxSprites Maximum size of the returned sprite_ptr vector.
     * @param position Position of the first generated sprite, considering the current alignment.
     * @param text Single line of text to print.
     * @return sprite_ptr vector containing the generated text sprites.
     */
    template<int MaxSprites>
    [[nodiscard]] vector<sprite_ptr, MaxSprites> generate(const fixed_point& position, const string_view& text) const
    {
        vector<sprite_ptr, MaxSprites> output_sprites;
        generate(position, text, output_sprites);
        return output_sprites;
    }

    /**
     * @brief Generates text sprites for the given single line of text.
     * @param x Horizontal position of the first generated sprite, considering the current alignment.
     * @param y Vertical position of the first generated sprite, considering the current alignment.
     * @param text Single line of text to print.
     * @param output_sprites Generated text sprites are stored in this vector.
     *
     * Keep in mind that this vector is not cleared before generating text.
     */
    void generate(fixed x, fixed y, const string_view& text, ivector<sprite_ptr>& output_sprites) const;

    /**
     * @brief Generates text sprites for the given single line of text.
     * @param position Position of the first generated sprite, considering the current alignment.
     * @param text Single line of text to print.
     * @param output_sprites Generated text sprites are stored in this vector.
     *
     * Keep in mind that this vector is not cleared before generating text.
     */
    void generate(const fixed_point& position, const string_view& text, ivector<sprite_ptr>& output_sprites) const;

    /**
     * @brief Generates text sprites for the given single line of text.
     * @param x Horizontal position of the first generated sprite, considering the current alignment.
     * @param y Vertical position of the first generated sprite, considering the current alignment.
     * @param text Single line of text to print.
     * @param output_sprites Generated text sprites are stored in this vector.
     *
     * Keep in mind that this vector is not cleared before generating text.
     *
     * @return <b>true</b> if the text generation finished successfully, otherwise <b>false</b>.
     */
    [[nodiscard]] bool generate_optional(fixed x, fixed y, const string_view& text,
                                         ivector<sprite_ptr>& output_sprites) const;

    /**
     * @brief Generates text sprites for the given single line of text.
     * @param position Position of the first generated sprite, considering the current alignment.
     * @param text Single line of text to print.
     * @param output_sprites Generated text sprites are stored in this vector.
     *
     * Keep in mind that this vector is not cleared before generating text.
     *
     * @return <b>true</b> if the text generation finished successfully, otherwise <b>false</b>.
     */
    [[nodiscard]] bool generate_optional(const fixed_point& position, const string_view& text,
                                         ivector<sprite_ptr>& output_sprites) const;

private:
    sprite_font _font;
    sprite_palette_item _palette_item;
    utf8_characters_map_type _utf8_characters_map;
    horizontal_alignment_type _alignment = horizontal_alignment_type::LEFT;
    int _bg_priority = 3;
    int _z_order = 0;
    bool _one_sprite_per_character = false;

    void _build_utf8_characters_map();
};

}

#endif

