/*
 * outputEngineBuzzer.c
 *
 *  Created on: Sep 22, 2017
 *      Author: gerard
 */


#include "outputEngine.h"
#include "outputEngineBuzzer.h"



typedef enum { BUZZERSTATE_IDLE, BUZZERSTATE_ON1, BUZZERSTATE_ON2, BUZZERSTATE_TEST, BUZZERSTATE_HOLD } BUZZERSTATE;

typedef struct {
	int  priority;
	int  repeat;
	int timeHigh;
	int timeLow;
	int frequency1;
	int frequency2;
} TToneSettings;

typedef struct
{
	int newCommand;
	int state;
	int repeat;
	int buzzerTime;
	int lastFrequency;
	TToneSettings command;
} TBuzzer;


static TBuzzer  buzzer = {0, 0, 0, 0, 0, {0xFF, 0, 0, 0, 0, 0}};



static void BuzzerSetTone(int freq)
{
	int buzzerIndex = outputGetOffset(OUTT_BUZZER);
	OUTPUTDEF_T *pOutput = outputGetOutput(buzzerIndex);

	if (pOutput)
		(*pOutput->setLowLevelOutput)(pOutput->id, freq);
}



//--- outputBuzzerRefresh1ms ---

void outputBuzzerRefresh1ms(void)
{
	static int oldstate = 0xff;

	switch (buzzer.state) {
		default:
		case BUZZERSTATE_IDLE:
			if (buzzer.command.priority < 0xFF) {
				buzzer.repeat = buzzer.command.repeat;
				buzzer.newCommand = 0;
				buzzer.state = BUZZERSTATE_HOLD;
			}
			break;

		case BUZZERSTATE_ON1:
			if (buzzer.buzzerTime == 0xffff) // GvdG add for set continue
				break;
			if (buzzer.buzzerTime > 0)
				buzzer.buzzerTime--;
			else {
				if (buzzer.command.frequency2 > 0)
					BuzzerSetTone(buzzer.command.frequency2);
				buzzer.buzzerTime = buzzer.command.timeHigh;
				buzzer.state = BUZZERSTATE_ON2;
			}
			break;

		case BUZZERSTATE_ON2:
			if (buzzer.buzzerTime > 0)
				buzzer.buzzerTime--;
			else {
				buzzer.state = BUZZERSTATE_HOLD;

				if (buzzer.command.timeLow > 0) {
					buzzer.buzzerTime = buzzer.command.timeLow;
					BuzzerSetTone(0);
				}
			}
			break;

		case BUZZERSTATE_HOLD:
			if (buzzer.buzzerTime > 0)
				buzzer.buzzerTime--;
			else if (buzzer.newCommand == 1) {
				BuzzerSetTone(0);
				buzzer.state = BUZZERSTATE_IDLE;
			}
			else if (buzzer.command.timeHigh == 0) {
				buzzer.command.priority = 0xFF;
				BuzzerSetTone(0);
				buzzer.state = BUZZERSTATE_IDLE;
			}
			else if (buzzer.repeat > 0) {
				buzzer.repeat--;
				buzzer.buzzerTime = buzzer.command.timeHigh;
				BuzzerSetTone(buzzer.command.frequency1);
				buzzer.state = BUZZERSTATE_ON1;
			}
			else if (buzzer.command.repeat == 0) {
				buzzer.buzzerTime = buzzer.command.timeHigh;
				BuzzerSetTone(buzzer.command.frequency1);
				buzzer.state = BUZZERSTATE_ON1;
			}
			else {
				buzzer.command.priority = 0xFF;
				BuzzerSetTone(0);
				buzzer.state = BUZZERSTATE_IDLE;
			}
			break;
	}

	if (buzzer.state != oldstate) {
		oldstate = buzzer.state;
		#ifdef nMYDIAGS
			print422("BUZZERSTATE NOW %d\n", buzzer.state);
		#endif
	}
}



//--- outputBuzzerSet ---

void outputBuzzerSet(int frequency)
{
	if (frequency == 0) {
		outputBuzzerRepeat(0, 0, 0, 0, 0);
		buzzer.buzzerTime = 0;
	}
	else
		outputBuzzerRepeat(0xffff, 0, 0, frequency, 0);
}



//--- outputBuzzerPulse ---

void outputBuzzerPulse(int onTime, int frequency)
{
	outputBuzzerRepeat(onTime, 0, 1, frequency, 0);
}



//--- outputBuzzerRepeat ---

void outputBuzzerRepeat(int onTime, int offTime, int repeat, int frequency1, int frequency2)
{
	int priority = 0; // for what to use this ?

	if ((buzzer.command.priority >= priority) &&
		((buzzer.command.repeat == 0) || (buzzer.state == BUZZERSTATE_IDLE))) {
		// Set buzzer command, time
		if (onTime == 0xffff)
			buzzer.command.timeHigh = 0xffff;
		else if (onTime > 0)
			buzzer.command.timeHigh = (onTime >> 1) + 1;
		else
			buzzer.command.timeHigh = 0;

		if (offTime > 0)
			buzzer.command.timeLow = offTime + 1;
		else
			buzzer.command.timeLow = 0;

		// Set buzzer command, settings
		buzzer.command.frequency1 = frequency1;
		buzzer.command.frequency2 = frequency2;
		buzzer.command.repeat = repeat;
		buzzer.command.priority = priority;
		buzzer.newCommand = 1;
	}
}



//--- outputBuzzerBusy ---

int outputBuzzerBusy(void)
{
	return (buzzer.state != BUZZERSTATE_IDLE) || buzzer.newCommand;
}





