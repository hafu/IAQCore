#include <IAQCore.h>

// comment out the specific sensor type
#define SENSOR_TYPE IAQCORE_TYPE_C
//#define SENSOR_TYPE IAQCORE_TYPE_P

// reads all information from sensor if defined, comment in to read only status
// and CO2 prediction
#define SENSOR_UPDATE_ALL
/**
 * Note:
 * Most users just want the status and the CO2 prediciton, if you want more 
 * (sensor restance and TVOC prediction) use the updateAll() function
 */

IAQCore iaqcore;

void setup() {
    Serial.begin(9600);
    while(!Serial) {
        ; // wait ...
    }

    if (SENSOR_TYPE == IAQCORE_TYPE_C) {
        Serial.println("iAQ-Core C Test"); 
    } else {
        Serial.println("iAQ-Core P Test");
    }
    iaqcore.begin(SENSOR_TYPE);
}

void loop() {

    Serial.print("Status: ");
#ifdef SENSOR_UPDATE_ALL
    switch(iaqcore.updateAll()) {
#else
    switch(iaqcore.update()) {
#endif
        case IAQCORE_STATUS_OK:
            Serial.println("OK");
            break;
        case IAQCORE_STATUS_RUNIN:
            Serial.println("Warming up ...");
            break;
        case IAQCORE_STATUS_BUSY:
            Serial.println("Busy ...");
            break;
        case IAQCORE_STATUS_ERROR:
            Serial.println("ERROR!");
            break;
        case IAQCORE_STATUS_NOT_UPDATED_AT_ALL:
            Serial.println("Sensor not updated");
            break;
        case IAQCORE_STATUS_UPDATING:
            Serial.println("Updating in progress");
            break;
        case IAQCORE_STATUS_I2C_REQ_FAILED:
            Serial.println("I2C Request failed");
            break;
        default:
            Serial.print("Undefined status code: ");
            Serial.println(iaqcore.getStatus(), HEX);
            break;
    }

    if (iaqcore.getStatus() == IAQCORE_STATUS_OK) {
        Serial.print("CO2 Prediction:    ");
        Serial.print(iaqcore.getCO2PredictionPPM());
        Serial.println(" ppm");
#ifdef SENSOR_UPDATE_ALL
        Serial.print("Sensor Resistance: ");
        Serial.print(iaqcore.getSensorResistanceOhm());
        Serial.println(" Ohm");

        Serial.print("TVOC Prediction:   ");
        Serial.print(iaqcore.getTVOCPredictionPPB());
        Serial.println(" ppb");
#endif
    }

    if (SENSOR_TYPE == IAQCORE_TYPE_C) {
        delay(IAQCORE_MEASUREMENT_INTERVAL_TYPE_C);
    } else {
        delay(IAQCORE_MEASUREMENT_INTERVAL_TYPE_P);
    }


}
