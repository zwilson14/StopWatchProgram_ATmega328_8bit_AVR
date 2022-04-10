//------------------------------------------------------------------------------
//             __             __   ___  __
//     | |\ | /  ` |    |  | |  \ |__  /__`
//     | | \| \__, |___ \__/ |__/ |___ .__/
//
//------------------------------------------------------------------------------

#include "buttons.h"
#include <avr/io.h>

//------------------------------------------------------------------------------
//      __   ___  ___         ___  __
//     |  \ |__  |__  | |\ | |__  /__`
//     |__/ |___ |    | | \| |___ .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//     ___      __   ___  __   ___  ___  __
//      |  \ / |__) |__  |  \ |__  |__  /__`
//      |   |  |    |___ |__/ |___ |    .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                __          __        ___  __
//     \  /  /\  |__) |  /\  |__) |    |__  /__`
//      \/  /~~\ |  \ | /~~\ |__) |___ |___ .__/
//
//------------------------------------------------------------------------------

static const uint8_t DEBOUNCE_TIME = 20;

//------------------------------------------------------------------------------
//      __   __   __  ___  __  ___      __   ___  __
//     |__) |__) /  \  |  /  \  |  \ / |__) |__  /__`
//     |    |  \ \__/  |  \__/  |   |  |    |___ .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      __        __          __
//     |__) |  | |__) |    | /  `
//     |    \__/ |__) |___ | \__,
//
//------------------------------------------------------------------------------

//=============================================================================
void buttons_init()
{
  DDRD &= ~(0x04); // Make sure PD2 and PD3 are inputs
}

//==============================================================================
// Buttons are active low
// Button 0 is on PD2
// Button 1 is on PD3
//==============================================================================
bool buttons_get(uint8_t button)
{
  bool retval = false;
  if (button == 0)
  {
    if ((PIND & (1 << 2)) == 0) // active low
    {
      retval = true;
    }
  }
  else // Button 1
  {
    if ((PIND & (1 << 3)) == 0) // active low
    {
      retval = true;
    }
  }
  return retval;
}

//==============================================================================
// The current system millis timer is passed into the routine.
// The assumption is that we are being called more frequently than the
// DEBOUNCE_TIME (otherwise, why debounce?)
// So - we will limit the calls to once ever DEBOUNCE_TIME so that we are
// guaranteed to miss any bounces. Make our DEBOUNCE_TIME greater than the
// maximum bounce timer, and shorter than the minimum stable time so that we
// never miss a button press.
//
//                         Minimum Stable Time
//              <--------------------------------------->
//      Max Bounce
//     <---------->
//         _    _________________________________________   _
// _______| |__|                                         |_| |________________
//
//    <------------------>
//       Debounce Time
//
// So, if we are reading the button every debounce time, the following will
// occur:
// Reads before press - 0
// Read during bounce - 0 or 1
// Read after bounce - 1
// So - we wither get 0,0,1 or 0,1,1 - that is, i we catch the first bounce,
// it looks like it happened one DEBOUNCE_TIME faster
//==============================================================================
bool buttons_get_debounce(uint8_t button, uint64_t millis)
{
  // The state of each button
  static bool button_state[2];
  // The time of the last state reading for each button
  static uint64_t old_millis[2] = {0,0};

  // See if it has been longer than the DEBOUNCE_TIME since we changed state
  if ((millis - old_millis[button]) >= DEBOUNCE_TIME)
  {
    // Get the new button
    button_state[button] = buttons_get(button);
    // set the timer to the new value
    old_millis[button] = millis;
  }
  else
  {
    // We only want to read once every debounce cycle,
    // just return the existing state
    ;
  }
  return button_state[button];
}
//------------------------------------------------------------------------------
//      __   __              ___  ___
//     |__) |__) | \  /  /\   |  |__
//     |    |  \ |  \/  /~~\  |  |___
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//        __   __  , __
//     | /__` |__)  /__`
//     | .__/ |  \  .__/
//
//------------------------------------------------------------------------------
