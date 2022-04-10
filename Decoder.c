//------------------------------------------------------------------------------
//             __             __   ___  __
//     | |\ | /  ` |    |  | |  \ |__  /__`
//     | | \| \__, |___ \__/ |__/ |___ .__/
//
//------------------------------------------------------------------------------

#include "Decoder.h"
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

//------------------------------------------------------------------------------
//      __   __   __  ___  __  ___      __   ___  __
//     |__) |__) /  \  |  /  \  |  \ / |__) |__  /__`
//     |    |  \ \__/  |  \__/  |   |  |    |___ .__/
//
//------------------------------------------------------------------------------

uint8_t decoder(uint8_t value);

//------------------------------------------------------------------------------
//      __        __          __
//     |__) |  | |__) |    | /  `
//     |    \__/ |__) |___ | \__,
//
//------------------------------------------------------------------------------

//==============================================================================
uint8_t decoder(uint8_t value)
{
  // This is the decoded value to be sent
  uint8_t decoded_val = 0;
  
  // These ifs process and encode the value
  if (0 == value)
  {
    decoded_val = 0b00111111;
  }
  
  if (1 == value)
  {
    decoded_val = 0b00000110;
  }
  
  if (2 == value)
  {
    decoded_val = 0b01011011;
  }
  
  if (3 == value)
  {
    decoded_val = 0b01001111;
  }
  
  if (4 == value)
  {
    decoded_val = 0b01100110;
  }
  
  if (5 == value)
  {
    decoded_val = 0b01101101;
  }
  
  if (6 == value)
  {
    decoded_val = 0b01111101;
  }
  
  if (7 == value)
  {
    decoded_val = 0b00000111;
  }
  
  if (8 == value)
  {
    decoded_val = 0b01111111;
  }
  
  if (9 == value)
  {
    decoded_val = 0b01100111;
  }
  
  // return the decoded value
  return decoded_val;
}

//------------------------------------------------------------------------------
//      __   __              ___  ___
//     |__) |__) | \  /  /\   |  |__
//     |    |  \ |  \/  /~~\  |  |___
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      __                  __        __        __
//     /  `  /\  |    |    |__)  /\  /  ` |__/ /__`
//     \__, /~~\ |___ |___ |__) /~~\ \__, |  \ .__/
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//        __   __  , __
//     | /__` |__)  /__`
//     | .__/ |  \  .__/
//
//------------------------------------------------------------------------------
