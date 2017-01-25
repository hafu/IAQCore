# iAQ-core sensor library

This is an Arduino (compatible) library for the [AMS indoor air quality sensor](http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors) [iAQ-core C](http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-C) and [iAQ-core P](http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-P).


It was tested on the TI CC3200 LaunchPad with the Arduino compatible [Energia v1.6.10E18](http://energia.nu/) IDE. If it works with other boards let me know.

## TI CC3200 LaunchPad Notes

The TI CC3200 LaunchPad I2C Bus runs in FastMode (400 kHz) but sadly the iAQ-core sensors can only handle 100 kHz, so some modificatons are necessary:  Modify ~/.energia15/packages/energia/hardware/cc3200/1.0.2/libraries/Wire/Wire.cpp

```diff
--- a/hardware/cc3200/libraries/Wire/Wire.cpp
+++ b/hardware/cc3200/libraries/Wire/Wire.cpp
@@ -48,7 +48,7 @@ void TwoWire::begin(void)
        MAP_PinTypeI2C(PIN_01, PIN_MODE_1);
        MAP_PinTypeI2C(PIN_02, PIN_MODE_1);
        MAP_PRCMPeripheralReset(PRCM_I2CA0);
-       MAP_I2CMasterInitExpClk(I2C_BASE, F_CPU, true);
+       MAP_I2CMasterInitExpClk(I2C_BASE, F_CPU, false);
 }
 
 void TwoWire::beginTransmission(uint8_t address)
@@ -69,9 +69,7 @@ uint8_t TwoWire::I2CTransact(unsigned long ulCmd)
        MAP_I2CMasterControl(I2C_BASE, ulCmd);
 
        while((MAP_I2CMasterIntStatusEx(I2C_BASE, false)
-                       & (I2C_INT_MASTER | I2C_MRIS_CLKTOUT)) == 0) {
-               if(!MAP_I2CMasterBusy(I2C_BASE)) return -1;
-       }
+                       & (I2C_INT_MASTER | I2C_MRIS_CLKTOUT)) == 0) { }
 
        /* Check for any errors in transfer */
        if(MAP_I2CMasterErr(I2C_BASE) != I2C_MASTER_ERR_NONE) {
```

### References:
 * energia/Energia issue: CC3200: Issues with I²C energia/Energia#444
 * energia/Energia issue: CC3200 i2c at 100KHz doesn't work energia/Energia#941
 * energia/cc3200-core issue: CC3200 I²C at 100KHz doesn't work energia/cc3200-core#7
