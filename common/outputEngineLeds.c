/*
 * outputEngineLeds.c
 *
 *  Created on: Sep 22, 2017
 *      Author: gerard
 */

#include "outputEngine.h"
#include "outputEngineLeds.h"


#define NO_FADE                                 0
#define FADE                                    1

#define MIN_PWM_STEP							5
#define MIN_TIME_STEP							1
#define PWM_CONST 								50
#define CALC_STEP_PWM(time,pwmdiff)				(((pwmdiff/(time*PWM_CONST)) > (MIN_PWM_STEP)) ? (pwmdiff/(time*PWM_CONST)) : (MIN_PWM_STEP))
#define CALC_STEP_TIME(time,pwmdiff,pwmstep) 	(((time*PWM_CONST*MIN_TIME_STEP) > (pwmdiff/pwmstep)) ? ((time*PWM_CONST*MIN_TIME_STEP)/(pwmdiff/pwmstep)) : (MIN_TIME_STEP))


typedef enum { RGB_INDEX_RED = 0, RGB_INDEX_GREEN = 1, RGB_INDEX_BLUE = 2, LEDOUTPUT_FADE = 0x8000 } LEDOUTPUT;

typedef enum { PWMSTATE_IDLE, PWMSTATE_READY, PWMSTATE_INCREASE, PWMSTATE_DECREASE } PWMSTATE;

typedef enum { PWMMODE_OFF, PWMMODE_ON, PWMMODE_FLASH, PWMMODE_FADE } PWMMODE;

typedef struct {
	int maxChannel; // 3 for RGB, 1 for 'normal'
	int outputIndex;

	int pwmHoldMode;
	int pwmHoldLow[3];
	int pwmHoldHigh[3];

	int pwmLow[3];
	int pwmHigh[3];
	int repeatCounter;

	int refresh;
	int currentPwm;

	int stepTimeH;
	int stepPwmH;
	int stepTimeL;
	int stepPwmL;

	int pwmMode;
	int oldPwmMode;
	int pwmState;
} TLedControl;


static TLedControl ledControl[MAX_PWMLEDS_COMBINED];



static void LedControlBackupPwm(int id)
{
	int channel;

	// Backup steady pwm values

	ledControl[id].pwmHoldMode = ledControl[id].pwmMode;
	for (channel = 0; channel < ledControl[id].maxChannel; channel++) {
		ledControl[id].pwmHoldLow[channel] = ledControl[id].pwmLow[channel];
		ledControl[id].pwmHoldHigh[channel] = ledControl[id].pwmHigh[channel];
	}
}



static void LedControlRestorePwm(int id)
{
	int channel;

	// Restore steady pwm values

	ledControl[id].pwmMode = ledControl[id].pwmHoldMode;
	for (channel = 0; channel < ledControl[id].maxChannel; channel++) {
		ledControl[id].pwmLow[channel]  = ledControl[id].pwmHoldLow[channel];
		ledControl[id].pwmHigh[channel] = ledControl[id].pwmHoldHigh[channel];
	}
}



static void LedControlCalculateLevels(int id)
{
	int channel, level;

	// Update a Led

	for (channel = 0; channel < ledControl[id].maxChannel; channel++) {
		if (ledControl[id].pwmLow[channel] > ledControl[id].pwmHigh[channel]) // Calculate pwm value
			level = ((((int)ledControl[id].pwmLow[channel] - (int)ledControl[id].pwmHigh[channel]) * (0xF0 - ledControl[id].currentPwm)) / 0xF0) + (int)ledControl[id].pwmHigh[channel];
		else if (ledControl[id].pwmHigh[channel] > 0)                         // Calculate pwm value
			level = ((((int)ledControl[id].pwmHigh[channel] - (int)ledControl[id].pwmLow[channel]) * ledControl[id].currentPwm) / 0xF0) + (int)ledControl[id].pwmLow[channel];
		else
			level = 0;

		// Drive led...

		OUTPUTDEF_T *pOutput = outputGetOutput(ledControl[id].outputIndex + channel);
		if (pOutput)
			(*pOutput->setLowLevelOutput)(pOutput->id, level);
	}
}



static void outputLedControlSetPattern(int id, int fade, int onTime, int offTime, int repeat)
{
	// Set custom Led state

	// Set repeat counter
	ledControl[id].repeatCounter = repeat;

	// Set pwm mode
	if ((onTime == 0) && (offTime == 0))     // set off
		ledControl[id].pwmMode = PWMMODE_OFF;
	else if ((onTime > 0) && (offTime == 0)) // set on
		ledControl[id].pwmMode = PWMMODE_ON;
	else if (fade > 0) {
		ledControl[id].pwmMode = PWMMODE_FADE;
		ledControl[id].stepPwmL = CALC_STEP_PWM(offTime, 0xF0);
		ledControl[id].stepTimeL = CALC_STEP_TIME(offTime, 0xF0, ledControl[id].stepPwmL) * 2;
		ledControl[id].stepPwmH = CALC_STEP_PWM(onTime, 0xF0);
		ledControl[id].stepTimeH = CALC_STEP_TIME(onTime, 0xF0, ledControl[id].stepPwmH) * 2;
	}
	else {
		ledControl[id].pwmMode = PWMMODE_FLASH;
		ledControl[id].stepTimeL = onTime * 100;
		ledControl[id].stepTimeH = offTime * 100;
		ledControl[id].stepPwmL = 0xF0;
		ledControl[id].stepPwmH = 0xF0;
	}

	// Reset pwm
	ledControl[id].oldPwmMode = 0xFF;
}



//--- LedControlRefreshLed ---

static void LedControlRefreshLed(int id)
{
	// Detect mode changes
	if (ledControl[id].pwmMode != ledControl[id].oldPwmMode) {
		ledControl[id].oldPwmMode = ledControl[id].pwmMode;
		ledControl[id].pwmState = PWMSTATE_IDLE;
	}

	// Handle pwm state
	switch (ledControl[id].pwmState) {
		default:
		case PWMSTATE_IDLE:
			switch (ledControl[id].pwmMode) {
				case PWMMODE_OFF:
					ledControl[id].currentPwm = 0;
					LedControlBackupPwm(id);
					ledControl[id].pwmState = PWMSTATE_READY;
					break;

				case PWMMODE_ON:
					ledControl[id].currentPwm = 0xF0;
					LedControlBackupPwm(id);
					ledControl[id].pwmState = PWMSTATE_READY;
					break;

				case PWMMODE_FLASH:
				case PWMMODE_FADE:
					ledControl[id].currentPwm = 0;
					ledControl[id].pwmState = PWMSTATE_INCREASE;
					break;
			}
			return;

			case PWMSTATE_READY:
				// Don't change pwm value
				break;

			case PWMSTATE_INCREASE:
				if ((ledControl[id].currentPwm += ledControl[id].stepPwmL) >= 0xF0) {
					ledControl[id].currentPwm = 0xF0;
					ledControl[id].pwmState = PWMSTATE_DECREASE;
				}
				ledControl[id].refresh = ledControl[id].stepTimeL;
				break;

			case PWMSTATE_DECREASE:
				// Set pwm low and check
				ledControl[id].refresh = ledControl[id].stepTimeH;

				if ((ledControl[id].currentPwm -= ledControl[id].stepPwmH) <= 0) {
					ledControl[id].currentPwm = 0;
					ledControl[id].pwmState = PWMSTATE_INCREASE;

					// Check number of repetitions
					if (ledControl[id].repeatCounter > 0) {
						ledControl[id].repeatCounter--;
						if (ledControl[id].repeatCounter == 0) {
							LedControlCalculateLevels(id);
							LedControlRestorePwm(id);
							ledControl[id].pwmState = PWMSTATE_READY;
							return;
						}
					}
				}
				break;
	}

	LedControlCalculateLevels(id);
}



//--- outputLedControlRefresh1ms ---

void outputLedControlRefresh1ms(void)
{
	int id;

	for (id = 0; id < outputGetNrOfPwmLedsCombined(); id++) {
		if (ledControl[id].refresh > 0)
			ledControl[id].refresh--;
		if (ledControl[id].refresh > 0)
			continue;

		LedControlRefreshLed(id);
	}
}



//--- outputLedOneSet ---

void outputLedOneSet(int id, int pwm)
{
	outputLedOneRepeat(id, pwm, 1, 0, 0, NO_FADE);
}



//--- outputLedOnePulse ---

void outputLedOnePulse(int id, int pwm, int onTime, int fade)
{
	outputLedOneRepeat(id, pwm, onTime, 1, 1, fade);
}



//--- outputLedOneRepeat ---

void outputLedOneRepeat(int id, int pwm, int onTime, int offTime, int repeat, int fade)
{
	if (pwm > PWM_MAX_DUTY)
		pwm = PWM_MAX_DUTY;

	#ifdef MYDIAGS
		print422("outputLedOneRepeat[%d]: t-on=%d   t-off=%d   reps=%d   fade=%d\n", id, onTime, offTime, repeat, fade);
	#endif

	if (id >= outputGetNrOfPwmOneLeds())
		return;

	fade = 0; // zzz fade werkt (nog) niet op TC1

	// Set led command..., set Pwm values (0..100% --> 0..240)

	ledControl[id].maxChannel  = 1;
	ledControl[id].outputIndex = outputGetOffset(OUTT_ONELED) + id;

	ledControl[id].pwmLow[0]   = 0;
	ledControl[id].pwmLow[1]   = 0;
	ledControl[id].pwmLow[2]   = 0;

	ledControl[id].pwmHigh[0]  = ((pwm * 24) + 5) / 10;
	ledControl[id].pwmHigh[1]  = 0;
	ledControl[id].pwmHigh[2]  = 0;

	// Set led pattern
	outputLedControlSetPattern(id, fade, onTime, offTime, repeat);
}



//--- outputLedRGBSet ---

void outputLedRGBSet(int id, int red, int green, int blue)
{
	outputLedRGBRepeat(id, red, green, blue, 1, 0, 0, NO_FADE);
}



//--- outputLedRGBPulse ---

void outputLedRGBPulse(int id, int red, int green, int blue, int onTime, int fade)
{
	outputLedRGBRepeat(id, red, green, blue, onTime, 1, 1, fade);
}



//--- outputLedRGBRepeat ---

void outputLedRGBRepeat(int id, int red, int green, int blue, int onTime, int offTime, int repeat, int fade)
{
	if (red > PWM_MAX_DUTY)
		red = PWM_MAX_DUTY;
	if (green > PWM_MAX_DUTY)
		green = PWM_MAX_DUTY;
	if (blue > PWM_MAX_DUTY)
		blue = PWM_MAX_DUTY;

	#ifdef MYDIAGS
		print422("outputLedSetRGB[%d]: red=%d   green=%d   blue=%d   t-on=%d   t-off=%d   reps=%d   fade=%d\n", id, red, green, blue, onTime, offTime, repeat, fade);
	#endif

	// Set full RGB led command..., set Pwm values (0..100% --> 0..240)

	int idx = (outputGetNrOfPwmOneLeds() + id);
	if (idx >= outputGetNrOfPwmLedsCombined())
		return;

	ledControl[idx].maxChannel  = 3;
	ledControl[idx].outputIndex = outputGetOffset(OUTT_RGBLED) + (id * 3);

	ledControl[idx].pwmLow[0]   = 0;
	ledControl[idx].pwmLow[1]   = 0;
	ledControl[idx].pwmLow[2]   = 0;

	ledControl[idx].pwmHigh[0]  = ((red * 24) + 5) / 10;
	ledControl[idx].pwmHigh[1]  = ((green * 24) + 5) / 10;
	ledControl[idx].pwmHigh[2]  = ((blue * 24) + 5) / 10;

	// Set rgb led pattern
	outputLedControlSetPattern(idx, fade, onTime, offTime, repeat);
}






