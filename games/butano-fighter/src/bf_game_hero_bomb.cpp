#include "bf_game_hero_bomb.h"

#include "btn_sound.h"
#include "btn_keypad.h"
#include "btn_window.h"
#include "btn_blending.h"
#include "btn_regular_bg_builder.h"
#include "btn_hero_bomb_bg_item.h"
#include "btn_sound_items.h"
#include "bf_constants.h"
#include "bf_game_hero.h"
#include "bf_wave_generator.h"
#include "bf_game_background.h"

namespace bf
{

namespace
{
    constexpr const int open_frames = 50;
    constexpr const int close_frames = 130;

    btn::regular_bg_ptr _create_bg()
    {
        btn::regular_bg_builder builder(btn::bg_items::hero_bomb);
        builder.set_priority(1);
        builder.set_blending_enabled(true);
        return builder.release_build();
    }
}

game_hero_bomb::game_hero_bomb() :
    _bg(_create_bg()),
    _bg_move_action(_bg, -0.5, 4),
    _circle_hblank_effect(btn::rect_window_boundaries_hblank_effect_ptr::create_horizontal(
                              btn::window::internal(), _circle_hblank_effect_deltas)),
    _wave_hblank_effect(btn::regular_bg_position_hblank_effect_ptr::create_horizontal(_bg, _wave_hblank_effect_deltas))
{
    btn::window::outside().set_show_bg(_bg, false);
    _circle_hblank_effect.set_visible(false);
    wave_generator().generate(_wave_hblank_effect_deltas);
    _wave_hblank_effect.reload_deltas_ref();
    _wave_hblank_effect.set_visible(false);
}

void game_hero_bomb::update(game_hero& hero, game_background& background)
{
    switch(_status)
    {

    case status_type::INACTIVE:
        if(btn::keypad::pressed(btn::keypad::button_type::A) && hero.throw_bomb())
        {
            const btn::fixed_point& hero_position = hero.weapon_position();
            btn::rect_window window = btn::window::internal();
            window.set_boundaries(hero_position, hero_position);
            window.set_show_blending(false);
            _move_window_top_action.emplace(window, -4);
            _move_window_bottom_action.emplace(window, 4);

            _circle_generator.set_origin_y(hero_position.y());
            _circle_generator.set_radius(0);
            _circle_generator.generate(_circle_hblank_effect_deltas);
            _circle_hblank_effect.reload_deltas_ref();
            _circle_hblank_effect.set_visible(true);

            background.show_bomb_open(open_frames);
            _wave_hblank_effect.set_visible(true);
            btn::sound::play(btn::sound_items::explosion_2);
            _status = status_type::OPEN;
            _counter = open_frames;
            _flame_sound_counter = 0;
        }
        break;

    case status_type::OPEN:
        _bg_move_action.update();

        if(_counter)
        {
            --_counter;

            _move_window_top_action->update();
            _move_window_bottom_action->update();

            _circle_generator.set_radius(_circle_generator.radius() + 4);
            _circle_generator.generate(_circle_hblank_effect_deltas);
            _circle_hblank_effect.reload_deltas_ref();

            _play_flame_sound();
        }
        else
        {
            _move_window_top_action.reset();
            _move_window_bottom_action.reset();
            _circle_hblank_effect.set_visible(false);
            btn::window::internal().set_boundaries(-1000, -1000, 1000, 1000);
            _blending_action.reset();
            _status = status_type::CLOSE;
            _counter = close_frames;
        }
        break;

    case status_type::CLOSE:
        _bg_move_action.update();

        if(_blending_action)
        {
            _blending_action->update();

            if(_blending_action->done())
            {
                _blending_action.reset();
            }
        }

        if(_counter)
        {
            --_counter;

            if(_counter == close_frames - 30)
            {
                btn::window::internal().set_show_blending(true);
                btn::blending::set_transparency_alpha(1);
                _blending_action.emplace(close_frames - 30, 0);
                background.show_bomb_fade(close_frames - 50);
            }
            else if(_counter == 20)
            {
                btn::window::internal().set_boundaries(0, 0, 0, 0);
                _blending_action.reset();
                background.show_clouds();
                _wave_hblank_effect.set_visible(false);
            }

            if(_counter > 40)
            {
                _play_flame_sound();
            }
        }
        else
        {
            _status = status_type::INACTIVE;
        }
        break;
    }
}

void game_hero_bomb::_play_flame_sound()
{
    ++_flame_sound_counter;

    if(_flame_sound_counter > 16 && _flame_sound_counter % 16 == 0)
    {
        btn::sound::play(btn::sound_items::flame_thrower);
    }
}

}