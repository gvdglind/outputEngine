/*
 * output_engine.c
 */

#include "outputEngine.h"
#include "outputEngineDigital.h"
#include "outputEngineLeds.h"
#include "outputEngineBuzzer.h"


extern void DigoutSetChannel(int channelId, int timeHigh, int timeLow, int repeat);


static OUTPUTDEF_T outputs[MAX_OUTPUTS];
static int nrOfAllOutputs, nrOfPwmLedsCombined, nrOfPwmOneLeds, nrOfPwmRgbLeds, nrOfDigitalOutputs;
static int base_digout = -1, base_pwrout = -1, base_oneled = -1, base_rgbled = -1, base_buzzer = -1;



static void xmemcpy(char *dest, const char *src, int n)
{   // Whats wrong with the standard memcpy ? zzz
	if (n > 0) {
		do {
			*dest++ = *src++;
		}
		while (--n > 0);
	}
}




//=====================================================================================================================
//===                                            G E N E R A L                                                      ===
//=====================================================================================================================



//--- outputInit ---

void outputInit(OUTPUTDEF_T outputDefs[], int nrOf)
{
	OUTPUTDEF_T *pOutput;
	int x;

	nrOfAllOutputs = nrOf;

	for (x = 0; x < nrOfAllOutputs; x++) {
		pOutput = &outputs[x];
        xmemcpy((char *)pOutput, (char *)&outputDefs[x], sizeof(OUTPUTDEF_T));

        // Find the offsets
		switch (pOutput->type) {
			case OUTT_GPIO   : if (base_digout == -1) base_digout = x; break;
			case OUTT_PWR	  : if (base_pwrout == -1) base_pwrout = x; break;
			case OUTT_ONELED : if (base_oneled == -1) base_oneled = x; break;
			case OUTT_RGBLED : if (base_rgbled == -1) base_rgbled = x; break;
			case OUTT_BUZZER : if (base_buzzer == -1) base_buzzer = x; break;
		}

		// Execute the init. function
		if (pOutput->initLowLevelOutput)
			(*pOutput->initLowLevelOutput)(pOutput->id);

		if (pOutput->type == OUTT_BUZZER)
			continue;

		// Count how much of each type of output
		if (outputs[x].mask & OUTM_PWM) {        // PWM (leds)
			if (pOutput->type == OUTT_ONELED)
				nrOfPwmOneLeds++;
			if (pOutput->type == OUTT_RGBLED)
				nrOfPwmRgbLeds++;
		}
		else {                                   // Digital outputs
			nrOfDigitalOutputs++;                // This first because DigoutSetChannel() checks on nrOfDigitalOutputs !
			DigoutSetChannel(x, (pOutput->pwmSetting > 0) ? 1 : 0, 0, 0);
		}
	}

	nrOfPwmLedsCombined = nrOfPwmOneLeds + (nrOfPwmRgbLeds / 3);

	#ifdef MYDIAGS
		print422("outputInit: max=%d dig=%d oneled=%d rgbled=%d ledcombined=%d\n", nrOfAllOutputs, nrOfDigitalOutputs, nrOfPwmOneLeds, nrOfPwmRgbLeds, nrOfPwmLedsCombined);
	#endif
}



//--- outputProcess ---

void outputProcess(void)
{
	// Make sure to call this function every 1 ms.

	outputDigoutRefresh1ms();
	outputLedControlRefresh1ms();
	outputBuzzerRefresh1ms();
}



//--- outputGetId ---

int outputGetId(int outputId)
{
	return outputs[outputId].id;
}



//--- outputGetType ---

int outputGetType(int outputId)
{
	if (outputId >= nrOfAllOutputs)
		return -1;
	return outputs[outputId].mask | outputs[outputId].type;
}



//--- outputGetOffset ---

int outputGetOffset(int type)
{
	type &= OUTT_MASK;
	switch (type) {
		case OUTT_GPIO   : if (base_digout != -1) return base_digout; break;
		case OUTT_PWR    : if (base_pwrout != -1) return base_pwrout; break;
		case OUTT_ONELED : if (base_oneled != -1) return base_oneled; break;
		case OUTT_RGBLED : if (base_rgbled != -1) return base_rgbled; break;
		case OUTT_BUZZER : if (base_buzzer != -1) return base_buzzer; break;
	}
	return 0;
}



//--- outputGetOutput ---

OUTPUTDEF_T *outputGetOutput(int outputId)
{
	if ((outputId < 0) || (outputId >= nrOfAllOutputs))
		return 0;
	return &outputs[outputId];
}



//--- outputGetNrOfOutputs ---

int outputGetNrOfOutputs(void)
{
	return nrOfAllOutputs;
}



//--- outputGetNrOfDigitalOutputs ---

int outputGetNrOfDigitalOutputs(void)
{
	return nrOfDigitalOutputs;
}



//--- outputGetNrOfPwmLedsCombined ---

int outputGetNrOfPwmLedsCombined(void)
{
	return nrOfPwmLedsCombined;
}



//--- outputGetNrOfPwmOneLeds ---

int outputGetNrOfPwmOneLeds(void)
{
	return nrOfPwmOneLeds;
}



//--- outputGetLibVersion ----

int outputGetLibVersion(void)
{
   return OUTPUT_ENGINE_LIB_VERSION;
}


