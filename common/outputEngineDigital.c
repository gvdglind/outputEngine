/*
 * outputEngineDigital.c
 *
 *  Created on: Sep 22, 2017
 *      Author: gerard
 */

#include "outputEngine.h"


typedef struct _outputPattern
{
	int overload; // output overload flag (saved)
    //
	int currentValue;
	int repeatCounter;
    int repeatState;
	int remainingTime;
	int timeLow;
	int timeHigh;
} OUTPUTPATTERN_T;

static OUTPUTPATTERN_T digitalOutputChannels[MAX_OUTPUTS];



static void DigoutSet(int index)
{
	OUTPUTDEF_T *pOutput = outputGetOutput(index);;

	#ifdef MYDIAGS
		print422("DigoutSet[%d]\n", index);
	#endif

	if (!pOutput)
		return;

	if (!pOutput->setLowLevelOutput)
		return;

	if (pOutput->negate)
		(*pOutput->setLowLevelOutput)(pOutput->id, 0);
	else
		(*pOutput->setLowLevelOutput)(pOutput->id, 1);
}



static void DigoutClr(int index)
{
	OUTPUTDEF_T *pOutput = outputGetOutput(index);

	#ifdef MYDIAGS
		print422("DigoutClr[%d]\n", index);
	#endif

	if (!pOutput)
		return;

	if (!pOutput->setLowLevelOutput)
		return;

	if ((index >= outputGetNrOfDigitalOutputs()) || (!pOutput->setLowLevelOutput))
		return;

	if (pOutput->negate)
		(*pOutput->setLowLevelOutput)(pOutput->id, 1);
	else
		(*pOutput->setLowLevelOutput)(pOutput->id, 0);
}



void DigoutSetChannel(int channelId, int timeHigh, int timeLow, int repeat)
{
	OUTPUTPATTERN_T *channel;

	if (channelId < outputGetNrOfDigitalOutputs()) {
		#ifdef MYDIAGS
			print422("DigoutSetChannel: Digout[%d] (%d:%d) * %d\n", channelId, timeHigh, timeLow, repeat);
		#endif

		channel = &digitalOutputChannels[channelId];
		channel->currentValue = channel->repeatCounter = channel->remainingTime = 0;
		channel->repeatState = -1;

		if ((timeHigh == 0) && (timeLow >= 0)) {  	       // set off
			channel->currentValue = 0;
		}
		else if ((timeHigh > 0) && (timeLow == 0)) {       // set on
			channel->currentValue = 1;
		}
		else {	                                           // set pattern
			channel->repeatState = 0;
			channel->repeatCounter = repeat;
			channel->timeLow  = timeHigh * 100;
			channel->timeHigh = timeLow * 100;
		}
	}
}



//--- outputDigoutRefresh1ms ---

void outputDigoutRefresh1ms(void)
{
	OUTPUTPATTERN_T *channel;
	int x;

	for (x = 0; x < outputGetNrOfDigitalOutputs(); x++) {
		channel = &digitalOutputChannels[x];

		if (channel->repeatState >= 0) {	                                   // this is a repetitive command
			if (--channel->remainingTime <= 0) {

				if (channel->repeatState == 1) {	                           // state == 1 & countdown -> level high->low
					channel->repeatState = 0;

					if (channel->repeatCounter > 0) {
						if (--channel->repeatCounter == 0) {
							channel->repeatState = -1;                        // disable channel
							DigoutClr(x);
						}
					}
					channel->remainingTime = channel->timeHigh;
				}
				else {                                                         // state == 0 & countdown -> level low->high
					channel->repeatState = 1;
					channel->remainingTime = channel->timeLow;
				}

				channel->currentValue = channel->repeatState;
			}
		}

		if (channel->currentValue >= 0) {
			if (channel->currentValue == 0)
				DigoutClr(x);
			else
				DigoutSet(x);
			channel->currentValue = -1;
		}
	}
}



//--- outputDigoutGetOverloadStatus ---

int outputDigoutGetOverloadStatus(int id)
{
	if (id < outputGetNrOfDigitalOutputs())
		return digitalOutputChannels[id].overload;
	return -1;
}



//--- outputDigoutSetOverloadStatus ---

void outputDigoutSetOverloadStatus(int id)
{
	if (id < outputGetNrOfDigitalOutputs())
		digitalOutputChannels[id].overload = 1;
}



//--- outputDigoutClrOverloadStatus ---

void outputDigoutClrOverloadStatus(int id)
{
	if (id < outputGetNrOfDigitalOutputs())
		digitalOutputChannels[id].overload = 0;
}



//--- outputDigoutSet ---

void outputDigoutSet(int id, int on)
{
	DigoutSetChannel(id, (on ? 1 : 0), (on ? 0 : 1), 0);
}



//--- outputDigoutPulse ---

void outputDigoutPulse(int id, int onTime)
{
   DigoutSetChannel(id, onTime, 1, 1);
}



//--- outputDigoutRepeat ---

void outputDigoutRepeat(int id, int onTime, int offTime, int repeat)
{
	DigoutSetChannel(id, onTime, offTime, repeat);
}
