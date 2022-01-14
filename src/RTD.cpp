#include "RTD.h"

RTDmodule::RTDmodule(int i2c_addressRTD)
    : MAX31865rtd(0), _i2c_addressRTD(i2c_addressRTD)
{
}

void RTDmodule::config(byte sensorPositions, int mode, int Rref, int wiring = 1, int FaultDetection = 0, int filter = 1)
{
    // Configure the module by for use.

    // Inputs:
    //  sensorPositions:    A byte containing the slots of the module that are occupied by MAX31865 breakout boards
    //                      e.g. 00000101 → Slots 1 an 3 are occupied
    //
    //  Rref:               Value of the reference resistor (430 ohms for Pt100)
    //
    //  mode:               0 → auto mode (continous measurement)
    //                      1 → normaly off (until one-shot measurement or changed to auto)
    //
    //  wiring:             0 → 2-wire or 4-wire configuration
    //                      1 → 3-wire configuration
    //
    //  FaultDetection:     0 → fault detection off
    //                      1 → fault detection on
    //
    // filter:              0 → 60Hz filter
    //                      1 → 50Hz filter

    _sensorPositions = sensorPositions;

    // Configure one single MAX31865 object to be used for all plugged modules
    MAX31865rtd.config(Rref, mode, wiring, FaultDetection, filter);

    _sensorCount = 0;

    for (int _i = 0; _i < 4; _i++)
    {
        // Check the sensor positions byte for the occupied slots
        if (bitRead(_sensorPositions, _i))
        {
            // Count number of sensors
            _sensorCount++;

            // Make a slot number array
            _sensorNumAtPos[_sensorCount] = _i + 1;
        }
    }
    sensorCount = _sensorCount;

    // Set all CS-pins of the module to high
    Wire.beginTransmission(_i2c_addressRTD);
    Wire.write(0xFF);
    Wire.endTransmission();
}

void RTDmodule::startMeasurementSingle(int sensorNum)
{
    // Start a measurement with a single MAX31865
    //
    // sensorNum    Number of the sensor on which the measurement is to be started

    // Start a new measurement only if at least 65 milliseconds passed and the sensor was read already
    if ((millis() - _tMeasurementStart[sensorNum] >= 65) && _valueRead[sensorNum])
    {
        _valueRead[sensorNum] = false;

        // Pull down the CS pin of the defined MAX31865 and send the configuration byte to start a measurement.
        // Set all CS pins to high after.

        // Set the CS pin of the according MAX31865 to low
        Wire.beginTransmission(_i2c_addressRTD);
        Wire.write(B11111111 ^ (B00000001 << _sensorNumAtPos[sensorNum]));
        Wire.endTransmission();

        // Send configuration byte to the sensor to start the measurement
        MAX31865rtd.startMeasurement();
        _tMeasurementStart[sensorNum] = millis();

         // Set all CS pins back to high
        Wire.beginTransmission(_i2c_addressRTD);
        Wire.write(0xFF);
        Wire.endTransmission();
    }
}

void RTDmodule::startMesasurementAll()
{
    // Starts measurement with all configured MAX31865s

    for (int _i = 0; _i < _sensorCount; _i++)
    {
        startMeasurementSingle(_sensorNumAtPos[_i]);
    }
}

float RTDmodule::readTemperatureSingle(int sensorNum)
{
    // Read temperatures from a single MAX31865
    //
    // sensorNum    Number of the sensor on which is to be read

    // Set CS pin to low for the according MAX31865
    Wire.beginTransmission(_i2c_addressRTD);
    Wire.write(B11111111 ^ (B00000001 << _sensorNumAtPos[sensorNum]));
    Wire.endTransmission();

    // Read the temperature value
    TemperatureSingle = MAX31865rtd.read();
    _valueRead[sensorNum] = true;

    // Set all CS pins back to high
    Wire.beginTransmission(_i2c_addressRTD);
    Wire.write(0xFF);
    Wire.endTransmission();

    return TemperatureSingle;
}

void RTDmodule::readTemperatureAll()
{
    // Read temperature from all configured MAX31865s

    for (int _i = 0; _i < _sensorCount; _i++)
    {
        TemperatureAll[_i] = readTemperatureSingle(_sensorNumAtPos[_i]);
    }
}
