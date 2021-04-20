/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2015 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_debounce.h : Digital debounce module
    Author(s):      Pieter Conradie
    Creation Date:  2015-08-21

============================================================================= */


/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_debounce.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("debounce");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_debounce_init(px_debounce_t * debounce,
                      bool            initial_state_is_hi)
{
    if(initial_state_is_hi)
    {
        debounce->state   = PX_DEBOUNCE_STATE_HI;
        debounce->counter = PX_DEBOUNCE_CFG_COUNT_MAX;
    }
    else
    {
        debounce->state   = PX_DEBOUNCE_STATE_LO;
        debounce->counter = 0;
    }

    debounce->rising_edge_flag  = false;
    debounce->falling_edge_flag = false;

#if (PX_DEBOUNCE_CFG_LONG_COUNT != 0)
    debounce->counter_long      = 0;
    debounce->long_hi_flag      = false;
    debounce->long_lo_flag      =  false;
#endif
}

void px_debounce_update(px_debounce_t * debounce, 
                        bool            input_is_hi)
{
    if(input_is_hi)
    {
        // Maximum reached?
        if(debounce->counter < PX_DEBOUNCE_CFG_COUNT_MAX)
        {
            // Increment count
            debounce->counter++;
        }
        // High watermark threshold reached?
        if(debounce->counter == PX_DEBOUNCE_CFG_THRESHOLD_HI)
        {
            // Must state be changed?
            if(debounce->state == PX_DEBOUNCE_STATE_LO)
            {
                // Change to HI state
                debounce->state = PX_DEBOUNCE_STATE_HI;
                // Set rising edge flag
                debounce->rising_edge_flag = true;
#if (PX_DEBOUNCE_CFG_LONG_COUNT != 0)
                // (Re)start long counter
                debounce->counter_long = PX_DEBOUNCE_CFG_LONG_COUNT;
#endif
            }
        }
    }
    else
    {
        // Minimum reached?
        if(debounce->counter > 0)
        {
            // Decrement count
            debounce->counter--;
        }
        // Low watermark threshold reached?
        if(debounce->counter == PX_DEBOUNCE_CFG_THRESHOLD_LO)
        {
            // Must state be changed?
            if(debounce->state == PX_DEBOUNCE_STATE_HI)
            {
                // Change to LO state
                debounce->state = PX_DEBOUNCE_STATE_LO;
                // Set falling edge flag
                debounce->falling_edge_flag = true;
#if (PX_DEBOUNCE_CFG_LONG_COUNT != 0)
                // (Re)start long counter
                debounce->counter_long = PX_DEBOUNCE_CFG_LONG_COUNT;
#endif
            }           
        }
    }

#if (PX_DEBOUNCE_CFG_LONG_COUNT != 0)
    // Long counter started?
    if(debounce->counter_long != 0)
    {
        // Long counter expired?
        if(--debounce->counter_long == 0)
        {
            // Is the current debounced state LO?
            if(debounce->state == PX_DEBOUNCE_STATE_LO)
            {
                // Set long LO flag
                debounce->long_lo_flag = true;
            }
            else
            {
                // Set long HI flag
                debounce->long_hi_flag = true;
            }
        }
    }
#endif
}

px_debounce_state_t px_debounce_state(const px_debounce_t * debounce)
{
    return debounce->state;
}

bool px_debounce_rising_edge_detected(px_debounce_t * debounce)
{
    // Flag set?
    if(debounce->rising_edge_flag)
    {
        // Clear flag
        debounce->rising_edge_flag = false;
        // Flag was set
        return true;
    }
    else
    {
        // Flag is not set
        return false;
    }
}

bool px_debounce_falling_edge_detected(px_debounce_t * debounce)
{
    // Flag set?
    if(debounce->falling_edge_flag)
    {
        // Clear flag
        debounce->falling_edge_flag = false;
        // Flag was set
        return true;
    }
    else
    {
        // Flag is not set
        return false;
    }
}

#if (PX_DEBOUNCE_CFG_LONG_COUNT != 0)
bool px_debounce_long_hi_detected(px_debounce_t * debounce)
{
    // Flag set?
    if(debounce->long_hi_flag)
    {
        // Clear flag
        debounce->long_hi_flag = false;
        // Flag was set
        return true;
    }
    else
    {
        // Flag is not set
        return false;
    }
}

bool px_debounce_long_lo_detected(px_debounce_t * debounce)
{
    // Flag set?
    if(debounce->long_lo_flag)
    {
        // Clear flag
        debounce->long_lo_flag = false;
        // Flag was set
        return true;
    }
    else
    {
        // Flag is not set
        return false;
    }
}
#endif
