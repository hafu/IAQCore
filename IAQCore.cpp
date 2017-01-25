/**
 * @file IAQCore.cpp
 * @author Hannes Fuchs
 * @date 2017-01-25
 * @copyright MIT
 * @brief Header file for iAQ-Core C/P Sensor library
 *
 * This is a simple libray for the indoor air quality sensors iAQ-core C and
 * iAQ-core P from AMS. Mor information about these sensors could be found
 * under http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors
 *
 * @see http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors
 * @see http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-C
 * @see http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-P
 */

#include <IAQCore.h>


/**
 * @breif The main Class for the iAQ-core sensor library
 */
IAQCore::IAQCore() {
}

/**
 * @brief Initalizates the I2C Bus and sets the sensor type
 * @param type The sensor type C or P, see IAQCore.h
 */
void IAQCore::begin(uint8_t type) {
    Wire.begin();
    _type = type;

    // atm there are only two types
    if (type == IAQCORE_TYPE_C) {
        _lastreadtime = -IAQCORE_MEASUREMENT_INTERVAL_TYPE_C;
        _measurementinterval = IAQCORE_MEASUREMENT_INTERVAL_TYPE_C;
    } else {
        _lastreadtime = -IAQCORE_MEASUREMENT_INTERVAL_TYPE_P;
        _measurementinterval = IAQCORE_MEASUREMENT_INTERVAL_TYPE_P;
    }

    // set unofficial status
    _data[IAQCORE_STATUS_OFFSET] = IAQCORE_STATUS_NOT_UPDATED_AT_ALL;
}

/**
 * @brief Wrapper for private function readBytesFromSensor()
 *
 * Updates only the status and the CO2 prediction value.
 *
 * @return Sensor status, see IAQCore.h for values
 */
uint8_t IAQCore::update(void) {
    return readBytesFromSensor(IAQCORE_READ_CO2_AND_STATUS);
}

/**
 * @brief Wrapper for private function readBytesFromSensor()
 * 
 * Updates all possible sensor values; including CO2 prediction, status, 
 * sensor resistance and TVOC prediction values.
 *
 * @return Returns the status of an update, see IAQCore.h for values
 */
uint8_t IAQCore::updateAll(void) {
    return readBytesFromSensor(IAQCORE_READ_ALL);
}

/**
 * @brief Calculates the CO2 prediction value and returns it.
 *
 * @return Calculated CO2 prediction value
 */
uint16_t IAQCore::getCO2PredictionPPM(void) {
    uint16_t tmp = _data[IAQCORE_CO2_PREDICTION_MSB_OFFSET];
    tmp <<= 8;
    tmp += _data[IAQCORE_CO2_PREDICTION_LSB_OFFSET];
    return tmp;
}

/**
 * @brief Just returns the status of the sensor
 *
 * @return Sensor status, see IAQCore.h for values
 */
uint8_t IAQCore::getStatus(void) {
    return _data[IAQCORE_STATUS_OFFSET];
}

/**
 * @brief Calculates the sensors resistance value and returns it.
 *
 * @return Calculated sensor resistance value
 */
uint32_t IAQCore::getSensorResistanceOhm(void) {
    uint32_t tmp = _data[IAQCORE_RESISTANCE_MSB_OFFSET];
    tmp <<= 8;
    tmp += _data[IAQCORE_RESISTANCE_MID_OFFSET];
    tmp <<= 8;
    tmp += _data[IAQCORE_RESISTANCE_LSB_OFFSET];
    return tmp;
}

/**
 * @brief Calculates the TVOC prediction value and returns it.
 *
 * @return Calculated TVOC prediction value
 */
uint16_t IAQCore::getTVOCPredictionPPB(void) {
    uint16_t tmp = _data[IAQCORE_TVOC_PREDICTION_MSB_OFFSET];
    tmp <<= 8;
    tmp += _data[IAQCORE_TVOC_PREDICTION_LSB_OFFSET];
    return tmp;
}

/**
 * @brief Reads the sensor, save the values in array and returns status
 *
 * On fast readings (specaly the P version) the last status will be returned
 * and the _data array will NOT be updated. Some custom status where added to
 * catch some more errors.
 *
 * @param bytes How many Bytes should be read from the sensor
 * @return Sensor status, see IAQCore.h for values
 */
uint8_t IAQCore::readBytesFromSensor(uint8_t bytes) {
    // return quickly if we read to fast
    uint32_t currenttime = millis();
    if ((currenttime - _lastreadtime) < _measurementinterval) {
        DEBUG_PRINTLN(F("Measurement interval to short, return last status"));
        return getStatus();
    }

    // set measurement time
    _lastreadtime = currenttime;

    // reset data, sizeof(data) should be known at compile time
    memset(_data, 0x00, sizeof(_data));
    // to be safe, this status should be overwritten
    _data[IAQCORE_STATUS_OFFSET] = IAQCORE_STATUS_UPDATING;

    // do we need this? see IAQCore.h on line 46
    /*
    Wire.beginTransmission(IAQCORE_DEF_I2C_ADDR);
    Wire.write(IAQCORE_START_READING);
    Wire.endTransmission();
    */

    DEBUG_PRINTLN(F("Request data from sensor"));
    Wire.requestFrom((uint8_t)IAQCORE_DEF_I2C_ADDR, (uint8_t)bytes, (uint8_t)true);
    if (Wire.available() != bytes) {
        DEBUG_PRINTLN(F("Request failed"));
        _data[IAQCORE_STATUS_OFFSET] = IAQCORE_STATUS_I2C_REQ_FAILED;
        return getStatus();
    }
    // read into buffer
    DEBUG_PRINT(F("Received: "));
    for (uint8_t i=0; i<bytes; i++) {
        _data[i] = Wire.read();
        DEBUG_PRINT(_data[i], HEX); DEBUG_PRINT(F(" "));
    }
    DEBUG_PRINTLN(F(""));

    return getStatus();
}
