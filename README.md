<!-- Please do not change this logo with link -->
[![MCHP](images/microchip.png)](https://www.microchip.com)

# Low-Power Current and Voltage Sensing Using the AVR® EA MCU

<!-- This is where the introduction to the example goes, including mentioning the peripherals used -->
Many systems include sensors that require to be periodically checked. The signals coming from these sensors are often low-power current or voltage signals. In battery-powered systems it is important to reduce the power consumption of the microcontroller. The Analog-to-Digital Converter (ADC) peripheral reads the signals while the microcontroller remains in Power Saving mode between measurements.

These Microchip Studio examples describe use cases for low-power current and voltage measurements on the AVR® EA family of microcontrollers:

- [Low-Power AVR® EA Current Sensing Measurements](analog-current-sensing)  
Measure a current by using the ADC peripheral
- [Low-Power AVR® EA Resistance Temperature Detector (RTD) Measurements](analog-voltage-sensing)  
Measure a voltage by using the ADC peripheral  

## Related Documentation

- [Acquiring Analog Sensor Data in Low Power Applications](https://www.microchip.com/DS00004886)
- [AVR64EA48 device page](https://www.microchip.com/wwwproducts/en/AVR64EA48)  

## Software Used

- [Microchip Studio for AVR® and SAM Devices 7.0.2594](https://www.microchip.com/en-us/development-tools-tools-and-software/microchip-studio-for-avr-and-sam-devices) or newer
- [Microchip AVR64EA48 Device Support Pack AVR-Ex_DFP v2.2.56](https://packs.download.microchip.com/) or newer
- [AVR® GCC 5.4.0 compiler](https://www.microchip.com/en-us/development-tools-tools-and-software/gcc-compilers-avr-and-arm) or newer
- [Atmel Data Visualizer Standalone v2.20.674](https://gallery.microchip.com/packages/AtmelDataVisualizerInstaller-Standalone/) or newer

## Hardware Used

- [AVR64EA48 Curiosity Nano](https://www.microchip.com/en-us/development-tool/EV66E56A)

## Summary
<!-- Summarize what the example has shown -->
The provided examples will give the user a better understanding of how to periodically measure a voltage or current signal from a sensor while consuming as little power as possible.  

