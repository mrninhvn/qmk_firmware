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

#define POT_TOLERANCE 4

#define POT_A_INPUT A1
#define POT_B_INPUT A0

// Constant value definitions

#define ADC_HYSTERESIS  20       // Must be 1 or higher. Noise filter, determines how big ADC change needed
#define POT_SENSITIVITY 8        // Higher number = more turns needed to reach max value

#define ADC_MAX_VALUE 1023

#define POT_DEBOUNCE 10

/* Variables for potmeter */
int ValuePotA = 0;              // Pot1 tap A value
int ValuePotB = 0;              // Pot1 tap B value
int PreviousValuePotA = 0;      // Used to remember last value to determine turn direction
int PreviousValuePotB = 0;      // Used to remember last value to determine turn direction
int8_t DirPotA = 1;             // Direction for Pot 1 tap A
int8_t DirPotB = 1;             // Direction for Pot1 tap B

int8_t PotDirection  = 1;       // Final CALCULATED direction
int8_t PreviousPotDirection = 0;
int PotValue = 0;
int OldPotValue = 0;

uint32_t PotTimer         = 0;

void read_pot(void) {
	ValuePotA  = analogReadPin(POT_A_INPUT);
  ValuePotB  = analogReadPin(POT_B_INPUT);

  /* Decode each individual pot tap's direction */
  if (ValuePotA > (PreviousValuePotA + ADC_HYSTERESIS)) { DirPotA = 1; }
  else if (ValuePotA < (PreviousValuePotA - ADC_HYSTERESIS)) { DirPotA = -1; }
  else { DirPotA = 0; }

  if (ValuePotB > (PreviousValuePotB + ADC_HYSTERESIS)) { DirPotB = 1; }
  else if (ValuePotB < (PreviousValuePotB - ADC_HYSTERESIS)) { DirPotB = -1; }
  else { DirPotB = 0; }

  /* Determine actual direction of ENCODER based on each individual potentiometer tap´s direction and the phase */
  if (DirPotA == -1 && DirPotB == -1) {
    if (ValuePotA > ValuePotB) { PotDirection = 1; }
    else { PotDirection = -1; }
  }
  else if (DirPotA == 1 && DirPotB == 1) {
    if (ValuePotA < ValuePotB) { PotDirection = 1; }
    else { PotDirection = -1; }
  }
  else if (DirPotA == 1 && DirPotB == -1) {
    if ( (ValuePotA > (ADC_MAX_VALUE/2)) || (ValuePotB > (ADC_MAX_VALUE/2)) ) { PotDirection = 1; }
    else { PotDirection = -1; }
  }
  else if (DirPotA == -1 && DirPotB == 1) {
    if ( (ValuePotA < (ADC_MAX_VALUE/2)) || (ValuePotB < (ADC_MAX_VALUE/2)) ) { PotDirection = 1; }
    else { PotDirection = -1; }
  }
  else { PotDirection = 0; }

  /* Debounce */
  if (PotDirection != PreviousPotDirection) {
    PotTimer = timer_read32();
    PreviousPotDirection = PotDirection;
  }

  /* Calculate value based on direction, how big change in ADC value, and sensitivit y*/
  if (timer_elapsed32(PotTimer) > POT_DEBOUNCE && DirPotA != 0 && DirPotB != 0) {
    if ((ValuePotA < ADC_MAX_VALUE*0.8) && (ValuePotA > ADC_MAX_VALUE*0.2)) {
      PotValue = PotValue + PotDirection*abs(ValuePotA - PreviousValuePotA)/POT_SENSITIVITY;
    }
    else {
      PotValue = PotValue + PotDirection*abs(ValuePotB - PreviousValuePotB)/POT_SENSITIVITY;
    }
    PreviousValuePotA = ValuePotA;
    PreviousValuePotB = ValuePotB;
  }
}

void matrix_init_user(void) {
	PreviousValuePotA  = analogReadPin(POT_A_INPUT);
  PreviousValuePotB  = analogReadPin(POT_B_INPUT);
}

void matrix_scan_user(void) {
	read_pot();
	if (PotValue - OldPotValue >= POT_TOLERANCE) {
    tap_code(KC_VOLD);
    OldPotValue = 0;
    if (PotValue >= POT_TOLERANCE) { PotValue = PotValue - POT_TOLERANCE; }
	}
	else if (PotValue - OldPotValue <= -POT_TOLERANCE) {
    tap_code(KC_VOLU);
    OldPotValue = 0;
    if (PotValue <= -POT_TOLERANCE) { PotValue = PotValue + POT_TOLERANCE; }
	}
}