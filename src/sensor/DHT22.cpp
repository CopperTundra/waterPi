/*
* GNU GENERAL PUBLIC LICENSE
* Version 3, 29 June 2007
* 
* Copyright (C) Olivier Schüwer
* 
* WaterPi is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* WaterPi is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with WaterPi.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "DHT22.h"
#include <cstdio>
#include <wiringPi.h>
#include <maxdetect.h>

DHT22::DHT22(uint8_t pin)
: pinNumber(pin)
{
    _temp = _rh = INVALID_RES;
}

bool DHT22::Init()
{
    /* No init is needed for DHT22 sensor */
    return true;
}

bool DHT22::GetHumidity(float* humidity)
{
    if(readDHTvalue(pinNumber)) {
        *humidity = (float) _rh / 10.0;
        return true;
    }
    else {
        _temp = _rh = INVALID_RES;
        return false;
    }
}

/* Customized version of the readRHT03 function from the original WiringPi library */
bool DHT22::readDHTvalue(const int pin)
{
    int result;
    struct timeval now, timeOut;
    unsigned char buffer[4];

    // the datasheet says we should wait 2 seconds before reading again
    gettimeofday(&now, NULL);
    if (timercmp(&now, &_then, <)) {
        return false;
    }

    // Set timeout for next read
    gettimeofday(&now, NULL);
    timerclear(&timeOut);
    timeOut.tv_sec = 2;
    timeradd(&now, &timeOut, &_then);

    // Read the sensor
    result = maxDetectRead(pin, buffer);

    if (!result) // Try again, but just once
        result = maxDetectRead(pin, buffer);

    if (!result)
        return false;
    _rh = (buffer[0] * 256 + buffer[1]);
    _temp = (buffer[2] * 256 + buffer[3]);

    if ((_temp & 0x8000) != 0) // Negative
    {
        _temp &= 0x7FFF;
        _temp = -_temp;
    }

    // Discard obviously bogus readings - the checksum can't detect a 2-bit error
    //	(which does seem to happen - no realtime here)
    if ((_rh > 999) || (_temp > 800) || (_temp < -400) || (_rh <= 0))
    {
        return false;
    }

    return true;
}