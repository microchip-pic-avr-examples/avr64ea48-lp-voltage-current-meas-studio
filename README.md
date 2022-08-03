<!-- Please do not change this logo with link -->

[![MCHP](images/microchip.png)](https://www.microchip.com)

# Update the title for avr64ea48-lp-voltage-current-meas-studio here

<!-- This is where the introduction to the example goes, including mentioning the peripherals used -->



##  Low-Power AVR® EA Current Sensing Measurements
This example shows how to use the AVR64EA48 microcontroller to measure a current by using the Analog-to-Digital (ADC) peripheral. Additional components are needed since the ADC can only sample a voltage signal.

Some sensors, like photodiodes, phototransistors and some temperature sensors, will output a current signal. Use the 12-bit ADC peripheral to measure the signal coming from such sensors. The ADC can only convert voltages. To measure a current, send the current through a "sense" resistor and measure the voltage drop across it. Then use this to calculate the current.

## Low-Power AVR EA Resistance Temperature Detector (RTD) Measurements 

This example uses the AVR EA microcontroller to periodically drive
a Resistance Temperature Detector (RTD) with current, measure the
voltage across the RTD, and calculate both resistance and
temperature of the RTD.  The only hardware needed in addition to
the microcontroller is a 1.8 kohm fixed resistor and the RTD itself.

## Software Used

<!-- All software used in this example must be listed here. Use unbreakable links!
     - MPLAB® X IDE 5.30 or newer [(microchip.com/mplab/mplab-x-ide)](http://www.microchip.com/mplab/mplab-x-ide)
     - MPLAB® XC8 2.10 or a newer compiler [(microchip.com/mplab/compilers)](http://www.microchip.com/mplab/compilers)
     - MPLAB® Code Configurator (MCC) 3.95.0 or newer [(microchip.com/mplab/mplab-code-configurator)](https://www.microchip.com/mplab/mplab-code-configurator)
     - MPLAB® Code Configurator (MCC) Device Libraries PIC10 / PIC12 / PIC16 / PIC18 MCUs [(microchip.com/mplab/mplab-code-configurator)](https://www.microchip.com/mplab/mplab-code-configurator)
     - Microchip PIC18F-Q Series Device Support (1.4.109) or newer [(packs.download.microchip.com/)](https://packs.download.microchip.com/) -->

- Microchip Studio for AVR® and SAM Devices 7.0.2542 or newer [(Microchip Studio for AVR® and SAM Devices 7.0.2542)](https://www.microchip.com/en-us/development-tools-tools-and-software/microchip-studio-for-avr-and-sam-devices?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_MPAE_Examples&utm_content=avr64ea48-lp-voltage-current-meas-studio-github)
- AVR® GCC 5.4.0 or newer compiler [(AVR® GCC 5.4)](https://www.microchip.com/en-us/development-tools-tools-and-software/gcc-compilers-avr-and-arm?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_MPAE_Examples&utm_content=avr64ea48-lp-voltage-current-meas-studio-github)

## Hardware Used
 - AVR64EA48 





<!-- Summarize what the example has shown -->
