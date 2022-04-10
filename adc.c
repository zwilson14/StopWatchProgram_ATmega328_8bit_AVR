//------------------------------------------------------------------------------
//             __             __   ___  __
//     | |\ | /  ` |    |  | |  \ |__  /__`
//     | | \| \__, |___ \__/ |__/ |___ .__/
//
//------------------------------------------------------------------------------

#include "adc.h"
#include <avr/io.h>
#include "led.h"

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

static volatile uint8_t quarter = 0;

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
void adc_init()
{
  // Set up the registers for ADC/Pot
  DDRC &= ~(0x01);
  ADMUX = (0x40);// 0x60 to left shift, this is normal no shift
  ADCSRB = (0x00);
  ADCSRA = (0xE7);//e3
}

//=============================================================================

// Return what quadrant the potentiometer is in
uint8_t adc_get_quarter()
{
  
  // Get the value of the potentiometer
  uint32_t adc_val = ADC;
  
  if (adc_val <= 256)
  {
    quarter = 0; // first quarter
  }
  
  else if (adc_val <= 512)
  {
    quarter = 1; // 2nd quarter
  }
  
  else if (adc_val <= 724)
  {
    quarter = 2; //3rd quarter
  }
  
  else if( adc_val <= 1023)
  {
    quarter = 3; // 4rth quarter
  }
  
  return quarter; // return quarter
}

//=============================================================================




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
