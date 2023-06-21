
#define F_CPU (10000000UL) // CPU frequency in Hz
#define TIMEBASE_VALUE ((uint8_t) ceil(F_CPU*0.000001))

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

ISR(RTC_PIT_vect)
{
	// Real-Time Counter (RTC) - Periodic Interrupt Timer (PIT)
	// Interrupt Service Routine
	RTC.PITINTFLAGS = RTC_PI_bm; // Clear the Periodic Interrupt flag
	// and go back to normal program flow
}

int main(void)
{
	// This code periodically measures the resistance of an RTD
	// (Resistance Temperature Detector) and uses it to compute
	// the temperature.  Hardware must be connected as follows:
	// The DAC0OUT (PD6 on AVR EA) pin of the
	// device must be connected to the VREFA (PD7 on AVR EA) pin
	// of the device so the DAC output can be used to provide
	// the reference voltage for the ADC.  A 1.8 kohm resistor
	// must be connected from the DAC0OUT pin to the ADC0 AIN0
	// (PD0 on AVR EA) pin, and the RTD must be connected from the
	// ADC0 AIN0 pin to ground.  The ADC0 AIN1 pin (PD1 on AVR EA)
	// must be connected directly to ground.
	//
	// In order to measure the RTD resistance, the DAC is enabled
	// to produce an output of approximately 1.8V.  Since the DAC
	// voltage is applied to a 1.8 kohm resistor in series with
	// the RTD, the current flowing through the RTD will be just
	// under 1 mA.  The ADC is used to measure the voltage across
	// the RTD, and this allows the resistance of the RTD to be
	// computed.
	
	volatile int32_t x = 0;  // Use for saving raw accumulated ADC result
	volatile float xAverage;
	volatile float rOhm;     // Resistance in Ohms
	volatile float tempDegC; // Temperature in Degrees Celsius
	
	// Disable digital input buffer on all pins
	// to reduce power consumption (one test
	// showed this can reduce consumption by
	// more than 50 uA)
    PORTA.PINCONFIG = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PINCTRLUPD = 0xff;
    PORTB.PINCTRLUPD = 0xff;
    PORTC.PINCTRLUPD = 0xff;
    PORTD.PINCTRLUPD = 0xff;
    PORTE.PINCTRLUPD = 0xff;
    PORTF.PINCTRLUPD = 0xff;

	// Change main clock to 10 MHz by dividing by 2 (20MHz/2=10MHz)
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, (CLKCTRL_PDIV_DIV2_gc | CLKCTRL_PEN_bm));
	while(CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm){
		; // Wait for clock change to complete
	}
	
	// First step in setting up RTC and PIT is choosing clock source
	RTC.CLKSEL = RTC_CLKSEL_OSC1K_gc; // Select 1.024 kHz from OSC32K

	while(RTC.PITSTATUS & RTC_CTRLBUSY_bm){
		; // Wait for PITCTRLA sync before writing to PITCTRLA
	}
	// Write to PITCTRLA to enable PIT and determine the
	// number of 1.024 kHz clock cycles between interrupts
	RTC.PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC512_gc;
	// For example, if PIT period is 512 cycles, there will be an
	// interrupt every 0.5 seconds
	while(RTC.PITSTATUS & RTC_CTRLBUSY_bm){
		; // Wait for PITCTRLA sync before continuing
	}
	
	sei(); // Enable global interrupts
	
	// At this point the PIT is running, but not yet generating interrupts
	RTC.PITINTCTRL = RTC_PI_bm; // Enable PIT interrupt
	
	// Enable the possibility to sleep in power-down mode
	SLPCTRL.CTRLA = SLEEP_MODE_PWR_DOWN | SLPCTRL_SEN_bm;	
    
	// Digital configuration is complete, now perform analog configuration
	
	// First, set up reference voltage for DAC
	VREF.DAC0REF = VREF_ALWAYSON_bm | VREF_REFSEL_VDD_gc; // DAC reference voltage will be VDD
	// Set up DAC to produce about 1.8V
	DAC0.DATA = 559 << DAC_DATA_0_bp; // Set output to be (1.8/3.3) of VDD
	
	// Setup timebase value
	CLKCTRL.MCLKTIMEBASE = TIMEBASE_VALUE;
	ADC0.CTRLB = ADC_PRESC_DIV4_gc; // CLK_ADC = 20 MHz/4 = 5 MHz (0.2 us period)
	ADC0.CTRLC = ADC_REFSEL_VREFA_gc; // Use external reference (VREFA pin must be connected
	// through external wiring to DAC output pin)
	ADC0.CTRLD = 0; // No need for window modes
	ADC0.CTRLE = 12; // In burst mode, SAMPDUR must be >= 12 (when PGA is used)
	ADC0.CTRLF = ADC_SAMPNUM_ACC16_gc; // Accumulate 16 samples
	
	// Enable PGA with 16x gain and 100% bias current
	ADC0.PGACTRL = ADC_GAIN_16X_gc | ADC_PGABIASSEL_100PCT_gc | ADC_PGAEN_bm;
	ADC0.MUXPOS = ADC_VIA_PGA_gc | ADC_MUXPOS_AIN0_gc;
	ADC0.MUXNEG = ADC_VIA_PGA_gc | ADC_MUXNEG_AIN1_gc;

	while(1){

		sleep_cpu(); // Go to sleep until the next interrupt occurs
		
        PORTB.DIRSET = PIN3_bm;     // Make PB3 an output so state can be seen on LED0
		PORTB.OUTCLR = PIN3_bm;     // Make PB3 go low to turn on LED0
		
        //_delay_ms(100);             // the blinking will be too fast for the human eye, 
                                      // use this to see it or use oscilloscope on PB3
        
		// When waking up, first enable the DAC and ADC
		DAC0.CTRLA = DAC_OUTEN_bm | DAC_ENABLE_bm; // Enable DAC and output pin
		ADC0.CTRLA = ADC_ENABLE_bm; // Enable ADC
		
		// Command the ADC to start a differential conversion immediately
		ADC0.COMMAND = ADC_DIFF_bm | ADC_MODE_BURST_gc | ADC_START_IMMEDIATE_gc;
		
		// If there is a previous ADC result (x > 0),
		// do math for converting previous ADC result into resistance and
		// temperature while ADC is busy converting a new result
		if (x > 0) {
			xAverage = ((float) x)/16.0; // Determine average ADC result from accumulated result
			rOhm = (xAverage * 1800.0)/((0.992*16.0*2048.0) - xAverage); // Compute resistor value
			tempDegC = (rOhm - 100.0)/0.385; // Simple temperature calculation only valid for 0 to 100 C
		}
        
        PORTB.DIRCLR = PIN3_bm;     // Set PB3 as input (save power)
        PORTB.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
        
        // computation is complete
		
		while(ADC0.STATUS & ADC_ADCBUSY_bm){
			; // Wait while ADC is busy
		}
		x = ADC0.RESULT; // Save new ADC result
		ADC0.CTRLA = 0; // Disable ADC
		DAC0.CTRLA = 0; // Disable DAC and output
	}

}
