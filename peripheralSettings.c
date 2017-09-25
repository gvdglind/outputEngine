/*
 *  peripheralSettings.c
 *
 */

#include <stdio.h>
#include <stdlib.h>



void initLed(int index)
{
	printf("initLed[%d]\n", index);
}



void setLedLevel(int index, int level)
{
   // level will be between 0 an full scale (240)
	static int old[] = {-1, -1, -1, -1, -1};
	if (old[index] != level) {
		old[index] = level;
		printf("setLedLevel[%d]: level=%d\n", index, level);
	}
}



void initBuzzer(int index)
{
	printf("initBuzzer[%d]\n", index);
}



void setBuzzerFreq(int index, int freq)
{
   printf("setBuzzerFreq: %d Hz\n", freq);
}



void initOutput(int pin)
{
	printf("initOutput[%d]\n", pin);
}



void setOutput(int pin, int set)
{
	printf("setLedLevel[%d]: set=%d\n", pin, set);
}





