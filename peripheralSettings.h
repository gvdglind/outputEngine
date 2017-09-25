/*
 *  peripheralSettings.h
 *
 *  Created on: Sep 11, 2017
 *      Author: gerard
 */

#ifndef _PERIPHERALSETTINGS_H_
#define _PERIPHERALSETTINGS_H_


#define PIN_OUT1        17 
#define PIN_OUT2        18
#define PIN_OUT3        19
#define PIN_OUT4        20
#define PIN_OUT5        32
#define PIN_OUT6        33
#define PIN_POWEROUT1   40
#define PIN_POWEROUT2   41


void initLed(int index);
void setLedLevel(int index, int level);
void initBuzzer(int index);
void setBuzzerFreq(int index, int freq);
void initOutput(int pin);
void setOutput(int pin, int set);

#endif /* _PERIPHERALSETTINGS_H_ */
