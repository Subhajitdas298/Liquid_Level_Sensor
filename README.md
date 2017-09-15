## Liquid Level Sensor (Ultrasonic)

Now a day’s industries are highly automated for various applications. Level sensors detect the level of liquids and other fluids and fluidized solids, including slurries, granular materials, and powders that exhibit an upper free surface.

Ultrasonic level sensors are used for non-contact level sensing of highly viscous liquids, as well as bulk solids. They are also widely used in water treatment applications for pump control and open channel flow measurement. The sensors emit high frequency (20 kHz to 200 kHz) acoustic waves that are reflected back to and detected by the emitting transducer.

### Working

In our sensor board we have ultrasonic Tx and Rx parts. The sensor reads the distance from surface level of liquid. The Tx is basically a 40KHz speaker, which sends pulses of 40KHz Ultrasonic sound. The pulse sending time and pulse receiving time is noted for each pulse. This pulses are sensed in MCU. 

The MCU notes the time difference between and then it used sound speed to calculate distance. The MCU is to be pre calibrated to record distance from ground level, that is when the tank/container is empty. The difference is calculated and thus we get the level of the liquid.
The level is shown in 16x2 LCD display. Also other details are shown in screen.

There are two presets for maximum and minimum value limit of the pump signal generator. The the is generated when liquid level reaches over maximum limit set by the preset. The signal again goes low when it reaches below minimum limit set by other preset.
The ground distance calibration is done by a switch, which sends a signal to the atmega328 chip and it records the current distance and sets it as reference ground. 

### Application

1.Measures liquid level of any liquid.

2.The system also gives a signal to a motor driver and it can be used to start or stop an pump to fill the container.

3.It can be made to measure solid levels too.

4.The system also can be used to measure height of any object.

### Credits

http://education.rec.ri.cmu.edu/content/electronics/boe/ultrasonic_sensor/1.html

http://arduino.cc

http://playground.arduino.cc/Code/NewPing

Wikipedia
