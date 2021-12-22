#include "RTD.h"

RTDmodule::RTDmodule(int i2c_addressRTD)
    : MAX31865rtd(0)
{
    _i2c_addressRTD = i2c_addressRTD;
}

void RTDmodule::config(byte sensorPosition, int Rref, int mode, int wiring = 1, int FaultDetection = 0, int filter = 1)
{
    _sensorPositions = sensorPosition;

    _sensorCount = 0;

    MAX31865rtd.config(Rref, mode, wiring, FaultDetection, filter);

    for(int _i = 0; _i < 4; _i++)
    {
        if(bitRead(_sensorPositions, _i))
        {
            _sensorCount++;
            _sensorNumAtPos[_sensorCount] = _i+1;
        }
    }
    sensorCount = _sensorCount;
    

    Wire.beginTransmission(_i2c_addressRTD);
    Wire.write(0xFF);
    Wire.endTransmission();
}

void RTDmodule::startMeasurementSingle(int sensorNum)
{
    if((millis() - _tMeasurementStart[sensorNum] >= 65) && _valueRead[sensorNum])
    {
    _valueRead[sensorNum] = false;

    Wire.beginTransmission(_i2c_addressRTD);
    Wire.write(B11111111 ^ (B00000001 << _sensorNumAtPos[sensorNum]));
    Wire.endTransmission();

    MAX31865rtd.startMeasurement();
    _tMeasurementStart[sensorNum] = millis();

    Wire.beginTransmission(_i2c_addressRTD);
    Wire.write(0xFF);
    Wire.endTransmission();
    }
}

void RTDmodule::startMesasurementAll()
{
    for(int _i = 0; _i < _sensorCount; _i++)
    {
        startMeasurementSingle(_sensorNumAtPos[_i]);
    }
}

float RTDmodule::readTemperatureSingle(int sensorNum)
{
    Wire.beginTransmission(_i2c_addressRTD);
    Wire.write(B11111111 ^ (B00000001 << _sensorNumAtPos[sensorNum]));
    Wire.endTransmission();

    TemperatureSingle = MAX31865rtd.read();
    _valueRead[sensorNum] = true;

    Wire.beginTransmission(_i2c_addressRTD);
    Wire.write(0xFF);
    Wire.endTransmission();

    return TemperatureSingle;
}

void RTDmodule::readTemperatureAll()
{
    for(int _i = 0; _i < _sensorCount; _i++)
    {
       TemperatureAll[_i] = readTemperatureSingle(_sensorNumAtPos[_i]);
    }
}
