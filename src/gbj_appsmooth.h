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

template<class SMT, typename DAT = float>
class gbj_appsmooth
{
public:
  const String VERSION = "GBJ_APPSMOOTH 1.0.0";

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
    measures_ = max((byte)1, measures);
    smoothers_ = new Smoother[measures_];
    for (byte i = 0; i < measures_; i++)
    {
      smoothers_[i].smoother = SMT();
      smoothers_[i].valueOutput = (DAT)smoothers_[i].smoother.getValue();
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
    The overloaded methods set or reset minimum or maximum value for
    corresponding measure, i.e., defines its valid range against which the input
    value is tested.
    - If there is no measure index provided, the corresponding method acts
      on all measures in cache.

    PARAMETERS:
    data - Extreme value for particular filtering method.
      - Data type: templated
    idx - Measure index which value should be filtered counting from zero.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ (measures - 1)

    RETURN: Filtering result flag
  */
  inline void setMinimum(DAT data, byte idx)
  {
    smoothers_[idx].minimum = data;
    smoothers_[idx].flMin = true;
  }
  inline void setMinimum(DAT data)
  {
    for (byte i = 0; i < getMeasures(); i++)
    {
      setMinimum(data, i);
    }
  }
  inline void resetMinimum(byte idx) { smoothers_[idx].flMin = false; }
  inline void resetMinimum()
  {
    for (byte i = 0; i < getMeasures(); i++)
    {
      resetMinimum(i);
    }
  }
  inline void setMaximum(DAT data, byte idx)
  {
    smoothers_[idx].maximum = data;
    smoothers_[idx].flMax = true;
  }
  inline void setMaximum(DAT data)
  {
    for (byte i = 0; i < getMeasures(); i++)
    {
      setMaximum(data, i);
    }
  }
  inline void resetMaximum(byte idx) { smoothers_[idx].flMax = false; }
  inline void resetMaximum()
  {
    for (byte i = 0; i < getMeasures(); i++)
    {
      resetMaximum(i);
    }
  }

  // Getters
  inline byte getMeasures() { return measures_; }

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
  inline SMT *getMeasurePtr(byte idx = 0) { return &smoothers_[idx].smoother; }

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
    SMT smoother; // Should be after DAT members
    bool flMin; // Test for minimum if true
    bool flMax; // Test for maximum if true
    bool flValid; // Input accepted
    bool setValue(DAT val)
    {
      valueInput = val;
      if (flMin && val < minimum)
      {
        return (flValid = false);
      }
      else if (flMax && val > maximum)
      {
        return (flValid = false);
      }
      valueOutput = (DAT)smoother.getValue((DAT)val);
      return (flValid = true);
    }
  };
  Smoother *smoothers_; // List of measures' smoothers
  byte measures_; // Number of used measures
};

#endif
