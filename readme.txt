
Aanzet tot hergebruik code.
---------------------------

Het valt mij op dat op we allemaal hier onze eigen routinetje maken 
om een ledje te laten knipperen een inputje te ontdenderen etc.
Ik heb geprobeerd een universele output engine library te maken, zie
subdir. 'common' met hier in de root het meest eenvoudige testprogramma 
als demo.

Bij initialisatie krijgt elke output (digitaal, led, rgb led, buzzer..)
een eigen 'init' en 'set' routine mee, B.v. : 

static OUTPUTDEF_T outputDefs[] = {
   ...
   { PIN_POWEROUT2, OUTT_PWR, OUTM_MON, PWM_OFF, OUTD_INVERSED, initOutput, setOutput},

De hoofdfile v/d output engine is outputEngine.c en outputInit() is de eerste
functie die vanuit de applicatie moet worden aangeroepen :

outputInit(outputDefs, NELT(outputDefs)); 

Er is met deze opzet flexibiliteit in het aantal output's en het soort.

Naast de hoofdfile outputEngine zijn en (nu) 3 neven files :
1) outputEngineDigital.c    {aan-uit}
2) outputEngineLeds.c       {enkelvoudige en rg(b)leds met fading mogelijkheid via PWM)
3) outputEngineBuzzer.c     {dubbel tone buzzer}

Met hun eigen min of meer specifieke functie sets.

Al deze neven functies hebben een 'poll' functie, b.v. outputDigoutRefresh1ms() die
om de 1 ms moeten worden aangeroepen vanuit de applicatie.
Dit lijkt natuurlijk (geen toeval) op de unix driver structuur.

Dus een applicatie fragment kan zijn:
  
   if (delta1ms()) {
      outputDigoutRefresh1ms()
      outputLedControlRefresh1ms();
      outputBuzzerRefresh1ms();
   }
   if (access) {
      // output with index 2 active for 3 seconds
      outputDigoutPulse(2, 30); 
      // flash the first rgb led 'orange', 3 times, with periods of 200 ms
      outputLedRGBRepeat(0, 100, 100, 0, 2, 2, 3, 0); 
   }

De files in de output Engine zijn bewust platform onafhankelijk gemaakt !!
Dus geen #ifdef dit en dat, geen processor specifieke includes als "avr.h" etc.
Dus de lib is uit elke C compiler te genereren, in deze test is dat gewoon een
PC based gcc.

Het processor specifieke deel, de 'init' en 'set' fucties staan buiten de library
files, dus in de applicatie, ik heb Ron zijn naamgeving overgenomen :
peripheralSettings.c
Als b.v. hier 'initOutput' om een digitale (aan-uit) output te initialiseren of
b.v 'setLedLevel' om een led op een bepaald helderheid te laten branden.
In de Orion applicatie zal 'initOutput' nodig zijn om b.v. de data directie van
een processor pen te te zetten, maar hier als PC demo doe ik alleen een printf.  


