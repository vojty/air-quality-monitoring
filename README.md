## Notes

### Execution time

Collecting the data & sending them via ESP-NOW takes usually around 55ms (peak 110ms) using `Forced-BME280`.

### Power consumption optimalization

#### BME280 Force mode

I've tried these libraries for a BME280 sensor:

- https://registry.platformio.org/libraries/bluedot-arduino/BlueDot%20BME280%20Library
- https://registry.platformio.org/libraries/sparkfun/SparkFun%20BME280
- https://registry.platformio.org/libraries/adafruit/Adafruit%20BME280%20Library
- https://registry.platformio.org/libraries/jvkran/Forced-BME280

Altough all of them worked in a normal mode, the best library which works for me is the last one - `Forced-BME280`.
This one is the fastest and supports the forced mode (sleep mode / low consumption mode) for BME280.

## Random observations

- 2 installed libraries for a BME280 sensor break the compilation, wtf
- ESP-NOW works only Wifi channel #1, wtf
- `Config.h` interferences with other internal config
- including files with platformio & C language is just weird. Apparently everything needs separate folder in `lib/<package>/src/*`
