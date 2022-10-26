<!-- Please do not change this logo with link -->
[![MCHP](images/microchip.png)](https://www.microchip.com)
# Low-Power Current and Voltage Sensing Using the AVR® EA MCU

<!-- This is where the introduction to the example goes, including mentioning the peripherals used -->
Many systems includes one or more sensors that needs to be checked periodically. These signals are often low-power current or voltage signals. In battery-powered systems it is important to reduce the power consumption of the microcontroller. The signals are read using the Analog-to-Digital (ADC) peripheral, and the microcontroller is in power saving mode between measurements.

These Microchip Studio examples describe use cases for low-power current and voltage measurements on the AVR® EA family of microcontrollers:

* [Low-Power AVR® EA Current Sensing Measurements](https://bitbucket.microchip.com/projects/EBE/repos/avr64ea48-lp-voltage-current-meas-studio/browse/analog-current-sensing?at=refs%2Fheads%2Ffeature%2Finit)  
Measure a current by using the Analog-to-Digital (ADC) peripheral
* [Low-Power AVR® EA Resistance Temperature Detector (RTD) Measurements](https://bitbucket.microchip.com/projects/EBE/repos/avr64ea48-lp-voltage-current-meas-studio/browse/analog-voltage-sensing?at=refs%2Fheads%2Ffeature%2Finit)  
Measure a voltage by using the Analog-to-Digital (ADC) peripheral

## Software Used

<!-- All software used in this example must be listed here. Use unbreakable links!
     - MPLAB® X IDE 5.30 or newer [(microchip.com/mplab/mplab-x-ide)](http://www.microchip.com/mplab/mplab-x-ide)
     - MPLAB® XC8 2.10 or a newer compiler [(microchip.com/mplab/compilers)](http://www.microchip.com/mplab/compilers)
     - MPLAB® Code Configurator (MCC) 3.95.0 or newer [(microchip.com/mplab/mplab-code-configurator)](https://www.microchip.com/mplab/mplab-code-configurator)
     - MPLAB® Code Configurator (MCC) Device Libraries PIC10 / PIC12 / PIC16 / PIC18 MCUs [(microchip.com/mplab/mplab-code-configurator)](https://www.microchip.com/mplab/mplab-code-configurator)
     - Microchip PIC18F-Q Series Device Support (1.4.109) or newer [(packs.download.microchip.com/)](https://packs.download.microchip.com/) -->

* [Microchip Studio for AVR® and SAM Devices 7.0.2542](https://www.microchip.com/en-us/development-tools-tools-and-software/microchip-studio-for-avr-and-sam-devices?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_MPAE_Examples&utm_content=avr64ea48-lp-voltage-current-meas-studio-github) or newer
* [AVR® GCC 5.4.0 compiler](https://www.microchip.com/en-us/development-tools-tools-and-software/gcc-compilers-avr-and-arm?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_MPAE_Examples&utm_content=avr64ea48-lp-voltage-current-meas-studio-github) or newer

## Hardware Used
* [AVR64EA48 Curiosity Nano](https://www.microchip.com/en-us/development-tool/EV66E56A)

## Summary
<!-- Summarize what the example has shown -->
After going through these examples you should have a better understanding for how to periodically measure a voltage or current signal from a sensor while consuming as little power as possible.
