/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BN_REGULAR_BG_ATTRIBUTES_HBLANK_EFFECT_HANDLER_H
#define BN_REGULAR_BG_ATTRIBUTES_HBLANK_EFFECT_HANDLER_H

#include "bn_any.h"
#include "bn_regular_bg_tiles_ptr.h"
#include "bn_regular_bg_attributes.h"
#include "bn_bgs_manager.h"
#include "../hw/include/bn_hw_bgs.h"

namespace bn
{

class regular_bg_attributes_hblank_effect_handler
{

public:
    static void setup_target(int target_id, iany& target_last_value)
    {
        target_last_value = last_value_type(target_id);
    }

    [[nodiscard]] static bool target_visible(int target_id)
    {
        auto handle = reinterpret_cast<void*>(target_id);
        return bgs_manager::hw_id(handle).has_value();
    }

    [[nodiscard]] static bool target_updated(int target_id, iany& target_last_value)
    {
        last_value_type& last_value = target_last_value.value<last_value_type>();
        last_value_type new_value = last_value_type(target_id);
        bool updated = last_value != new_value;
        last_value = new_value;
        return updated;
    }

    [[nodiscard]] static uint16_t* output_register(int target_id)
    {
        auto handle = reinterpret_cast<void*>(target_id);
        return hw::bgs::attributes_register(*bgs_manager::hw_id(handle));
    }

    static void write_output_values(int target_id, const iany&, const void* input_values_ptr,
                                    uint16_t* output_values_ptr)
    {
        auto handle = reinterpret_cast<void*>(target_id);
        auto regular_bg_attributes_ptr = reinterpret_cast<const regular_bg_attributes*>(input_values_ptr);
        bgs_manager::fill_hblank_effect_regular_attributes(
                    handle, regular_bg_attributes_ptr, output_values_ptr);
    }

    static void show(int)
    {
    }

    static void cleanup(int)
    {
        bgs_manager::reload();
    }

private:
    class alignas(int) last_value_type
    {

    public:
        explicit last_value_type(const regular_bg_map_ptr& target_map_ptr) :
            _map_dimensions(target_map_ptr.dimensions()),
            _tiles_cbb(target_map_ptr.tiles().cbb()),
            _bpp(target_map_ptr.bpp())
        {
        }

        explicit last_value_type(void* handle) :
            last_value_type(bgs_manager::map(handle))
        {
        }

        explicit last_value_type(int target_id) :
            last_value_type(reinterpret_cast<void*>(target_id))
        {
        }

        [[nodiscard]] friend bool operator==(const last_value_type& a, const last_value_type& b) = default;

    private:
        size _map_dimensions;
        int _tiles_cbb;
        bpp_mode _bpp;
    };
};

}

#endif
