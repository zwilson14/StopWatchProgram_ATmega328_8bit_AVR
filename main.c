/*****************************************************************
* Name        : <Zach Wilson>
* Program     : Program <6>
* Class       : ENGE 320
* Date        : <10/17/16>
* Description : <fully functioning stopwatch>
* =============================================================
* Program Grading Criteria
* =============================================================
* Stopwatch displays on the seven seg: (10) ____
* Display doesn’t ghost: (10) ____
* Stopwatch timer operates at the correct speed:  (10) ____
* Start/Stop button works:  (10) ____
* Stop time displayed for 2 seconds (if display set to a lap time):  (10) ____
* Lap button works:  (10) ____
* Lap time displayed for 2 seconds:  (10) ____
* Potentiometer controls the Lap time display:  (10) ____
* Lap times queue properly:  (10) ____
* LEDs indicate the Lap time: (10) ___
* LEDs fade properly at 250ms: (20) ____
* Serial port functions properly at 1MHz: (10) ____
* Serial start prints properly (10) ____
* Serial lap prints properly (10) ____
* Serial stop prints properly (10) ____
* Pressing “s” performs a start/stop: (10) ____
* Pressing “l” performs a lap: (10) ____
* Messages don’t cause system delay 1MHz: (10) ____
* =============================================================
*
* TOTAL  (200)_________
*
* =============================================================
*****************************************************************/

//------------------------------------------------------------------------------
//             __             __   ___  __
//     | |\ | /  ` |    |  | |  \ |__  /__`
//     | | \| \__, |___ \__/ |__/ |___ .__/
//
//------------------------------------------------------------------------------

#include "display.h"
#include "timer.h"
#include <util/atomic.h>
#include <avr/interrupt.h>
#include "Decoder.h"
#include "switch.h"
#include "adc.h"
#include "led.h"
#include "pwm.h"
#include "buttons.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "serial.h"


//------------------------------------------------------------------------------
//      __   ___  ___         ___  __
//     |  \ |__  |__  | |\ | |__  /__`
//     |__/ |___ |    | | \| |___ .__/
//
//------------------------------------------------------------------------------

#define MAX_STRING (4)
#define MAX_STRING_SIZE (500)

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

static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static char my_string[MAX_STRING_SIZE];
static char read_save;

// Display counter
uint64_t prev_seg_counter = 0;
uint64_t seg_counter = 0;

// Stopwatch Counter
uint64_t counter = 0;
uint64_t prev_counter = 0;

// Pwm counter
uint64_t pwm_counter = 0;
uint64_t prev_pwm_counter = 0;

// Button Values:
uint8_t start = 0;
uint8_t lap = 0;
uint8_t stop = 0;

// if start has been pressed and is not stopped
uint8_t run = 0;
uint8_t reset_1 = 0;
uint8_t reset_2 = 0;

// Lap pointer determines where to display and save
uint8_t lap_pointer = 0;
uint8_t lap_counter = 0;

// This flags the program to initiate the 2 second display of lap time
uint8_t lap_flag = 0;

// counter starts a two second display of the new lap time
uint16_t lap_timer = 0;
uint16_t prev_lap_timer = 0;


// Anode value adjustments (digits to display)
// use anode quarter to determine which one to display and
// anode times to save into lap
// use lap pointer to index display and save
uint8_t anode1[MAX_STRING];
uint8_t anode2[MAX_STRING];
uint8_t anode3[MAX_STRING];
uint8_t anode4[MAX_STRING];

// These are the lap and timer values to be incremented.
uint64_t main_time = 0;
uint64_t lap_save = 0;
uint64_t prev_lap_save = 0;
uint64_t main_mod = 0;
uint64_t stop_time = 0;
uint64_t main_alt_time = 0;

// Alternating variable for which anode to turn on
uint8_t step = 1;

// These are button values:
bool button[2] = {false,false};
bool old_button[2] = {false, false};
uint64_t timerval_old, timerval_new;

uint8_t button_reset;

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

int main(void)
{
  
  // Initializations:
  display_init();
  timer_init();
  switch_init();
  adc_init();
  led_init();
  pwm_init();
  buttons_init();
  serial_init();
  stdout = &mystdout;
  sei();
  
  timerval_new = timerval_old = timer_get();
  
  
  // Main Loop:
  while(1)
  {    
    
    // Check serial input
    read_save = serial_read();
    
    // Checks if the +/- keys have been pressed
    if ( 's' == read_save)
    {
      start = 1;
    }
    
    if ('l' == read_save)
    {
      lap = 1;
    }
    
    // Clear read save
    read_save = 0;    
    
    ////////////////////////////
    
    if(switch_get())
    {
      // PWM adjustment Loop:
      pwm_counter = timer_get();
      
      if (20 < (pwm_counter - prev_pwm_counter))
      {
        DDRB |= (0x02);
        DDRD |= (0x60);
        update_pwm(adc_get_quarter());
        prev_pwm_counter = pwm_counter;
      }
    }
    
    else
    {
      DDRB &= ~(0x02);
      DDRD &= ~(0x60);
      update_pwm(4);
    }
    /////////////////////////////
    
    
    ///////////////////////////
    //   Stop watch Counter  //
    ///////////////////////////
    
    // Get the time
    timerval_new = timer_get();
    
    // See if the buttons have been pressed
    for (int i = 0; i < 2; i++)
    {
      button[i] = buttons_get_debounce(i, timerval_new);
      
      // Just look for the falling edge
      if ((true == button[i]) && (false == old_button[i]))
      {
        if (0 == i)
        {
          start = 1;
        }
        if (1 == i)
        {
          lap = 1;
        }
        
        
        if ( button[0] == button[1] == 1)
        {
          button_reset = 1;
        }
        
      }
      
      // Save the button val
      old_button[i] = button[i];

    }
    
    // if start is pressed, toggle run
    if (start)
    {
      if (run)
      {
        run = 0;
        stop = 1;
        lap_counter++;  
      }
      
      else
      {
        // Print the delay
        sprintf(my_string, "Stopwatch Started\r\n");
        serial_string_write(my_string);
        run = 1;
        stop = 0;
        
        // print that it has begun
        //printf("StopWatch Started \n\r");
      }
      start = 0;
    }
    
    
    // If reset, kill all values.
    if(button_reset)
    {
      for (int i = 0; i < 4; i++)
      {
        anode1[i] = 0;
        anode2[i] = 0;
        anode3[i] = 0;
        anode4[i] = 0;
      }
      button_reset = 0;
      start = 0;
      run = 0;
      lap = 0;
      lap_flag = 0;
      lap_pointer = 0;
      stop = 0;
      lap_save = 0;
      main_time = 0;
      lap_counter = 0;
    }
    
    // If button to start has been initiated
    if(run)
    {
      
    
      ///////////////////////////////////////
      counter = timer_get();
      
      if (100 < (counter - prev_counter))
      {
        
        lap_save++;
        main_time++;

		main_mod = main_time;
		stop_time = main_time;
		 //////////////////////////////////////////////
		 
		 // Split of the lap_save value:
		 // Put in the anode arrays
		 
		 // Sepparate the lowest val
		 anode1[0] = (main_mod % 10);
		 
		 main_mod = (main_mod / 10);
		 
		 // Sepparate the 10s place
		 anode2[0] = (main_mod % 10);
		 
		 main_mod = (main_mod / 10);
		 
		 //sepparate 100s place
		 anode3[0] = (main_mod % 10);
		 
		 main_mod = (main_mod / 10);
		 
		 //Sepparate 1000s place // mod 10?
		 anode4[0] = (main_mod);

		 //////////////////////////////////////////////
        
        prev_counter = counter;
      }
      
      // Check to save lap times
      if(lap)
      {
        
        lap_counter++;
        
        // Digit 1------------
        // shift values in array:
        // [3] = lap 3, [0] = display val
        anode1[3] = anode1[2];
        anode1[2] = anode1[1];
        
        // Digit 2------------
        // shift values in array:
        // [3] = lap 3, [0] = display val
        anode2[3] = anode2[2];
        anode2[2] = anode2[1];
        
        // Digit 3------------
        // shift values in array:
        // [3] = lap 3, [0] = display val
        anode3[3] = anode3[2];
        anode3[2] = anode3[1];
        
        // Digit 4------------
        // shift values in array:
        // [3] = lap 3, [0] = display val
        anode4[3] = anode4[2];
        anode4[2] = anode4[1];

 ////////////////////////////////////////////// 
         
        // Split of the lap_save value:
        // Put in the anode arrays
        
        // Sepparate the lowest val
        anode1[1] = (lap_save % 10);
        
        lap_save = (lap_save / 10);
        
        // Sepparate the 10s place
        anode2[1] = (lap_save % 10);
        
        lap_save = (lap_save / 10);
        
        //sepparate 100s place
        anode3[1] = (lap_save % 10);
        
        lap_save = (lap_save / 10);
        
        //Sepparate 1000s place // mod 10?
        anode4[1] = (lap_save);

 //////////////////////////////////////////////       
        
        // Print the delay
        sprintf(my_string, "Lap%d: %d%d.%d%d\r\n", lap_counter, anode4[1], anode3[1], anode2[1], anode1[1]);
        serial_string_write(my_string);          
        
        lap_flag = 1;		  

        lap = 0;
        lap_save = 0;
        prev_lap_timer = timer_get();
        
      }
      
    }
    

    // Put a timer that determines lap pointer so that
    if (lap_flag)
    {
      
      lap_timer = timer_get();
      if (18500 < (lap_timer- prev_lap_timer))
      {
        // signal that the 2 second wait is done, and start to display
        // the current time again
        lap_flag = 0;
        lap_pointer = 0;
        lap_timer = 0;
        prev_lap_timer = 0;
        
      }
      
      else
      {
        // display the saved lap time
        lap_pointer = 1;
      }
      //prev_lap_timer = lap_timer;
    }

	  // this saves the final display time as lap1... if dif just change
	  // the stop time variable
	  if (stop)
	  {
	    ////////////////////////////////////////////// 
        
      // Digit 1------------
      // shift values in array:
      // [3] = lap 3, [0] = display val
      anode1[3] = anode1[2];
      anode1[2] = anode1[1];
        
      // Digit 2------------
      // shift values in array:
      // [3] = lap 3, [0] = display val
      anode2[3] = anode2[2];
      anode2[2] = anode2[1];
        
      // Digit 3------------
      // shift values in array:
      // [3] = lap 3, [0] = display val
      anode3[3] = anode3[2];
      anode3[2] = anode3[1];
        
      // Digit 4------------
      // shift values in array:
      // [3] = lap 3, [0] = display val
      anode4[3] = anode4[2];
      anode4[2] = anode4[1];
         
      // Split of the lap_save value:
      // Put in the anode arrays
        
      // Sepparate the lowest val
      anode1[1] = (lap_save % 10);
        
      lap_save = (lap_save / 10);
        
      // Sepparate the 10s place
      anode2[1] = (lap_save % 10);
        
      lap_save = (lap_save / 10);
         
      //sepparate 100s place
      anode3[1] = (lap_save % 10);
        
      lap_save = (lap_save / 10);
          
      //Sepparate 1000s place // mod 10?
      anode4[1] = (lap_save);

      // Print the delay
      sprintf(my_string, "Lap%d: %d%d.%d%d\r\n", lap_counter, anode4[1], anode3[1], anode2[1], anode1[1]);
      serial_string_write(my_string);          
      // Print the delay
      sprintf(my_string, "Final Time: %d%d.%d%d\r\nStopwatch Complete\r\n\n", anode4[0], anode3[0], anode2[0], anode1[0]);
      serial_string_write(my_string);   
      
//////////////////////////////////////////////  
//////////////////////////////////////////////  
	}
    
    // Double check that values are cleared
    start = 0;
    stop = 0;
    lap = 0;
    
    // This is to change which lap time to be displayed:
    // as long as the two second lap init display isn't in progress
    if (!lap_flag)
    {
      lap_pointer = adc_get_quarter();
    }
    
    
    
    
    // Seven Seg////////////////////
    
    // Anodes #'s from left to right:
    //    |4| * |3| *  |2| * |1|    
    
    if (0 == get_busy_flag()) // means not busy
    {
      
      seg_counter = timer_get();
      
      if (2 < (seg_counter - prev_seg_counter))
      {
        
        if (1 == step)
        {
          display_write(0x00, decoder(anode1[lap_pointer]), 4);
        }
        
        else if (2 == step)
        {
          display_write(0x00, decoder(anode2[lap_pointer]), 3);
        }
        
        else if (3 == step)
        {
          display_write(0x00,  (1 << 7) | (decoder(anode3[lap_pointer])), 2);
        }
        
        else if (4 == step)
        {
          display_write(0x00, (decoder(anode4[lap_pointer])), 1);
        }
        step ++;
        if (5 == step)
        {
          step = 1;
        }
        
        prev_seg_counter = timer_get();
      }
    }
  }
}

//------------------------------------------------------------------------------
//      __   __              ___  ___
//     |__) |__) | \  /  /\   |  |__
//     |    |  \ |  \/  /~~\  |  |___
//
//------------------------------------------------------------------------------

static int uart_putchar(char c, FILE *stream)
{
  serial_write(c);
  return 0;
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

