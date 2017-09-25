/*
 * outputEngineLeds.h
 *
 *  Created on: Sep 22, 2017
 *      Author: gerard
 */

#ifndef SRC_OUTPUTENGINE_OUTPUTENGINELEDS_H_
#define SRC_OUTPUTENGINE_OUTPUTENGINELEDS_H_

// All timers are in periodes of 100 ms !

void outputLedControlRefresh1ms(void);

void outputLedOneSet(int id, int pwm);
void outputLedOnePulse(int id, int pwm, int onTime, int fade);
void outputLedOneRepeat(int id, int pwm, int onTime, int offTime, int repeat, int fade);

void outputLedRGBSet(int id, int red, int green, int blue);
void outputLedRGBPulse(int id, int red, int green, int blue, int onTime, int fade);
void outputLedRGBRepeat(int id, int red, int green, int blue, int onTime, int offTime, int repeat, int fade);

#endif /* SRC_OUTPUTENGINE_OUTPUTENGINELEDS_H_ */
