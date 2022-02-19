/*
  NAME:
  gbj_appsmooth

  DESCRIPTION:
  Library manages the statistical smoothing by various filtering algorithms.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3
  http://www.gnu.org/licenses/gpl-3.0.html (related to original code) and MIT
  License (MIT) for added code.

  CREDENTIALS:
  Author: Libor Gabaj
  GitHub: https://github.com/mrkaleArduinoLib/gbj_appsmooth.git
*/
#ifndef GBJ_APPSMOOTH_H
#define GBJ_APPSMOOTH_H

#if defined(__AVR__)
  #include <Arduino.h>
  #include <inttypes.h>
#elif defined(ESP8266) || defined(ESP32)
  #include <Arduino.h>
#endif
#include "gbj_serial_debug.h"

#undef SERIAL_PREFIX
#define SERIAL_PREFIX "gbj_appsmooth"

template<class FLT, typename DAT = float>
class gbj_appsmooth
{
public:
  const String VERSION = "GBJ_APPSMOOTH 1.0.0";

  inline gbj_appsmooth() {}

  /*
    Initialization.

    DESCRIPTION:
    The method initiates exponential filtering for statistical smoothing.
    - It creates individual filter object for each data item provided.
    - It should be called in the SETUP section of a sketch.

    PARAMETERS:
    measures - Number of measures to be filtered, which means number of used
      smoothers. It initiates (defines) valid measures range.
      - Data type: positive integer
      - Default value: 1
      - Limited range: 1 ~ 255

    RETURN: none
  */
  inline void begin(byte measures = 1)
  {
    SERIAL_VALUE("begin", measures)
    smootherCnt_ = max((byte)1, measures);
    smoothers_ = new Smoother[smootherCnt_];
    for (byte i = 0; i < smootherCnt_; i++)
    {
      smoothers_[i].smoother = FLT();
      smoothers_[i].valueOutput = smoothers_[i].smoother.getValue();
      smoothers_[i].flValid = true;
      resetMinimum(i);
      resetMaximum(i);
    }
  }

  /*
    Calculate new smoothed value from provided input value and cache them both

    DESCRIPTION:
    The method calculates a new filtered value from the input value for
    a particular data smoother determined by the index.
    - Input value as well as filtered value is cached in the smoother and can
      be returned by corresponding getter.

    PARAMETERS:
    data - Input value to be smoothed.
      - Data type: templated
    idx - Index or sequence order of a measure which value should be smoothed
      counting from zero.
      - Data type: non-negative integer
      - Default value: 0
      - Limited range: 0 ~ (measures - 1)

    RETURN:
    Flag determining whether input value has been accepted.
  */
  inline bool setValue(DAT data, byte idx = 0)
  {
    return smoothers_[idx].setValue(data);
  }

  /*
    Set filtering extreme values for particular measure

    DESCRIPTION:
    The particular setter method sets minimum or maximum value for
    corresponding measure, i.e., defines its valid range against which the input
    value is tested.
    The particular resetter method resets extreme value, so that the smoothed
    values are not test against it more.

    PARAMETERS:
    data - Extreme value for particular filtering method.
      - Data type: templated
    idx - Measure index which value should be filtered counting from zero.
      - Data type: non-negative integer
      - Default value: 0
      - Limited range: 0 ~ (measures - 1)

    RETURN: Filtering result flag
  */
  inline void setMinimum(DAT data, byte idx = 0)
  {
    smoothers_[idx].minimum = data;
    smoothers_[idx].flMin = true;
  }
  inline void resetMinimum(byte idx = 0) { smoothers_[idx].flMin = false; }
  inline void setMaximum(DAT data, byte idx = 0)
  {
    smoothers_[idx].maximum = data;
    smoothers_[idx].flMax = true;
  }
  inline void resetMaximum(byte idx = 0) { smoothers_[idx].flMax = false; }

  // Getters
  inline byte getMeasures() { return smootherCnt_; }

  /*
    Get pointer to the smoother for particular measure

    DESCRIPTION:
    The getter returns pointer to instance object of used smoothing library
    (smoother) for corresponding measure, which allows to utilize entire
    inteface of that smoother.

    PARAMETERS:
    idx - Measure index which value should be returned counting from zero.
      - Data type: non-negative integer
      - Default value: 0
      - Limited range: 0 ~ (measures - 1)

    RETURN: Pointer to smoother or NULL
  */
  inline FLT *getMeasurePtr(byte idx = 0) { return &smoothers_[idx].smoother; }

  /*
    Get characteristic value for particular measure

    DESCRIPTION:
    The particular getter returns corresponding value of corresponding measure.

    PARAMETERS:
    idx - Measure index which value should be returned counting from zero.
      - Data type: non-negative integer
      - Default value: 0
      - Limited range: 0 ~ (measures - 1)

    RETURN: Measure value or NaN
  */
  inline DAT getValue(byte idx = 0) { return smoothers_[idx].valueOutput; }
  inline DAT getInput(byte idx = 0) { return smoothers_[idx].valueInput; }
  inline DAT getMinimum(byte idx = 0) { return smoothers_[idx].minimum; }
  inline DAT getMaximum(byte idx = 0) { return smoothers_[idx].maximum; }
  inline bool isValid(byte idx = 0) { return smoothers_[idx].flValid; }

private:
  struct Smoother
  {
    DAT valueInput;
    DAT valueOutput;
    DAT minimum;
    DAT maximum;
    FLT smoother; // Should be after DAT members
    bool flMin; // Test for minimum if true
    bool flMax; // Test for maximum if true
    bool flValid; // Input accepted
    bool setValue(DAT val)
    {
      valueInput = val;
      if (flMin && val < minimum)
      {
        valueOutput = (DAT)smoother.getValue();
        return (flValid = false);
      }
      else if (flMax && val > maximum)
      {
        valueOutput = (DAT)smoother.getValue();
        return (flValid = false);
      }
      valueOutput = (DAT)smoother.getValue((DAT)val);
      return (flValid = true);
    }
  };
  Smoother *smoothers_; // List of measures' smoothers
  byte smootherCnt_; // Number of used measures
};

#endif
