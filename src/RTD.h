#ifndef RTD_MOD
#define RTD_MOD

#include <Arduino.h>
#include <max31865_no_delay.h>
#include <SPI.h>
#include <i2c_t3.h>

#define RREF 430.0

class RTDmodule
{
private:
   int _i2c_addressRTD;
   byte _sensorPositions;
   int _sensorCount;
   int _sensorNumAtPos[4];
   long _tMeasurementStart[4];
   bool _valueRead[4];


public:
    RTDmodule(int i2c_addressRTD);
    void config(byte sensorPosition, int mode, int Rref = RREF, int wiring = 1, int FaultDetection = 0, int filter = 1);
    void startMeasurementSingle(int sensorNum);
    void startMesasurementAll();
    float readTemperatureSingle(int sensorNum);
    void readTemperatureAll();
    int sensorCount;
    float TemperatureSingle;
    float TemperatureAll[4];
    MAX31865 MAX31865rtd;
};

#endif
