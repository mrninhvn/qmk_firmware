/*
Copyright 2021 mrninhvn (https://github.com/mrninhvn)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <stdio.h>
#include "analog.h"

#define POT_TOLERANCE 12

#define POT_A_INPUT A1
#define POT_B_INPUT A0

// Constant value definitions

#define ADC_HYSTERESIS  20        // Must be 1 or higher. Noise filter, determines how big ADC change needed
#define POT_SENSITIVITY 8        // Higher number = more turns needed to reach max value

#define ADC_MAX_VALUE 1023

// Variables for potmeter
int ValuePotA = 0;            // Pot1 tap A value
int ValuePotB = 0;            // Pot1 tap B value
int PreviousValuePotA = 0;    // Used to remember last value to determine turn direction
int PreviousValuePotB = 0;    // Used to remember last value to determine turn direction
int8_t DirPotA = 1;              // Direction for Pot 1 tap A
int8_t DirPotB = 1;              // Direction for Pot1 tap B

int8_t PotDirection  = 1;        // Final CALCULATED direction
int PotValue = 0;
int OldPotValue = 0;

void read_pot(void) {
	ValuePotA  = analogReadPin(POT_A_INPUT);
  ValuePotB  = analogReadPin(POT_B_INPUT);
	/****************************************************************************
  * Step 1 decode each  individual pot tap's direction
  ****************************************************************************/
  // First check direction for Tap A
  if (ValuePotA > (PreviousValuePotA + ADC_HYSTERESIS))       // check if new reading is higher (by <debounce value>), if so...
  {
    DirPotA = 1;                                              // ...direction of tap A is up
  }
  else if (ValuePotA < (PreviousValuePotA - ADC_HYSTERESIS))  // check if new reading is lower (by <debounce value>), if so...
  {
    DirPotA = -1;                                             // ...direction of tap A is down
  }
  else
  {
    DirPotA = 0;                                              // No change
  }
  // then check direction for tap B
  if (ValuePotB > (PreviousValuePotB + ADC_HYSTERESIS))       // check if new reading is higher (by <debounce value>), if so...
  {
    DirPotB = 1;                                              // ...direction of tap B is up
  }
  else if (ValuePotB < (PreviousValuePotB - ADC_HYSTERESIS))  // check if new reading is lower (by <debounce value>), if so...
  {
    DirPotB = -1;                                             // ...direction of tap B is down
  }
  else
  {
    DirPotB = 0;                                              // No change
  }

  /****************************************************************************
  * Step 2: Determine actual direction of ENCODER based on each individual
  * potentiometer tap´s direction and the phase
  ****************************************************************************/
  if (DirPotA == -1 && DirPotB == -1)       //If direction of both taps is down
  {
    if (ValuePotA > ValuePotB)               // If value A above value B...
    {
      PotDirection = 1;                         // ...direction is up
    }
    else
    {
      PotDirection = -1;                        // otherwise direction is down
    }
  }
  else if (DirPotA == 1 && DirPotB == 1)    //If direction of both taps is up
  {
    if (ValuePotA < ValuePotB)               // If value A below value B...
    {
      PotDirection = 1;                         // ...direction is up
    }
    else
    {
      PotDirection = -1;                        // otherwise direction is down
    }
  }
  else if (DirPotA == 1 && DirPotB == -1)   // If A is up and B is down
  {
    if ( (ValuePotA > (ADC_MAX_VALUE/2)) || (ValuePotB > (ADC_MAX_VALUE/2)) )  //If either pot at upper range A/B = up/down means direction is up
    {
      PotDirection = 1;
    }
    else                                     //otherwise if both pots at lower range A/B = up/down means direction is down
    {
      PotDirection = -1;
    }
  }
  else if (DirPotA == -1 && DirPotB == 1)
  {
    if ( (ValuePotA < (ADC_MAX_VALUE/2)) || (ValuePotB < (ADC_MAX_VALUE/2)))   //If either pot  at lower range, A/B = down/up means direction is down
    {
      PotDirection = 1;
    }
    else                                     //otherwise if bnoth pots at higher range A/B = up/down means direction is down
    {
      PotDirection = -1;
    }
  }
  else
  {
    PotDirection = 0;                           // if any of tap A or B has status unchanged (0), indicate unchanged
  }

  /****************************************************************************
  * Step 3: Calculate value based on direction, how big change in ADC value,
  * and sensitivity. Avoid values around zero and max  as value has flat region
  ****************************************************************************/
  if (DirPotA != 0 && DirPotB != 0)         //If both taps indicate movement
  {
    if ((ValuePotA < ADC_MAX_VALUE*0.8) && (ValuePotA > ADC_MAX_VALUE*0.2))         // if tap A is not at endpoints
    {
      PotValue = PotValue + PotDirection*abs(ValuePotA - PreviousValuePotA)/POT_SENSITIVITY; //increment value
    }
    else                                    // If tap A is close to end points, use tap B to calculate value
    {
      PotValue = PotValue + PotDirection*abs(ValuePotB - PreviousValuePotB)/POT_SENSITIVITY;  //Make sure to add/subtract at least 1, and then additionally the jump in voltage
    }
    PreviousValuePotA = ValuePotA;          // Update previous value variable
    PreviousValuePotB = ValuePotB;          // Update previous value variable
  }
}

void matrix_init_user(void) {
	PreviousValuePotA  = analogReadPin(POT_A_INPUT);
  PreviousValuePotB  = analogReadPin(POT_B_INPUT);
}

void matrix_scan_user(void) {
	read_pot();
	if (abs(PotValue - OldPotValue) > POT_TOLERANCE) {
		if (PotDirection == 1) {
			tap_code_delay(KC_VOLD, 10);
		}
		else if (PotDirection == -1) {
			tap_code_delay(KC_VOLU, 10);
		}
		OldPotValue = PotValue;
    PotValue = 0;
  }
}