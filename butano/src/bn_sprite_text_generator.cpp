/*
 * Copyright (c) 2020-2021 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#include "bn_sprite_text_generator.h"

#include "bn_sprites.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_builder.h"
#include "../hw/include/bn_hw_sprite_tiles.h"

namespace bn
{

namespace
{
    static_assert(BN_CFG_SPRITE_TEXT_MAX_UTF8_CHARACTERS > 0);
    static_assert(power_of_two(BN_CFG_SPRITE_TEXT_MAX_UTF8_CHARACTERS));

    constexpr const int max_columns_per_sprite = 32;
    constexpr const int fixed_character_width = 8;
    constexpr const int fixed_max_characters_per_sprite = max_columns_per_sprite / fixed_character_width;

    template<sprite_size size, int max_tiles_per_sprite, bool allow_failure>
    [[nodiscard]] tile* _build_sprite(const sprite_text_generator& generator, const sprite_palette_ptr& palette,
                                      const fixed_point& current_position, ivector<sprite_ptr>& output_sprites)
    {
        optional<sprite_tiles_ptr> tiles_ptr;

        if(allow_failure)
        {
            if(output_sprites.full())
            {
                return nullptr;
            }

            tiles_ptr = sprite_tiles_ptr::allocate_optional(max_tiles_per_sprite, bpp_mode::BPP_4);

            if(! tiles_ptr)
            {
                return nullptr;
            }
        }
        else
        {
            BN_ASSERT(! output_sprites.full(), "output_sprites vector is full,\ncan't hold more sprites");

            tiles_ptr = sprite_tiles_ptr::allocate(max_tiles_per_sprite, bpp_mode::BPP_4);
        }

        sprite_tiles_ptr& tiles_ptr_ref = *tiles_ptr;
        optional<span<tile>> tiles_vram = tiles_ptr_ref.vram();

        sprite_builder builder(sprite_shape_size(sprite_shape::WIDE, size), move(tiles_ptr_ref), palette);
        builder.set_position(current_position);
        builder.set_bg_priority(generator.bg_priority());
        builder.set_z_order(generator.z_order());

        if(allow_failure)
        {
            optional<sprite_ptr> sprite_ptr = sprite_ptr::create_optional(move(builder));

            if(! sprite_ptr)
            {
                return nullptr;
            }

            output_sprites.push_back(move(*sprite_ptr));
        }
        else
        {
            output_sprites.push_back(sprite_ptr::create(move(builder)));
        }

        return tiles_vram->data();
    }


    class fixed_width_painter
    {

    public:
        explicit fixed_width_painter(const sprite_text_generator& generator) :
            _space_between_characters(generator.font().space_between_characters())
        {
        }

        [[nodiscard]] int width() const
        {
            return _width;
        }

        void paint_space()
        {
            _width += fixed_character_width + _space_between_characters;
        }

        void paint_tab()
        {
            _width += (fixed_character_width * 4) + _space_between_characters;
        }

        [[nodiscard]] bool paint_character([[maybe_unused]] int graphics_index)
        {
            _width += fixed_character_width + _space_between_characters;
            return true;
        }

    private:
        int _space_between_characters;
        int _width = 0;
    };


    class variable_width_painter
    {

    public:
        explicit variable_width_painter(const sprite_text_generator& generator) :
            _character_widths(generator.font().character_widths_ref().data()),
            _space_between_characters(generator.font().space_between_characters())
        {
        }

        [[nodiscard]] int width() const
        {
            return _width;
        }

        void paint_space()
        {
            _width += _character_widths[0] + _space_between_characters;
        }

        void paint_tab()
        {
            _width += (_character_widths[0] * 4) + _space_between_characters;
        }

        [[nodiscard]] bool paint_character(int graphics_index)
        {
            _width += _character_widths[graphics_index + 1] + _space_between_characters;
            return true;
        }

    private:
        const int8_t* _character_widths;
        int _space_between_characters;
        int _width = 0;
    };


    template<bool allow_failure>
    class fixed_one_sprite_per_character_painter
    {

    public:
        fixed_one_sprite_per_character_painter(
                const sprite_text_generator& generator, sprite_palette_ptr&& palette,
                const fixed_point& position, ivector<sprite_ptr>& output_sprites) :
            _generator(generator),
            _output_sprites(output_sprites),
            _palette(move(palette)),
            _current_position(position.x() + (fixed_character_width / 2), position.y()),
            _space_between_characters(generator.font().space_between_characters())
        {
        }

        void paint_space()
        {
            _current_position.set_x(_current_position.x() + fixed_character_width + _space_between_characters);
        }

        void paint_tab()
        {
            _current_position.set_x(_current_position.x() + (fixed_character_width * 4) + _space_between_characters);
        }

        [[nodiscard]] bool paint_character(int graphics_index)
        {
            if(allow_failure)
            {
                if(_output_sprites.full())
                {
                    return false;
                }
            }
            else
            {
                BN_ASSERT(! _output_sprites.full(), "output_sprites vector is full,\ncan't hold more sprites");
            }

            const sprite_item& item = _generator.font().item();
            const sprite_tiles_item& tiles_item = item.tiles_item();
            optional<sprite_tiles_ptr> source_tiles_ptr;

            if(allow_failure)
            {
                source_tiles_ptr = sprite_tiles_ptr::create_optional(tiles_item, graphics_index);

                if(! source_tiles_ptr)
                {
                    return false;
                }
            }
            else
            {
                source_tiles_ptr = sprite_tiles_ptr::create(tiles_item, graphics_index);
            }

            sprite_shape_size shape_size(item.shape_size().shape(), sprite_size::SMALL);
            sprite_builder builder(shape_size, move(*source_tiles_ptr), _palette);
            builder.set_position(_current_position);
            builder.set_bg_priority(_generator.bg_priority());
            builder.set_z_order(_generator.z_order());

            if(allow_failure)
            {
                optional<sprite_ptr> sprite_ptr = sprite_ptr::create_optional(move(builder));

                if(! sprite_ptr)
                {
                    return false;
                }

                _output_sprites.push_back(move(*sprite_ptr));
            }
            else
            {
                _output_sprites.push_back(sprite_ptr::create(move(builder)));
            }

            _current_position.set_x(_current_position.x() + fixed_character_width + _space_between_characters);
            return true;
        }

    private:
        const sprite_text_generator& _generator;
        ivector<sprite_ptr>& _output_sprites;
        sprite_palette_ptr _palette;
        fixed_point _current_position;
        int _space_between_characters;
    };


    template<bool allow_failure>
    class variable_one_sprite_per_character_painter
    {

    public:
        variable_one_sprite_per_character_painter(
                const sprite_text_generator& generator, sprite_palette_ptr&& palette, const fixed_point& position,
                ivector<sprite_ptr>& output_sprites) :
            _generator(generator),
            _character_widths(generator.font().character_widths_ref().data()),
            _output_sprites(output_sprites),
            _palette(move(palette)),
            _current_position(position.x() + (fixed_character_width / 2), position.y()),
            _space_between_characters(generator.font().space_between_characters())
        {
        }

        void paint_space()
        {
            _current_position.set_x(_current_position.x() + _character_widths[0] + _space_between_characters);
        }

        void paint_tab()
        {
            _current_position.set_x(_current_position.x() + (_character_widths[0] * 4) + _space_between_characters);
        }

        [[nodiscard]] bool paint_character(int graphics_index)
        {
            if(int character_width = _character_widths[graphics_index + 1])
            {
                if(allow_failure)
                {
                    if(_output_sprites.full())
                    {
                        return false;
                    }
                }
                else
                {
                    BN_ASSERT(! _output_sprites.full(), "output_sprites vector is full,\ncan't hold more sprites");
                }

                const sprite_item& item = _generator.font().item();
                const sprite_tiles_item& tiles_item = item.tiles_item();
                optional<sprite_tiles_ptr> source_tiles_ptr;

                if(allow_failure)
                {
                    source_tiles_ptr = sprite_tiles_ptr::create_optional(tiles_item, graphics_index);

                    if(! source_tiles_ptr)
                    {
                        return false;
                    }
                }
                else
                {
                    source_tiles_ptr = sprite_tiles_ptr::create(tiles_item, graphics_index);
                }

                sprite_shape_size shape_size(item.shape_size().shape(), sprite_size::SMALL);
                sprite_builder builder(shape_size, move(*source_tiles_ptr), _palette);
                builder.set_position(_current_position);
                builder.set_bg_priority(_generator.bg_priority());
                builder.set_z_order(_generator.z_order());

                if(allow_failure)
                {
                    optional<sprite_ptr> sprite_ptr = sprite_ptr::create_optional(move(builder));

                    if(! sprite_ptr)
                    {
                        return false;
                    }

                    _output_sprites.push_back(move(*sprite_ptr));
                }
                else
                {
                    _output_sprites.push_back(sprite_ptr::create(move(builder)));
                }

                _current_position.set_x(_current_position.x() + character_width + _space_between_characters);
            }
            else
            {
                _current_position.set_x(_current_position.x() + _space_between_characters);
            }

            return true;
        }

    private:
        const sprite_text_generator& _generator;
        const int8_t* _character_widths;
        ivector<sprite_ptr>& _output_sprites;
        sprite_palette_ptr _palette;
        fixed_point _current_position;
        int _space_between_characters;
    };


    template<bool allow_failure>
    class fixed_8x8_painter
    {

    public:
        fixed_8x8_painter(const sprite_text_generator& generator, sprite_palette_ptr&& palette,
                          const fixed_point& position, ivector<sprite_ptr>& output_sprites) :
            _generator(generator),
            _output_sprites(output_sprites),
            _palette(move(palette)),
            _current_position(position.x() + (max_columns_per_sprite / 2), position.y()),
            _space_between_characters(generator.font().space_between_characters())
        {
        }

        ~fixed_8x8_painter()
        {
            _clear_left();
        }

        void paint_space()
        {
            if(_sprite_character_index < fixed_max_characters_per_sprite)
            {
                _clear(1);
                ++_sprite_character_index;
            }

            _current_position.set_x(_current_position.x() + fixed_character_width + _space_between_characters);
        }

        void paint_tab()
        {
            _clear_left();
            _current_position.set_x(_current_position.x() + (fixed_character_width * 4) + _space_between_characters);
        }

        [[nodiscard]] bool paint_character(int graphics_index)
        {
            if(_sprite_character_index == fixed_max_characters_per_sprite)
            {
                _tiles_vram = _build_sprite<sprite_size::NORMAL, fixed_max_characters_per_sprite, allow_failure>(
                            _generator, _palette, _current_position, _output_sprites);

                if(allow_failure && ! _tiles_vram)
                {
                    return false;
                }

                _sprite_character_index = 0;
            }

            const sprite_item& item = _generator.font().item();
            const tile* source_tiles_data = item.tiles_item().graphics_tiles_ref(graphics_index).data();
            hw::sprite_tiles::copy_tiles(source_tiles_data, 1, _tiles_vram + _sprite_character_index);
            _current_position.set_x(_current_position.x() + fixed_character_width + _space_between_characters);
            ++_sprite_character_index;
            return true;
        }

    private:
        const sprite_text_generator& _generator;
        ivector<sprite_ptr>& _output_sprites;
        sprite_palette_ptr _palette;
        fixed_point _current_position;
        tile* _tiles_vram = nullptr;
        int _sprite_character_index = fixed_max_characters_per_sprite;
        int _space_between_characters;

        void _clear(int characters) const
        {
            hw::sprite_tiles::clear_tiles(characters, _tiles_vram + _sprite_character_index);
        }

        void _clear_left()
        {
            if(_sprite_character_index < fixed_max_characters_per_sprite)
            {
                _clear(fixed_max_characters_per_sprite - _sprite_character_index);
                _sprite_character_index = fixed_max_characters_per_sprite;
            }
        }
    };


    template<bool allow_failure>
    class variable_8x8_painter
    {

    public:
        variable_8x8_painter(const sprite_text_generator& generator, sprite_palette_ptr&& palette,
                             const fixed_point& position, ivector<sprite_ptr>& output_sprites) :
            _generator(generator),
            _character_widths(generator.font().character_widths_ref().data()),
            _output_sprites(output_sprites),
            _palette(move(palette)),
            _current_position(position.x() + (max_columns_per_sprite / 2), position.y()),
            _space_between_characters(generator.font().space_between_characters())
        {
        }

        void paint_space()
        {
            int width_with_space = _character_widths[0] + _space_between_characters;
            _sprite_column += width_with_space;
            _current_position.set_x(_current_position.x() + width_with_space);
        }

        void paint_tab()
        {
            int width_with_space = (_character_widths[0] * 4) + _space_between_characters;
            _sprite_column += width_with_space;
            _current_position.set_x(_current_position.x() + width_with_space);
        }

        [[nodiscard]] bool paint_character(int graphics_index)
        {
            if(int width = _character_widths[graphics_index + 1])
            {
                int width_with_space = width + _space_between_characters;

                if(_sprite_column + width_with_space > max_columns_per_sprite)
                {
                    _tiles_vram = _build_sprite<sprite_size::NORMAL, _tiles, allow_failure>(
                                _generator, _palette, _current_position, _output_sprites);

                    if(allow_failure && ! _tiles_vram)
                    {
                        return false;
                    }

                    hw::sprite_tiles::clear_tiles(_tiles, _tiles_vram);
                    _sprite_column = 0;
                }

                const sprite_tiles_item& tiles_item = _generator.font().item().tiles_item();
                const tile* source_tiles_data = tiles_item.tiles_ref().data();
                int source_height = tiles_item.graphics_count() * _character_height;
                int source_y = graphics_index * _character_height;
                hw::sprite_tiles::plot_tiles(width, source_tiles_data, source_height, source_y, _sprite_column,
                                             _tiles_vram);
                _current_position.set_x(_current_position.x() + width_with_space);
                _sprite_column += width_with_space;
            }
            else
            {
                _current_position.set_x(_current_position.x() + _space_between_characters);
                _sprite_column += _space_between_characters;
            }

            return true;
        }

    private:
        static constexpr const int _character_height = 8;
        static constexpr const int _tiles = 4;

        const sprite_text_generator& _generator;
        const int8_t* _character_widths;
        ivector<sprite_ptr>& _output_sprites;
        sprite_palette_ptr _palette;
        fixed_point _current_position;
        tile* _tiles_vram = nullptr;
        int _space_between_characters;
        int _sprite_column = max_columns_per_sprite;
    };


    template<bool allow_failure>
    class fixed_8x16_painter
    {

    public:
        fixed_8x16_painter(const sprite_text_generator& generator, sprite_palette_ptr&& palette,
                           const fixed_point& position, ivector<sprite_ptr>& output_sprites) :
            _generator(generator),
            _output_sprites(output_sprites),
            _palette(move(palette)),
            _current_position(position.x() + (max_columns_per_sprite / 2), position.y()),
            _space_between_characters(generator.font().space_between_characters())
        {
        }

        ~fixed_8x16_painter()
        {
            _clear_left();
        }

        void paint_space()
        {
            if(_sprite_character_index < fixed_max_characters_per_sprite)
            {
                _clear(1);
                ++_sprite_character_index;
            }

            _current_position.set_x(_current_position.x() + fixed_character_width + _space_between_characters);
        }

        void paint_tab()
        {
            _clear_left();
            _current_position.set_x(_current_position.x() + (fixed_character_width * 4) + _space_between_characters);
        }

        [[nodiscard]] bool paint_character(int graphics_index)
        {
            if(_sprite_character_index == fixed_max_characters_per_sprite)
            {
                _tiles_vram = _build_sprite<sprite_size::BIG, fixed_max_characters_per_sprite * 2, allow_failure>(
                            _generator, _palette, _current_position, _output_sprites);

                if(allow_failure && ! _tiles_vram)
                {
                    return false;
                }

                _sprite_character_index = 0;
            }

            const sprite_item& item = _generator.font().item();
            const tile* source_tiles_data = item.tiles_item().graphics_tiles_ref(graphics_index).data();
            tile* up_tiles_vram_ptr = _tiles_vram + _sprite_character_index;
            hw::sprite_tiles::copy_tiles(source_tiles_data, 1, up_tiles_vram_ptr);

            tile* down_tiles_vram_ptr = up_tiles_vram_ptr + fixed_max_characters_per_sprite;
            hw::sprite_tiles::copy_tiles(source_tiles_data + 1, 1, down_tiles_vram_ptr);

            _current_position.set_x(_current_position.x() + fixed_character_width + _space_between_characters);
            ++_sprite_character_index;
            return true;
        }

    private:
        const sprite_text_generator& _generator;
        ivector<sprite_ptr>& _output_sprites;
        sprite_palette_ptr _palette;
        fixed_point _current_position;
        tile* _tiles_vram = nullptr;
        int _space_between_characters;
        int _sprite_character_index = fixed_max_characters_per_sprite;

        void _clear(int characters) const
        {
            tile* up_tiles_vram_ptr = _tiles_vram + _sprite_character_index;
            hw::sprite_tiles::clear_tiles(characters, up_tiles_vram_ptr);

            tile* down_tiles_vram_ptr = up_tiles_vram_ptr + fixed_max_characters_per_sprite;
            hw::sprite_tiles::clear_tiles(characters, down_tiles_vram_ptr);
        }

        void _clear_left()
        {
            if(_sprite_character_index < fixed_max_characters_per_sprite)
            {
                _clear(fixed_max_characters_per_sprite - _sprite_character_index);
                _sprite_character_index = fixed_max_characters_per_sprite;
            }
        }
    };


    template<bool allow_failure>
    class variable_8x16_painter
    {

    public:
        variable_8x16_painter(const sprite_text_generator& generator, sprite_palette_ptr&& palette,
                              const fixed_point& position, ivector<sprite_ptr>& output_sprites) :
            _generator(generator),
            _character_widths(generator.font().character_widths_ref().data()),
            _output_sprites(output_sprites),
            _palette(move(palette)),
            _current_position(position.x() + (max_columns_per_sprite / 2), position.y()),
            _space_between_characters(generator.font().space_between_characters())
        {
        }

        void paint_space()
        {
            int width_with_space = _character_widths[0] + _space_between_characters;
            _sprite_column += width_with_space;
            _current_position.set_x(_current_position.x() + width_with_space);
        }

        void paint_tab()
        {
            int width_with_space = (_character_widths[0] * 4) + _space_between_characters;
            _sprite_column += width_with_space;
            _current_position.set_x(_current_position.x() + width_with_space);
        }

        [[nodiscard]] bool paint_character(int graphics_index)
        {
            if(int width = _character_widths[graphics_index + 1])
            {
                int width_with_space = width + _space_between_characters;

                if(_sprite_column + width_with_space > max_columns_per_sprite)
                {
                    _tiles_vram = _build_sprite<sprite_size::BIG, _tiles, allow_failure>(
                                _generator, _palette, _current_position, _output_sprites);

                    if(allow_failure && ! _tiles_vram)
                    {
                        return false;
                    }

                    hw::sprite_tiles::clear_tiles(_tiles, _tiles_vram);
                    _sprite_column = 0;
                }

                const sprite_tiles_item& tiles_item = _generator.font().item().tiles_item();
                const tile* source_tiles_data = tiles_item.tiles_ref().data();
                int source_height = tiles_item.graphics_count() * _character_height;
                int source_y = graphics_index * _character_height;
                hw::sprite_tiles::plot_tiles(width, source_tiles_data, source_height,
                                             source_y,
                                             _sprite_column, _tiles_vram);
                hw::sprite_tiles::plot_tiles(width, source_tiles_data, source_height,
                                             source_y + (_character_height / 2),
                                             _sprite_column + (_character_height * 2), _tiles_vram);
                _current_position.set_x(_current_position.x() + width_with_space);
                _sprite_column += width_with_space;
            }
            else
            {
                _current_position.set_x(_current_position.x() + _space_between_characters);
                _sprite_column += _space_between_characters;
            }

            return true;
        }

    private:
        static constexpr const int _character_height = 16;
        static constexpr const int _tiles = 8;

        const sprite_text_generator& _generator;
        const int8_t* _character_widths;
        ivector<sprite_ptr>& _output_sprites;
        sprite_palette_ptr _palette;
        fixed_point _current_position;
        tile* _tiles_vram = nullptr;
        int _space_between_characters;
        int _sprite_column = max_columns_per_sprite;
    };


    template<bool allow_failure, class Painter>
    [[nodiscard]] bool paint(const string_view& text, const iunordered_map<int, int>& utf8_characters_map,
                             Painter& painter)
    {
        const char* text_data = text.data();
        int text_index = 0;
        int text_size = text.size();

        while(text_index < text_size)
        {
            char character = text_data[text_index];

            if(character == ' ')
            {
                painter.paint_space();
                ++text_index;
            }
            else if(character == '\t')
            {
                painter.paint_tab();
                ++text_index;
            }
            else if(character >= '!')
            {
                int graphics_index;

                if(character <= '~')
                {
                    graphics_index = character - '!';
                    ++text_index;
                }
                else
                {
                    utf8_character utf8_char(text_data[text_index]);
                    auto it = utf8_characters_map.find(utf8_char.data());
                    BN_ASSERT(it != utf8_characters_map.end(), "UTF-8 character not found: ", text);

                    graphics_index = it->second;
                    text_index += utf8_char.size();
                }

                bool success = painter.paint_character(graphics_index);

                if(allow_failure && ! success)
                {
                    return false;
                }
            }
            else
            {
                BN_ERROR("Invalid character: ", character, " (text: ", text, ")");
            }
        }

        return true;
    }

    template<bool allow_failure>
    bool _generate(const sprite_text_generator& generator, const fixed_point& position, const string_view& text,
                   const iunordered_map<int, int>& utf8_characters_map, ivector<sprite_ptr>& output_sprites)
    {
        optional<sprite_palette_ptr> palette;

        if(allow_failure)
        {
            palette = generator.palette_item().create_palette_optional();

            if(! palette)
            {
                return false;
            }
        }
        else
        {
            palette = generator.palette_item().create_palette();
        }

        fixed_point aligned_position = position;

        switch(generator.alignment())
        {

        case sprite_text_generator::alignment_type::LEFT:
            break;

        case sprite_text_generator::alignment_type::CENTER:
            aligned_position.set_x(aligned_position.x() - (generator.width(text) / 2));
            break;

        case sprite_text_generator::alignment_type::RIGHT:
            aligned_position.set_x(aligned_position.x() - generator.width(text));
            break;

        default:
            BN_ERROR("Invalid alignment: ", int(generator.alignment()));
            break;
        }

        const sprite_font& font = generator.font();
        int output_sprites_count = output_sprites.size();
        bool success;

        if(generator.one_sprite_per_character())
        {
            if(font.character_widths_ref().empty())
            {
                fixed_one_sprite_per_character_painter<allow_failure> painter(
                            generator, move(*palette), aligned_position, output_sprites);
                success = paint<allow_failure>(text, utf8_characters_map, painter);
            }
            else
            {
                variable_one_sprite_per_character_painter<allow_failure> painter(
                            generator, move(*palette), aligned_position, output_sprites);
                success = paint<allow_failure>(text, utf8_characters_map, painter);
            }
        }
        else
        {
            if(font.item().shape_size().height() == 8)
            {
                if(font.character_widths_ref().empty())
                {
                    fixed_8x8_painter<allow_failure> painter(generator, move(*palette), aligned_position,
                                                             output_sprites);
                    success = paint<allow_failure>(text, utf8_characters_map, painter);
                }
                else
                {
                    variable_8x8_painter<allow_failure> painter(generator, move(*palette), aligned_position,
                                                                output_sprites);
                    success = paint<allow_failure>(text, utf8_characters_map, painter);
                }
            }
            else
            {
                if(font.character_widths_ref().empty())
                {
                    fixed_8x16_painter<allow_failure> painter(generator, move(*palette), aligned_position,
                                                              output_sprites);
                    success = paint<allow_failure>(text, utf8_characters_map, painter);
                }
                else
                {
                    variable_8x16_painter<allow_failure> painter(generator, move(*palette), aligned_position,
                                                                 output_sprites);
                    success = paint<allow_failure>(text, utf8_characters_map, painter);
                }
            }
        }

        if(allow_failure && ! success)
        {
            output_sprites.shrink(output_sprites_count);
        }

        return success;
    }
}

sprite_text_generator::sprite_text_generator(const sprite_font& font) :
    _font(font),
    _palette_item(font.item().palette_item())
{
    _build_utf8_characters_map();
}

sprite_text_generator::sprite_text_generator(const sprite_font& font, const sprite_palette_item& palette_item) :
    _font(font),
    _palette_item(palette_item)
{
    BN_ASSERT(palette_item.bpp() == bpp_mode::BPP_4, "8BPP fonts not supported");

    _build_utf8_characters_map();
}

void sprite_text_generator::set_palette_item(const sprite_palette_item& palette_item)
{
    BN_ASSERT(palette_item.bpp() == bpp_mode::BPP_4, "8BPP fonts not supported");

    _palette_item = palette_item;
}

void sprite_text_generator::set_bg_priority(int bg_priority)
{
    BN_ASSERT(bg_priority >= 0 && bg_priority <= sprites::max_bg_priority(), "Invalid BG priority: ", bg_priority);

    _bg_priority = bg_priority;
}

void sprite_text_generator::set_z_order(int z_order)
{
    BN_ASSERT(z_order >= sprites::min_z_order() && z_order <= sprites::max_z_order(), "Invalid z order: ", z_order);

    _z_order = z_order;
}

int sprite_text_generator::width(const string_view& text) const
{
    if(_font.character_widths_ref().empty())
    {
        fixed_width_painter painter(*this);
        [[maybe_unused]] bool success = paint<false>(text, _utf8_characters_map, painter);
        return painter.width();
    }
    else
    {
        variable_width_painter painter(*this);
        [[maybe_unused]] bool success = paint<false>(text, _utf8_characters_map, painter);
        return painter.width();
    }
}

void sprite_text_generator::generate(fixed x, fixed y, const string_view& text,
                                     ivector<sprite_ptr>& output_sprites) const
{
    _generate<false>(*this, fixed_point(x, y), text, _utf8_characters_map, output_sprites);
}

void sprite_text_generator::generate(const fixed_point& position, const string_view& text,
                                     ivector<sprite_ptr>& output_sprites) const
{
    _generate<false>(*this, position, text, _utf8_characters_map, output_sprites);
}

bool sprite_text_generator::generate_optional(fixed x, fixed y, const string_view& text,
                                              ivector<sprite_ptr>& output_sprites) const
{
    return _generate<true>(*this, fixed_point(x, y), text, _utf8_characters_map, output_sprites);
}

bool sprite_text_generator::generate_optional(const fixed_point& position, const string_view& text,
                                              ivector<sprite_ptr>& output_sprites) const
{
    return _generate<true>(*this, position, text, _utf8_characters_map, output_sprites);
}

void sprite_text_generator::_build_utf8_characters_map()
{
    int utf8_character_index = sprite_font::minimum_graphics;

    for(const string_view& utf8_character_text : _font.utf8_characters_ref())
    {
        utf8_character utf8_char(utf8_character_text.data());
        _utf8_characters_map.insert(utf8_char.data(), utf8_character_index);
        ++utf8_character_index;
    }
}

}
