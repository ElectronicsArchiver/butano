/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BTN_TEMPLATE_ACTIONS_H
#define BTN_TEMPLATE_ACTIONS_H

#include "btn_assert.h"
#include "btn_limits.h"

namespace btn
{

/**
 * @brief Modifies a property by a given delta.
 *
 * @tparam Property Property to modify.
 * @tparam PropertyManager Reads and writes the property to modify.
 *
 * @ingroup template_action
 */
template<typename Property, class PropertyManager>
class by_template_action
{

public:
    /**
     * @brief Resets the property to its initial state.
     */
    void reset()
    {
        PropertyManager::set(_initial_property);
    }

    /**
     * @brief Adds delta_property to the property.
     */
    void update()
    {
        PropertyManager::set(PropertyManager::get() + _delta_property);
    }

protected:
    /**
     * @brief Constructor.
     * @param delta_property How much to add to the property when update is called.
     */
    explicit by_template_action(const Property& delta_property) :
        _delta_property(delta_property),
        _initial_property(PropertyManager::get())
    {
    }

    /**
     * @brief Returns how much to add to the property when update is called.
     */
    [[nodiscard]] const Property& delta_property() const
    {
        return _delta_property;
    }

private:
    Property _delta_property;
    Property _initial_property;
};


/**
 * @brief Modifies the property by delta from a minimum to a maximum.
 * When the property is over the given maximum, it goes back to the given minimum and vice versa.
 *
 * @tparam Property Property to modify.
 * @tparam PropertyManager Reads and writes the property to modify.
 *
 * @ingroup template_action
 */
template<typename Property, class PropertyManager>
class cyclic_by_template_action
{

public:
    /**
     * @brief Resets the property to its initial state.
     */
    void reset()
    {
        PropertyManager::set(_initial_property);
    }

    /**
     * @brief Adds delta_property to the property.
     *
     * When the property reaches the maximum, it goes back to the minimum and vice versa.
     */
    void update()
    {
        Property new_property = PropertyManager::get() + _delta_property;

        if(new_property < _min_property)
        {
            new_property += _after_max_property - _min_property;
        }
        else if(new_property >= _after_max_property)
        {
            new_property -= _after_max_property - _min_property;
        }

        PropertyManager::set(new_property);
    }

protected:
    /**
     * @brief Constructor.
     * @param delta_property How much to add to the property when update is called.
     * @param min_property Minimum property.
     * @param after_max_property Immediate amount after the maximum property.
     */
    cyclic_by_template_action(const Property& delta_property, const Property& min_property,
                              const Property& after_max_property) :
        _delta_property(delta_property),
        _min_property(min_property),
        _after_max_property(after_max_property),
        _initial_property(PropertyManager::get())
    {
    }

    /**
     * @brief Returns how much to add to the property when update is called.
     */
    [[nodiscard]] const Property& delta_property() const
    {
        return _delta_property;
    }

    /**
     * @brief Sets the minimum property.
     */
    void set_min_property(const Property& min_property)
    {
        _min_property = min_property;
    }

    /**
     * @brief Sets the immediate amount after the maximum property.
     */
    void set_after_max_property(const Property& after_max_property)
    {
        _after_max_property = after_max_property;
    }

private:
    Property _delta_property;
    Property _min_property;
    Property _after_max_property;
    Property _initial_property;
};


/**
 * @brief Modifies the property by delta when a given amount of update calls are done.
 *
 * @tparam Property Property to modify.
 * @tparam PropertyManager Reads and writes the property to modify.
 *
 * @ingroup template_action
 */
template<typename Property, class PropertyManager>
class duration_by_template_action
{

public:
    /**
     * @brief Resets the property to its initial state.
     */
    void reset()
    {
        PropertyManager::set(_initial_property);
        _current_update = 0;
    }

    /**
     * @brief Adds delta_property to the property when the given amount of update calls are done.
     */
    void update()
    {
        if(_current_update == _duration_updates - 1)
        {
            PropertyManager::set(PropertyManager::get() + _delta_property);
            _current_update = 0;
        }
        else
        {
            ++_current_update;
        }
    }

    /**
     * @brief Returns how much update calls have to be done before updating the property.
     */
    [[nodiscard]] int duration_updates() const
    {
        return _duration_updates;
    }

protected:
    /**
     * @brief Constructor.
     * @param duration_updates How much update calls have to be done before updating the property.
     * @param delta_property How much to add to the property when update is called duration_updates times.
     */
    duration_by_template_action(int duration_updates, const Property& delta_property) :
        _delta_property(delta_property),
        _initial_property(PropertyManager::get()),
        _duration_updates(duration_updates)
    {
        BTN_ASSERT(duration_updates > 0, "Invalid duration updates: ", duration_updates);
    }

    /**
     * @brief Returns how much to add to the property when update is called duration_updates times.
     */
    [[nodiscard]] const Property& delta_property() const
    {
        return _delta_property;
    }

private:
    uint16_t _current_update = 0;
    Property _delta_property;
    Property _initial_property;
    int _duration_updates;
};


/**
 * @brief Modifies the property by delta when a given amount of update calls are done.
 * When the property is over the given maximum, it goes back to the given minimum and vice versa.
 *
 * @tparam Property Property to modify.
 * @tparam PropertyManager Reads and writes the property to modify.
 *
 * @ingroup template_action
 */
template<typename Property, class PropertyManager>
class cyclic_duration_by_template_action
{

public:
    /**
     * @brief Resets the property to its initial state.
     */
    void reset()
    {
        PropertyManager::set(_initial_property);
    }

    /**
     * @brief Adds delta_property to the property when the given amount of update calls are done.
     *
     * When the property reaches the maximum, it goes back to the minimum and vice versa.
     */
    void update()
    {
        if(_current_update == _duration_updates - 1)
        {
            Property new_property = PropertyManager::get() + _delta_property;

            if(new_property < _min_property)
            {
                new_property += _after_max_property - _min_property;
            }
            else if(new_property >= _after_max_property)
            {
                new_property -= _after_max_property - _min_property;
            }

            PropertyManager::set(new_property);
            _current_update = 0;
        }
        else
        {
            ++_current_update;
        }
    }

    /**
     * @brief Returns how much update calls have to be done before updating the property.
     */
    [[nodiscard]] int duration_updates() const
    {
        return _duration_updates;
    }

protected:
    /**
     * @brief Constructor.
     * @param duration_updates How much update calls have to be done before updating the property.
     * @param delta_property How much to add to the property when update is called duration_updates times.
     * @param min_property Minimum property.
     * @param after_max_property Immediate amount after the maximum property.
     */
    cyclic_duration_by_template_action(int duration_updates, const Property& delta_property,
                                       const Property& min_property, const Property& after_max_property) :
        _delta_property(delta_property),
        _min_property(min_property),
        _after_max_property(after_max_property),
        _initial_property(PropertyManager::get()),
        _duration_updates(duration_updates)
    {
        BTN_ASSERT(duration_updates > 0, "Invalid duration updates: ", duration_updates);
    }

    /**
     * @brief Returns how much to add to the property when update is called duration_updates times.
     */
    [[nodiscard]] const Property& delta_property() const
    {
        return _delta_property;
    }

    /**
     * @brief Sets the minimum property.
     */
    void set_min_property(const Property& min_property)
    {
        _min_property = min_property;
    }

    /**
     * @brief Sets the immediate amount after the maximum property.
     */
    void set_after_max_property(const Property& after_max_property)
    {
        _after_max_property = after_max_property;
    }

private:
    uint16_t _current_update = 0;
    Property _delta_property;
    Property _min_property;
    Property _after_max_property;
    Property _initial_property;
    int _duration_updates;
};


/**
 * @brief Modifies the property until it has a given state.
 *
 * @tparam Property Property to modify.
 * @tparam PropertyManager Reads and writes the property to modify.
 *
 * @ingroup template_action
 */
template<typename Property, class PropertyManager>
class to_template_action
{

public:
    /**
     * @brief Resets the property to its initial state.
     */
    void reset()
    {
        PropertyManager::set(_initial_property);
        _current_update = 0;
    }

    /**
     * @brief Adds ((final_property - initial_property) / duration_updates) to the property
     * until it is equal to final_property.
     */
    void update()
    {
        BTN_ASSERT(! done(), "Action is done");

        ++_current_update;

        if(_current_update == _duration_updates)
        {
            PropertyManager::set(_final_property);
        }
        else
        {
            PropertyManager::set(PropertyManager::get() + _delta_property);
        }
    }

    /**
     * @brief Indicates if update has been called duration_updates times.
     */
    [[nodiscard]] bool done() const
    {
        return _current_update == _duration_updates;
    }

    /**
     * @brief Returns the number of times that update has to be called until the property is equal to final_property.
     */
    [[nodiscard]] int duration_updates() const
    {
        return _duration_updates;
    }

protected:
    /**
     * @brief Constructor.
     * @param duration_updates Number of times that update has to be called
     * until the property is equal to final_property.
     * @param final_property Property when update is called duration_updates times.
     */
    to_template_action(int duration_updates, const Property& final_property) :
        _final_property(final_property),
        _initial_property(PropertyManager::get()),
        _delta_property(_calculate_delta_property(duration_updates)),
        _duration_updates(duration_updates)
    {
    }

    /**
     * @brief Returns the state of property when update is called duration_updates times.
     */
    [[nodiscard]] const Property& final_property() const
    {
        return _final_property;
    }

private:
    uint16_t _current_update = 0;
    Property _final_property;
    Property _initial_property;
    Property _delta_property;
    int _duration_updates;

    [[nodiscard]] Property _calculate_delta_property(int duration_updates) const
    {
        BTN_ASSERT(duration_updates > 0, "Invalid duration updates: ", duration_updates);
        BTN_ASSERT(duration_updates <= numeric_limits<decltype(_current_update)>::max(),
                   "Too much duration updates: ", duration_updates);

        return (_final_property - _initial_property) / duration_updates;
    }
};


/**
 * @brief Modifies the property by delta from a minimum to a maximum.
 * When the property is equal to the given final state, it goes back to its initial state and vice versa.
 *
 * @tparam Property Property to modify.
 * @tparam PropertyManager Reads and writes the property to modify.
 *
 * @ingroup template_action
 */
template<typename Property, class PropertyManager>
class loop_template_action
{

public:
    /**
     * @brief Resets the property to its initial state.
     */
    void reset()
    {
        PropertyManager::set(_initial_property);
        _current_update = 0;
        _reverse = false;
    }

    /**
     * @brief Adds or subtracts ((final_property - initial_property) / duration_updates) to the property.
     *
     * When the property is equal to final_property, it goes back to its initial state and vice versa.
     */
    void update()
    {
        ++_current_update;

        if(_current_update == _duration_updates)
        {
            _current_update = 0;

            if(_reverse)
            {
                PropertyManager::set(_initial_property);
                _reverse = false;
            }
            else
            {
                PropertyManager::set(_final_property);
                _reverse = true;
            }
        }
        else
        {
            if(_reverse)
            {
                PropertyManager::set(PropertyManager::get() - _delta_property);
            }
            else
            {
                PropertyManager::set(PropertyManager::get() + _delta_property);
            }
        }
    }

    /**
     * @brief Returns how much update calls have to be done before changing the direction of the property delta.
     */
    [[nodiscard]] int duration_updates() const
    {
        return _duration_updates;
    }

protected:
    /**
     * @brief Constructor.
     * @param duration_updates How much update calls have to be done
     * before changing the direction of the property delta.
     * @param final_property When the property is equal to this parameter,
     * it goes back to its initial state and vice versa.
     */
    loop_template_action(int duration_updates, const Property& final_property) :
        _final_property(final_property),
        _initial_property(PropertyManager::get()),
        _delta_property(_calculate_delta_property(duration_updates)),
        _duration_updates(duration_updates)
    {
    }

    /**
     * @brief When the property is equal to the returned parameter,
     * it goes back to its initial state and vice versa.
     */
    [[nodiscard]] const Property& final_property() const
    {
        return _final_property;
    }

private:
    bool _reverse = false;
    uint16_t _current_update = 0;
    Property _final_property;
    Property _initial_property;
    Property _delta_property;
    int _duration_updates;

    [[nodiscard]] Property _calculate_delta_property(int duration_updates) const
    {
        BTN_ASSERT(duration_updates > 0, "Invalid duration updates: ", duration_updates);
        BTN_ASSERT(duration_updates <= numeric_limits<decltype(_current_update)>::max(),
                   "Too much duration updates: ", duration_updates);

        return (_final_property - _initial_property) / duration_updates;
    }
};


/**
 * @brief Changes the property when a given amount of update calls are done.
 *
 * @tparam Property Property to modify.
 * @tparam PropertyManager Reads and writes the property to modify.
 *
 * @ingroup template_action
 */
template<typename Property, class PropertyManager>
class toggle_template_action
{

public:
    /**
     * @brief Resets the property to its initial state.
     */
    void reset()
    {
        PropertyManager::set(_initial_property);
        _current_update = 0;
        _reverse = false;
    }

    /**
     * @brief Sets the property to new_property or to its initial state when the given amount of update calls are done.
     */
    void update()
    {
        ++_current_update;

        if(_current_update == _duration_updates)
        {
            _current_update = 0;

            if(_reverse)
            {
                PropertyManager::set(_initial_property);
                _reverse = false;
            }
            else
            {
                PropertyManager::set(_new_property);
                _reverse = true;
            }
        }
    }

    /**
     * @brief Returns how much update calls have to be done to change the property.
     */
    [[nodiscard]] int duration_updates() const
    {
        return _duration_updates;
    }

protected:
    /**
     * @brief Constructor.
     * @param duration_updates How much update calls have to be done to change the property.
     * @param new_property New state for the property to set when update is called duration_updates times.
     */
    toggle_template_action(int duration_updates, const Property& new_property) :
        _initial_property(PropertyManager::get()),
        _new_property(new_property),
        _duration_updates(duration_updates)
    {
        BTN_ASSERT(duration_updates > 0, "Invalid duration updates: ", duration_updates);
    }

    /**
     * @brief Returns the new state for the property when update is called duration_updates times.
     */
    [[nodiscard]] const Property& new_property() const
    {
        return _new_property;
    }

private:
    bool _reverse = false;
    uint16_t _current_update = 0;
    Property _initial_property;
    Property _new_property;
    int _duration_updates;
};


/**
 * @brief Toggles a boolean property when a given amount of update calls are done.
 *
 * @tparam PropertyManager Reads and writes the property to modify.
 *
 * @ingroup template_action
 */
template<class PropertyManager>
class bool_toggle_template_action
{

public:
    /**
     * @brief Resets the property to its initial state.
     */
    void reset()
    {
        PropertyManager::set(_initial_property);
        _current_update = 0;
        _reverse = false;
    }

    /**
     * @brief Toggles the property when the given amount of update calls are done.
     */
    void update()
    {
        ++_current_update;

        if(_current_update == _duration_updates)
        {
            _current_update = 0;

            if(_reverse)
            {
                PropertyManager::set(_initial_property);
                _reverse = false;
            }
            else
            {
                PropertyManager::set(! _initial_property);
                _reverse = true;
            }
        }
    }

    /**
     * @brief Returns how much update calls have to be done to toggle the property.
     */
    [[nodiscard]] int duration_updates() const
    {
        return _duration_updates;
    }

protected:
    /**
     * @brief Constructor.
     * @param duration_updates How much update calls have to be done to toggle the property.
     */
    explicit bool_toggle_template_action(int duration_updates) :
        _initial_property(PropertyManager::get()),
        _duration_updates(duration_updates)
    {
        BTN_ASSERT(duration_updates > 0, "Invalid duration updates: ", duration_updates);
    }

private:
    bool _reverse = false;
    bool _initial_property;
    uint16_t _current_update = 0;
    int _duration_updates;
};

}

#endif
