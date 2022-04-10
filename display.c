//------------------------------------------------------------------------------
//             __             __   ___  __
//     | |\ | /  ` |    |  | |  \ |__  /__`
//     | | \| \__, |___ \__/ |__/ |___ .__/
//
//------------------------------------------------------------------------------

#include "display.h"
#include <avr/io.h>
#include <avr/interrupt.h>

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

uint8_t busy = 0;
static uint8_t save = 0;
uint8_t first = 0;
static volatile uint8_t anode_save = 0;

//------------------------------------------------------------------------------
//      __   __   __  ___  __  ___      __   ___  __
//     |__) |__) /  \  |  /  \  |  \ / |__) |__  /__`
//     |    |  \ \__/  |  \__/  |   |  |    |___ .__/
//
//------------------------------------------------------------------------------
/*
void spi_init();
void display_init();
static uint8_t spi (uint8_t led, uint8_t digit);
void display_write(uint8_t led, uint8_t number, uint8_t anode);
*/

static void spi (uint8_t led, uint8_t digit, uint8_t anode);


//------------------------------------------------------------------------------
//      __        __          __
//     |__) |  | |__) |    | /  `
//     |    \__/ |__) |___ | \__,
//
//------------------------------------------------------------------------------

// Main function to write values
void display_write(uint8_t led, uint8_t number, uint8_t anode)
{    
  // Send in specified value
  spi(led, number, anode);
  
  // save anode to be turned on
  anode_save = anode;
}

//=============================================================================

uint8_t get_anode_save() // Make atomic???
{  
  return anode_save;
}

//=============================================================================

//=============================================================================

uint8_t get_busy_flag() // Make atomic???
{  
  return busy;
}

//=============================================================================
void display_init()
{
  
  // Calls the function that initializes the MISO/MOSI/ect.
  spi_init();
  
  // Set the direction of the anode pins
  DDRC |= (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);
  
  // Turn on the Anodes (active low)
  PORTC &= ~(1 << 1);
  PORTC &= ~(1 << 2);
  PORTC &= ~(1 << 3);
  PORTC &= ~(1 << 4);
  
  // Turn off the Anodes (active low)
  PORTC |= (1 << 1);
  PORTC |= (1 << 2);
  PORTC |= (1 << 3);
  PORTC |= (1 << 4);
  
  //Initialize the blank
  //active low so if high everything is dead.
  DDRD |= (1 << 7);
  PORTD &= ~(1 << 7); // allows lights on
  
  // Set The Latch
  DDRC |= (1 << 5);
  PORTC |= (1 << 5); // turn on latch
  PORTC &= ~(1 << 5); // turn off latch
}
//==============================================================================

void anode_set(uint8_t anode)
{  
  PORTC &= ~(1 << anode);
}

//------------------------------------------------------------------------------
//      __   __              ___  ___
//     |__) |__) | \  /  /\   |  |__
//     |    |  \ |  \/  /~~\  |  |___
//
//------------------------------------------------------------------------------

//=============================================================================
// This sets up the SPI on the board
void spi_init()
{

  // Set up MOSI - PB3, SCK - PB5 as outputs
  PORTB &= ~((1 << 3) | (1 << 5));
  DDRB |= ((1 << 3) | (1 << 5));
  
  // Set up MISO - PB4 as an input
  DDRB &= ~(1 << 4);
  
  // Set up SS as an output (pb2), even though
  // we aren't using it - see 19.3.2 of the data sheet
  DDRB |= ( 1 << 2);

  // Set up the SPI
  // SPI Type: Master
  // SPI Clock Rate: 250 KHz
  // SPI clock phse: cycle Start-rising edge
  // SPI Clock Polarity: Low
  // SPI Data ORder: MSB First
  SPCR = 0xD2; // 0b11010010 turns on SPIE, 52 for without
  SPSR = 0x00;
  SPSR |= (1 << 7); // turn on the SPIF Flag
}
// SPI is now set up
//==========================================================================

static void spi (uint8_t led, uint8_t digit, uint8_t anode)
{  
  SPDR = digit;
  save = led;
  busy = 1;
  first = 1;
    // Turn off the Anodes (active low)
    //PORTC &= ~(1 << (anode - 1));
    
      // Turn off the Anodes (active low)
      PORTC |= (1 << 1);
      PORTC |= (1 << 2);
      PORTC |= (1 << 3);
      PORTC |= (1 << 4);
}

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

ISR(SPI_STC_vect)
{
  if (1 == first)
  {
    SPDR = save;
    first = 0;
  }
  
  else
  {
    busy = 0;
    
    // Pulse Latch
    PORTC |= (1 << 5);// turn latch on
    PORTC &= ~(1 << 5);// Turn off latch
    
    // Now that busy flag was cleared turn anode on
    anode_set(anode_save);
        
  }
}