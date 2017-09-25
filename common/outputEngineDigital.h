/*
 * outputEngineDigital.h
 *
 *  Created on: Sep 22, 2017
 *      Author: gerard
 */

#ifndef SRC_OUTPUTENGINE_OUTPUTENGINEDIGITAL_H_
#define SRC_OUTPUTENGINE_OUTPUTENGINEDIGITAL_H_

// All timers are in periodes of 100 ms !

void outputDigoutRefresh1ms(void);

int  outputDigoutGetOverloadStatus(int id);
void outputDigoutClrOverloadStatus(int id);
void outputDigoutSetOverloadStatus(int id);
void outputDigoutSet(int id, int on);
void outputDigoutPulse(int id, int onTime);
void outputDigoutRepeat(int id, int onTime, int offTime, int repeat);

#endif /* SRC_OUTPUTENGINE_OUTPUTENGINEDIGITAL_H_ */
