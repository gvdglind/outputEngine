/*
 * outputEngineBuzzer.h
 *
 *  Created on: Sep 22, 2017
 *      Author: gerard
 */

#ifndef SRC_OUTPUTENGINE_OUTPUTENGINEBUZZER_H_
#define SRC_OUTPUTENGINE_OUTPUTENGINEBUZZER_H_

void outputBuzzerRefresh1ms(void);
void outputBuzzerSet(int frequency); // frequency 0 is off
void outputBuzzerPulse(int onTime, int frequency);
void outputBuzzerRepeat(int onTime, int offTime, int repeat, int frequency1, int frequency2); // single tone -> set frequency2 to 0
int  outputBuzzerBusy(void);

#endif /* SRC_OUTPUTENGINE_OUTPUTENGINEBUZZER_H_ */
