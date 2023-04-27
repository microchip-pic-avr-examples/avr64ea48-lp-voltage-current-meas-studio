/*
 * Analog Current Sensing
 *
 * Created: 15.03.2022 13:31:34
 * Author : M51282
 */ 

// Define Main clock
#define F_CPU (20000000UL)      // 20 MHz clock

// USART Defines
#define BAUD_RATE 115200        // Define the baud rate for the USART
                                // Max BAUD_RATE = F_CPU / S [Min clock period, S = 16 for async]

// ADC Defines
#define ADC_SAMPLES 16          // Number of samples for ADC burst mode
#define ADC_GAIN 16             // Gain for PGA operation
#define ADC_REF 3.300           // ADC reference voltage in V 

// DAC Defines
#define DAC_REF 3.300           // DAC reference voltage in V 
#define DAC_OUT 1.000           // DAC output in V

// General Defines
#define TIMEBASE_VALUE ((uint8_t) ceil(F_CPU * 0.000001))
                                // TIMEBASE_VALUE = number of clock ticks in 1us
#define WAKEUP_TIME 10          // Wakeup and sample ADC each 10 seconds
#define R_SENSE 10000           // Sense resistor value in Ohm
#define BIAS_ADJUST             // add bias and offset adjustment to measurement
#define LED_ON                  // Enable blink LED (PB3) on ADC sampling
#define PGA_ON                  // turn on PGA for ADC
#define USART_ON                // Enable USART1 output on terminal


// Inlcudes
#include <avr/io.h>
#include <math.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


// Global variables
int32_t sample_acc = 0;
int16_t adc_center = 0;
int16_t adc_offset = 0;
float sample_acc_mean = 0;

float dac_val = 0;
float dac_ref = 0;
uint16_t dac_data = 0;

uint8_t timeout = 0;
float measured_voltage = 0;
float measured_current = 0;


/**************************************************************
*
*   Function definitions
*
**************************************************************/
void usart1_putc(uint8_t data);
void usart1_sendString(char *strptr);
void rev_str(char* str, int len);
int intToStr(int x, char str[], int d);
void ftostr(float n, char* res, int decimals);
void init_clock(void);
void init_PORT(void);
void init_VREF(void);
void init_DAC0(void);
void init_ADC0(void);
void measure_offset_bias(void);
void set_DAC0_output(void);
void init_RTC_PIT(void);
void init_USART1(void);
void do_ADC0_measurement(void);



/*************************************************************************
*
*   usart1_putc()
*
*   USART1 send one character/byte
*
**************************************************************************/
void usart1_putc(uint8_t data)
{
    while( !(USART1.STATUS & USART_DREIF_bm) )  // Wait for TX shift register to clear
        ;
    USART1.TXDATAL = data;                      // Send byte to TX buffer (shift register)
}

/*************************************************************************
*
*   usart1_sendString()
*
*   Send a string to terminal using USART1
*   Will send string starting at address *strptr and loop until the NULL
*   character is encountered (C automatically adds this to the end of
*   any string)
*
**************************************************************************/
void usart1_sendString(char *strptr)
{
    while (*strptr)
    {
        //loop through entire string
        usart1_putc(*strptr);
        strptr++;
    }
    while(!(USART1.STATUS & USART_TXCIF_bm))    // Wait for TXCIF to be set, indicating writing is done
        ;
    
    USART1.STATUS |= USART_TXCIF_bm;            // Clear TXCIF flag by writing a '1' to it
}


/*************************************************************************
*
*   rev_str(char* str, int len)
*
*   Reverses a string 'str' of length 'len'
*
**************************************************************************/
void rev_str(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    
    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}


/*************************************************************************
*
*   intToStr(int x, char str[], int d)
*
*   Convert integer x to string str[]
*   d =  number of digits in the output
*   if d > number of digits in x, 0s are added at the beginning
*
**************************************************************************/
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
    
    //if d is bigger than chars in x, add '0's at the beginning
    while (i < d)
    {
        str[i++] = '0';
    }
    
    rev_str(str, i);    // to print number correctly, the order must be reversed
    str[i] = '\0';      // last character is NULL
    return i;
}


/*************************************************************************
*
*   ftostr(float n, char* res, int decimals)
*
*   Convert integer x to string str[]
*   d =  number of digits in the output
*   if d > number of digits in x, 0s are added at the beginning
*
**************************************************************************/
void ftostr(float n, char* res, int decimals)
{
    // Extract integer part
    int ipart = (int)n;
    
    // Extraxt floating part
    float fpart = n - (float)ipart;
    
    // convert integer part to string
    int i = intToStr(ipart, res, 0);
    
    if(ipart == 0)
    {
        res[i++] = '0';     // if integer part zero then add zero to string
    }
    
    // Check for display option after point
    if (decimals != 0)
    {
        res[i] = '.';   // add decimal symbol ('dot')
        
        // Get the value of fraction part up to given no. of points after dot.
        // The third parameter is needed to handle cases like 233.007
        fpart = fpart * pow (10, decimals);
        intToStr( (int) fpart, res + i + 1, decimals);
    }
}



/*************************************************************************
*
*   init_clock(void)
*
*   Set main clock 
*
**************************************************************************/
void init_clock(void)
{
    // Main clock no divider --> CLK_PER = 20 MHz
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);
    
}



/*************************************************************************
*
*   init_PORT(void)
*
*   Init PORT (disable all port pins) 
*
**************************************************************************/
void init_PORT(void)
{
     PORTA.PINCONFIG = PORT_ISC_INPUT_DISABLE_gc;
     PORTA.PINCTRLUPD = 0xff;
     PORTB.PINCTRLUPD = 0xff;
     PORTC.PINCTRLUPD = 0xff;
     PORTD.PINCTRLUPD = 0xff;
     PORTE.PINCTRLUPD = 0xff;
     PORTF.PINCTRLUPD = 0xff;
}


/*************************************************************************
*
*   init_VREF(void)
*
*   Init Voltage Reference (VREF)
*
**************************************************************************/
void init_VREF(void)
{
    // Set DAC0 reference to internal 3.3V
    VREF.DAC0REF = VREF_REFSEL_VDD_gc;
}


/*************************************************************************
*
*   init_DAC0(void)
*
*   Init Digital to Analog Converter (DAC0)
*
**************************************************************************/
void init_DAC0(void)
{
    DAC0.DATA = 0 << DAC_DATA_gp;                   // Set DAC0 output = 0
    DAC0.CTRLA = DAC_OUTEN_bm | DAC_ENABLE_bm;      // Enable DAC Output, Enable DAC
    
}


/*************************************************************************
*
*   init_ADC0(void)
*
*   Init Analog to Digital Converter (ADC0)
*
*   Prescaler = 10 (ADC Clock is divided by 10)
*   ADC Reference = V_DD
*   ADC running in debug mode enabled
*   Sample Duration (SAMPDUR) is set to 12
*   Sign chopping enabled
*   Number of accumulated samples is set to 16
*
*   If PGA is on, the gain is set to 16 and the bias is set to 100%
*   This bias causes ADCPGASAMPDUR = 3us (or 4 if using int. ref)
*   See table in data sheet for ADCPGASAMPDUR for other bias settings
*
**************************************************************************/
void init_ADC0(void)
{
    CLKCTRL.MCLKTIMEBASE = TIMEBASE_VALUE;                  // Set the Timebase in the Main clock (f_CLK_PER)
    ADC0.CTRLB = ADC_PRESC_DIV10_gc;                        // Set ADC prescaler, f_CLK_ADC = f_CLK_PER/10
                                                            // CLK_PER = 20MHz --> f_CLK_ADC = 2MHz, CLK_ADC = 0,5us
                                                            // CLK_PER = 3.33MHz --> f_CLK_ADC = 333kHz, CLK_ADC = 3us (approx)
                                                            // CLK_PER = 2MHz --> f_CLK_ADC = 200kHz, CLK_ADC = 5us
    ADC0.CTRLC = ADC_REFSEL_VDD_gc;                         // Select V_DD as ADC reference
    ADC0.DBGCTRL = ADC_DBGRUN_bm;                           // Enable run in debug
    ADC0.CTRLE = 12;                                        // In Burst mode, SAMPDUR must be >= 12
    ADC0.CTRLF = ADC_CHOPPING_bm | ADC_SAMPNUM_ACC16_gc;    // Enable sign chopping (reduce offset), Accumulate 16 samples
    
    #ifdef PGA_ON
        ADC0.PGACTRL = ADC_PGAEN_bm                         // Turn on PGA
                     | ADC_GAIN_16X_gc                      // Gain 16x
                     | ADC_PGABIASSEL_100PCT_gc;            // bias 100% (CLK_ADC <= 6MHz)
    #endif
}



/*************************************************************************
*
*   measure_offset_bias(void)
*
*   Measure ADC0 offset and bias level on inputs (AIN0, AIN1)
*
**************************************************************************/
void measure_offset_bias(void)
{
    ADC0.CTRLA = ADC_ENABLE_bm;                             // Enable ADC0
    
    // Measure offset (sample 16 times)
    #ifdef PGA_ON                                           // If PGA is enabled
        ADC0.MUXPOS = ADC_VIA_PGA_gc | ADC_MUXPOS_AIN0_gc;  // Sending the same signal to MUXPOS and MUXNEG (PD0)
        ADC0.MUXNEG = ADC_VIA_PGA_gc | ADC_MUXNEG_AIN0_gc;  // The signals should cancel each other out
    #else
        ADC0.MUXPOS = ADC_MUXPOS_AIN0_gc;                   // else PGA is disabled so same but without VIA on
        ADC0.MUXNEG = ADC_MUXNEG_AIN0_gc; 
    #endif
    
    while(ADC0.STATUS != 0)                                 // Wait for ADC0 to be ready (ADC needs some time
        ;                                                   // when changing reference or inputs)

    ADC0.COMMAND = ADC_DIFF_bm                              // Differential input mode                
                 | ADC_MODE_BURST_gc                        // Use Burst mode
                 | ADC_START_IMMEDIATE_gc;                  // Start immediately
    
    while(ADC0.COMMAND & ADC_START_IMMEDIATE_gc)            // Wait for conversion to finish
        ;
    
    adc_offset = ADC0.RESULT;                               // Read result
    
    // Measure the ADC input signal base level (bias)
    #ifdef PGA_ON                                           // If PGA is enabled
        ADC0.MUXPOS = ADC_VIA_PGA_gc | ADC_MUXPOS_AIN1_gc;  // Reconfigure ADC inputs, MUXPOS = AIN1
        ADC0.MUXNEG = ADC_VIA_PGA_gc | ADC_MUXNEG_AIN0_gc;  // MUXNEG = AIN0
    #else
        ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;                   // else PGA is disabled so same but without VIA on
        ADC0.MUXNEG = ADC_MUXNEG_AIN0_gc;
    #endif
    
    while(ADC0.STATUS != 0)
        ;                                                   // Wait for ADC0 to be ready
    
    ADC0.COMMAND = ADC_DIFF_bm                              // Differential input mode  
                 | ADC_MODE_BURST_gc                        // Use Burst mode 
                 | ADC_START_IMMEDIATE_gc;                  // Start immediately
    
    while(ADC0.COMMAND & ADC_START_IMMEDIATE_gc)            // Wait for conversion to finish
        ;
    
    sample_acc = ADC0.RESULT;                               // Read result
    adc_center = sample_acc - adc_offset;                   // Adjust for offset
    
    ADC0.CTRLA = 0;                                         // Disable ADC0
    DAC0.CTRLA = 0;                                         // Disable DAC0
}



/*************************************************************************
*
*   set_DAC0_output(void)
*
*   Set the DAC0 output to value in V defined by DAC_OUT (see #Defines)
*
**************************************************************************/
void set_DAC0_output(void)
{
    dac_ref = DAC_REF;                          // DAC0 Reference value
    dac_val = DAC_OUT;                          // Set DAC0 output to DAC_OUT
    dac_data = dac_val / (dac_ref / 1024);      // DAC is 10 bit (2^10 = 1024) --> DAC0.DATA = dac_val / (dac_ref / 1024)
    DAC0.DATA = dac_data << DAC_DATA_gp;        // Write value to DAC0.DATA register
}



/*************************************************************************
*
*   init_RTC_PIT(void)
*
*   Enable RTC and PIT
*
*   RTC clock is set to 1kHz. PIT is set to trigger each 1s (1024 cycles)
*
**************************************************************************/
void init_RTC_PIT(void)
{
    
    while (RTC.STATUS > 0)                      // Wait for RTC to be synchronized
        ;
     
    RTC.CLKSEL = RTC_CLKSEL_OSC1K_gc;           // RTC clock is 1.024kHz from OSC32K
    
    while(RTC.PITSTATUS > 0)
        ;                                       // Wait for PIT to be synchronized
    
    RTC.PITCTRLA = RTC_PITEN_bm                 // Enable PIT 
                 | RTC_PERIOD_CYC1024_gc;       // approx. every 1s (1024 cycles on 1kHz clock)
    RTC.PITDBGCTRL = RTC_DBGRUN_bm;             // Allow RTC to run in debug mode
    
    while(RTC.PITSTATUS > 0)                    // Wait for PIT sync
        ;

}



/**********************************************************************************
*
*   init_USART1(void)
*
*   BAUD = (64 * F_CPU) / (S * BAUD_RATE)
*   S = no. of samples (16 for async)
*   Note! Max BAUD_RATE = F_CPU / S (see #Defines), F_CPU = main clock frequency
*
**********************************************************************************/
void init_USART1(void)
{
    USART1.BAUD = (64.0 * F_CPU) / (16.0 * BAUD_RATE); 
    
    PORTC.DIRSET = PIN0_bm;                     // set PC0 as output (USART1 TxD)
    USART1.CTRLB = USART_TXEN_bm;               // enable usart1 TX
}


/***********************************************************************************************
*
*   do_ADC0_measurement(void)
*
*   Perform ADC measurement using burst mode and 16 samples (as set up in init_ADC0)
*   While sampling, do calculation based on previous sampling. Typically you will have some
*   idle time while waiting for ADC to be done since you are accumulating samples.
*
*   Time required for conversion is:
*   t_conv = t_init + (((SAMPDUR + 2) * SAMPNUM + 14) / f_CLK_ADC) + ADCPGASAMPDUR * SAMPNUM
*   2MHz: t_conv = 20us (PGA) + ((12+2)*16 + 14) / 200k + 12us*16 = 20 + 1190 + 192 = 1402 us
*   20MHz: t_conv = 20us (PGA) + ((12+2)*16 + 14) / 2000000 + 3us*16 = 20 + 119 + 48 = 187 us
*   20MHz: t_conv = 20us (PGA) + ((12+2)*16 + 14) / 2000000 + 6us*16 = 20 + 119 + 96 = 235 us
*   
*   Without PGA the formula is: 
*   t_init + (((SAMPDUR+14)*SAMPNUM + 1.5) / f_CLK_ADC)
*   20MHz w/o PGA: t_conv = 10us (ADC) + ( (12 + 2)*16 + 1.5) / 2000000 = 122.75 us
*
************************************************************************************************/
void do_ADC0_measurement(void)
{
    float adc_samples = 0;
    float adc_gain = 0;
    float adc_ref = 0;
    float r_sense = 0;
    
    #ifdef USART_ON                                                 // If USART is enabled, define array to convert result to string
        char res[20];
    #endif
    
    DAC0.CTRLA = DAC_OUTEN_bm | DAC_ENABLE_bm;                      // DAC Output enable, Enable DAC
    ADC0.CTRLA = ADC_ENABLE_bm;                                     // Enable ADC
    
    adc_samples = ADC_SAMPLES;                                      // read defined number of samples
    adc_gain = ADC_GAIN;                                            // read defined gain (PGA)
    adc_ref = ADC_REF;                                              // read defined ADC reference
    r_sense = R_SENSE;                                              // read sense resistor value
    
    while(ADC0.STATUS > 0)                                          // wait for ADC ready
        ;
    
    // Start differential ADC conversion, burst mode (16x)
    ADC0.COMMAND = ADC_DIFF_bm | ADC_MODE_BURST_gc | ADC_START_IMMEDIATE_gc;
    
    //While ADC is sampling, calculate previous measurement
    #ifdef BIAS_ADJUST
        sample_acc = sample_acc - adc_offset - adc_center;          // Adjust for offset and bias
    #endif
    
    #ifdef PGA_ON
        sample_acc = sample_acc / adc_gain;                         // remove gain (16x)
    #endif
    
    sample_acc_mean = sample_acc / adc_samples;                     // Get mean value
    
    measured_voltage = (( sample_acc_mean * adc_ref ) / 2048);      // Calculate voltage
    measured_current = ( measured_voltage / r_sense ) * 1000000;    // calculate current in uA
    
    #ifdef USART_ON                                                 // Send measurement to terminal (USART1)
        ftostr(measured_voltage, res, 4);
        usart1_sendString("Measured voltage: ");
        usart1_sendString(res);
        usart1_sendString("V\n");
        
        measured_current = measured_current + 0.05;                 // add 0.05 to fix rounding errors
        ftostr(measured_current, res, 1);
        usart1_sendString("Measured current: ");
        usart1_sendString(res);
        usart1_sendString("uA\n");
    #endif
    
    while( !(ADC0.INTFLAGS & ADC_RESRDY_bm) )                       // wait until RESRDY flag is set
        ;
    
    sample_acc = ADC0.RESULT;                                       // Read ADC result
    
    ADC0.CTRLA = 0;                                                 // Disable ADC
    DAC0.CTRLA = 0;                                                 // Disable DAC
}


/********************************************************************************
*
*   ISR(RTC_PIT_vect)
*
*   Interrupt Service Routine function for PIT interrupts (RTC)
*
********************************************************************************/
ISR(RTC_PIT_vect)
{
    timeout--;                                  // Decrement timeout variable
    RTC.PITINTFLAGS = RTC_PI_bm;                // Clear PIT interrupt flag
}



/****************************************************************************
*
*   main()
*
*   This example code will periodically wake up and sample the ADC.
*   The code example uses the PIT events of the RTC peripheral to create
*   ~1 second interrupts, and the #define WAKEUP_TIME determines how many
*   seconds between each ADC sampling.
*
*   The resulting voltage measurement is sent out on USART0 as ASCII text.
*   Enable terminal to see result, baud rate defined in #define BAUD_RATE.
*   (standard 8 data bit, no parity bit, 1 stop bit)
*
*   LED blink is determined by #define LED_ON included or not
*
*****************************************************************************/
int main(void)
{
    init_clock();                               // Set main clock (CLK_PER) 
    init_PORT();                                // Disable all port pins to reduce current consumption
    init_VREF();                                // Init Voltage Reference (VREF)
    init_DAC0();                                // Init DAC0
    init_ADC0();                                // Init ADC0
    
    measure_offset_bias();                      // Measure ADC0 offset and bias on inputs (AIN0 + AIN1)
    
    set_DAC0_output();                          // Set DAC output voltage as defined by DAC_OUT (see #defines)
    init_RTC_PIT();                             // Init RTC and PIT
    
    #ifdef USART_ON
        init_USART1();                          // Init USART
        usart1_sendString("Let's go! \n");      // Send start message
    #endif
    
    SLPCTRL.CTRLA = SLEEP_MODE_PWR_DOWN
                  | SLPCTRL_SEN_bm;             // Enable the possibility to sleep in power-down mode
    
    timeout = WAKEUP_TIME;                      // Set timeout = number of seconds between measurements
    sei();                                      // Enable global interrupts
    RTC_PITINTCTRL = RTC_PI_bm;                 // Enable PIT interrupt
    
    while (1) 
    {
        sleep_cpu();                            // go to sleep
        cli();                                  // so woke, disable interrupts
        
        if(timeout < 1)                         // are we in timeout now?
        {
            #ifdef LED_ON                       // should we blink the LED?
                PORTB.DIRSET = PIN3_bm;         // set PB3 as output (LED0)
                PORTB.OUTCLR = PIN3_bm;         // turn on LED0 (active low)
            #endif
            
            // do ADC measurement, send result to terminal if enabled
            do_ADC0_measurement();
            
            timeout = WAKEUP_TIME;                          // Reset timeout
            
            #ifdef LED_ON
                PORTB.DIRCLR = PIN3_bm;                     // PD7 input
                PORTB.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc; // Turn off LED and disable PB3
            #endif
        }
        sei();                                              // enable global interrupts
    }
}

