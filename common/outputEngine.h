/*
 * output_engine.h
 *
 *  Created on: Nov 1, 2012
 *      Author: sebastiaan
 */

#ifndef OUTPUT_ENGINE_H_
#define OUTPUT_ENGINE_H_

//#define MYDIAGS // zzz
#ifdef MYDIAGS
	#include "orion/orion_board.h"
	// #define TESTPWM
#endif

#define OUTPUT_ENGINE_LIB_VERSION   101

#define MAX_OUTPUTS                 24
#define MAX_PWMLEDS_COMBINED        8 // Max total of 'normal' leds and 'rg' or 'rgb' leds : one such can take max 3 output channels

#define OUTD_NORMAL                 0
#define OUTD_INVERSED               1

#define OUTM_NONE                   0x00
#define OUTM_RFU                    0x10
#define OUTM_SHARED_INP				0x20
#define OUTM_MON					0x40
#define OUTM_PWM					0x80
#define OUTM_MASK                   0xf0

#define OUTT_GPIO					0x0
#define OUTT_PWR					0x1
#define OUTT_ONELED					0x8
#define OUTT_RGBLED					0x9
#define OUTT_BUZZER					0xf
#define OUTT_MASK                   0x0f

#define PWM_OFF						0
#define PWM_MAX_DUTY                100
#define OP_OFF                      0
#define OP_ON                       1


typedef struct _outputDef
{
	int  id;			    //	GPIO pin number
	int  type;              //  i.e. a (rgb)led, buzzer etc)
	int  mask;              //  i.e. it's a 'normal' output or PWM
	int  pwmSetting;		//	initial PWM duty setting, normally PWM_OFF
	int  negate;			//	negative hardware driver (2*transistor)
	void (*initLowLevelOutput)(int id);
	void (*setLowLevelOutput)(int id, int value);
} OUTPUTDEF_T;


void outputInit(OUTPUTDEF_T [], int nrOf);
void outputProcess(void);   // call once per ms
int  outputGetId(int outputId);
int  outputGetType(int outputId);
int  outputGetOffset(int type);
OUTPUTDEF_T *outputGetOutput(int outputId);
int  outputGetNrOfOutputs(void);
int  outputGetNrOfDigitalOutputs(void);
int  outputGetNrOfPwmLedsCombined(void);
int  outputGetNrOfPwmOneLeds(void);
int  outputGetLibVersion(void);

#endif /* OUTPUT_ENGINE_H_ */
