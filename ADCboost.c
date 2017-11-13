#include "wiring_private.h"
#include "pins_arduino.h"

uint8_t ADC_reference = 01;

uint8_t low, high;

void ADC_set_reference(uint8_t ref){
	
	ADC_reference = ref;
	
}


void ADC_init(uint8_t pin){
	
	// defines for setting and clearing register bits
	#ifndef cbi
	#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
	#endif
	#ifndef sbi
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
	#endif
	
	  // set prescale to 4
    cbi(ADCSRA,ADPS2) ; //0
    sbi(ADCSRA,ADPS1) ; //1
    cbi(ADCSRA,ADPS0) ; //0

    
/*  These bits determine the division factor between the system clock
    frequency and the input clock to the ADC.
    ADPS2 ADPS1 ADPS0 Division Factor
    0     0     0     2
    0     0     1     2
    0     1     0     4
    0     1     1     8
    1     0     0     16
    1     0     1     32
    1     1     0     64
    1     1     1     128
	
*/ //tempo impiegato per la lettura, con prescaler 128  è pari a 112us, con prescaler=4 è pari a 8us  

	#if defined(analogPinToChannel)
	#if defined(__AVR_ATmega32U4__)
		if (pin >= 18) pin -= 18; // allow for channel or pin numbers
	#endif
		pin = analogPinToChannel(pin);
	#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		if (pin >= 54) pin -= 54; // allow for channel or pin numbers
	#elif defined(__AVR_ATmega32U4__)
		if (pin >= 18) pin -= 18; // allow for channel or pin numbers
	#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
		if (pin >= 24) pin -= 24; // allow for channel or pin numbers
	#else
		if (pin >= 14) pin -= 14; // allow for channel or pin numbers
	#endif

	#if defined(ADCSRB) && defined(MUX5)
		// the MUX5 bit of ADCSRB selects whether we're reading from channels
		// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
		ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
	#endif
	  
		// set the analog reference (high two bits of ADMUX) and select the
		// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
		// to 0 (the default).
	#if defined(ADMUX)
	#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
		ADMUX = (ADC_reference << 4) | (pin & 0x07);
	#else
		ADMUX = (ADC_reference << 6) | (pin & 0x07);
	#endif
	#endif
}

uint16_t ADC_read(){
 
	// without a delay, we seem to read from the wrong channel
	//delayMicroseconds(2);
	//ADMUX |= (1 << ADLAR); //se si toglie si torna alla conversione a 10 bit

#if defined(ADCSRA) && defined(ADCL)

	// start the conversion
	sbi(ADCSRA, ADSC);   //ADCSRA = ADCSRA | (1 << 6);

	// ADSC is cleared when the conversion finishes
	//while (bit_is_set(ADCSRA, ADSC));
	//delayMicroseconds(0.2);
	cbi(ADCSRA, ADSC); //ADCSRA &= B10111111;

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	low  = ADCL;
	high = ADCH;
#else
	// we dont have an ADC, return 0
	low  = 0;
	high = 0;
#endif

	// combine the two bytes
	return (high << 8) | low;  


/* ADMUX = (1<<REFS0);
pin&=0b00000111;  // AND operation with 7
ADMUX = (ADMUX&0xF8)|pin; // clears the bottom 3 bits before Oring 
ADCSRA |= (1<<ADSC);
while(ADCSRA & (1<<ADSC));
return (ADC); */
}
/*
void ADC_init_continuous(uint8_t pin){
	// clear ADLAR in ADMUX (0x7C) to right-adjust the result
  // ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits)
  ADMUX &= B11011111;
 
  // Set REFS1..0 in ADMUX (0x7C) to change reference voltage to the
  // proper source (01)
  ADMUX |= B01000000;
 
  // Clear MUX3..0 in ADMUX (0x7C) in preparation for setting the analog
  // input
  ADMUX &= B11110000;
 
  // Set MUX3..0 in ADMUX (0x7C) to read from AD8 (Internal temp)
  // Do not set above 15! You will overrun other parts of ADMUX. A full
  // list of possible inputs is available in Table 24-4 of the ATMega328
  // datasheet
  ADMUX |= 8;
  // ADMUX |= B00001000; // Binary equivalent
 
  // Set ADEN in ADCSRA (0x7A) to enable the ADC.
  // Note, this instruction takes 12 ADC clocks to execute
  ADCSRA |= B10000000;
 
  // Set ADATE in ADCSRA (0x7A) to enable auto-triggering.
  ADCSRA |= B00100000;
 
  // Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
  // This means that as soon as an ADC has finished, the next will be
  // immediately started.
  ADCSRB &= B11111000;
 
  // Set the Prescaler to 128 (16000KHz/128 = 125KHz)
  // Above 200KHz 10-bit results are not reliable.
  ADCSRA |= B00000111;
 
  // Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
  // Without this, the internal interrupt will not trigger.
  ADCSRA |= B00001000;
 
  // Enable global interrupts
  // AVR macro included in <avr/interrupts.h>, which the Arduino IDE
  // supplies by default.
  sei();
 
  // Kick off the first ADC
  readFlag = 0;
  // Set ADSC in ADCSRA (0x7A) to start the ADC conversion
  ADCSRA |=B01000000;
}
/*
uint16_t ADC_read_continuous(){
	while(true){
	  // Check to see if the value has been updated
	  if (readFlag == 1){
	   
		// Perform whatever updating needed
	   
	  readFlag = 0;
	  }
  
	}
  // Whatever else you would normally have running in loop().
}

ISR(ADC_vect){

  // Done reading
  readFlag = 1;
 
  // Must read low first
  analogVal = ADCL | (ADCH << 8);
 
  // Not needed because free-running mode is enabled.
  // Set ADSC in ADCSRA (0x7A) to start another ADC conversion
  // ADCSRA |= B01000000;
}*/
