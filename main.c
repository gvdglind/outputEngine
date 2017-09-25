#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "peripheralSettings.h"
#include "common/outputEngine.h"
#include "common/outputEngineDigital.h"
#include "common/outputEngineLeds.h"
#include "common/outputEngineBuzzer.h"




static char *kbhit()
{
   // 1000 ms delay & kbhit routine

   int nFdStdIn = fileno(stdin);
   struct timeval sTijdSlot;
   fd_set FdInSet;
   int iSelRdy, iLen;
   static char szStdin[1];

   FD_ZERO(&FdInSet);
   FD_SET(nFdStdIn, &FdInSet);
   sTijdSlot.tv_sec = 0l;
   sTijdSlot.tv_usec = 10000;
   iSelRdy = select(64, &FdInSet, NULL, NULL, (struct timeval *) &sTijdSlot);
   if (iSelRdy > 0) {
      if(FD_ISSET(nFdStdIn, &FdInSet)) {
        iLen = read(nFdStdIn, szStdin, 1);
        if (iLen > 0)
          return szStdin;
      }
   }
   return (char *) NULL;
}



static OUTPUTDEF_T outputDefs[] = {
	// First GPIO's, then ONELED's, then RGBLED's then BUZZER !!!!!
	// pin/pwmindex   type         mask        pwm        neg
	{ PIN_OUT1,       OUTT_GPIO,   OUTM_NONE,  PWM_OFF,   OUTD_NORMAL,   initOutput, setOutput     },
	{ PIN_OUT2,       OUTT_GPIO,   OUTM_NONE,  PWM_OFF,   OUTD_NORMAL,   initOutput, setOutput     },
	{ PIN_OUT3,       OUTT_GPIO,   OUTM_NONE,  PWM_OFF,   OUTD_NORMAL,   initOutput, setOutput     },
	{ PIN_OUT4,       OUTT_GPIO,   OUTM_NONE,  PWM_OFF,   OUTD_NORMAL,   initOutput, setOutput     },
	{ PIN_OUT5,       OUTT_GPIO,   OUTM_NONE,  PWM_OFF,   OUTD_NORMAL,   initOutput, setOutput     },
	{ PIN_OUT6,       OUTT_GPIO,   OUTM_NONE,  PWM_OFF,   OUTD_NORMAL,   initOutput, setOutput     },
	{ PIN_POWEROUT1,  OUTT_PWR,    OUTM_MON,   PWM_OFF,   OUTD_INVERSED, initOutput, setOutput     },
	{ PIN_POWEROUT2,  OUTT_PWR,    OUTM_MON,   PWM_OFF,   OUTD_INVERSED, initOutput, setOutput     },

	{ 0,              OUTT_ONELED, OUTM_PWM,   PWM_OFF,   OUTD_NORMAL,   initLed,    setLedLevel   },
	{ 1         ,     OUTT_ONELED, OUTM_PWM,   PWM_OFF,   OUTD_NORMAL,   initLed,    setLedLevel   },

	{ 2,              OUTT_RGBLED, OUTM_PWM,   PWM_OFF,   OUTD_NORMAL,   initLed,    setLedLevel   },
	{ 3,              OUTT_RGBLED, OUTM_PWM,   PWM_OFF,   OUTD_NORMAL,   initLed,    setLedLevel   },
	{ 4,              OUTT_RGBLED, OUTM_PWM,   PWM_OFF,   OUTD_NORMAL,   initLed,    setLedLevel   },

	{ 5,              OUTT_BUZZER, OUTM_PWM,   PWM_OFF,   OUTD_NORMAL,   initBuzzer, setBuzzerFreq }
};



static void board_init(void)
{
   #define NELT(x) (sizeof(x) / sizeof(x[0]))

	// For example init output [3] to state 'on'
	outputDefs[3].pwmSetting = PWM_MAX_DUTY;
   // Init the output engine
	outputInit(outputDefs, NELT(outputDefs));
}



int main(void) 
{
   char *pc;
   int quit = 0;

   printf("Starting output engine demo %d.%02d : q)uit or d), r), b)\n", outputGetLibVersion() / 100, outputGetLibVersion() % 100);
	board_init();

   while (!quit) {
      // Call each 1 ms
		outputDigoutRefresh1ms();
		outputLedControlRefresh1ms();
      outputBuzzerRefresh1ms();

		pc = kbhit();
      if (pc) {
      	switch (*pc) {
				case 'q' : quit++; break;
				case 'd' : outputDigoutPulse(2, 30); break;
				case 'r' : outputLedRGBRepeat(0, 100, 100, 0, 2, 2, 2, 0); break;
				case 'b' : outputBuzzerRepeat(500, 500, 3, 1000, 2000); break;
			}
		} 
   }

	return 0;
}



