/*
  NAME:
  Unit tests of application library "gbj_appsmoothing" for smoothing
  with the generic library "gbj_exponential".

  DESCRIPTION:
  The test suite provides test cases for smoothing measures of various data
  types.
  - The test runner is Unity Project - ThrowTheSwitch.org.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#if defined(__AVR__)
  #include <Arduino.h>
  #include <inttypes.h>
#elif defined(ESP8266) || defined(ESP32)
  #include <Arduino.h>
#elif defined(PARTICLE)
  #include <Particle.h>
#endif
#include "gbj_appsmooth.h"
#include "gbj_exponential.h"
#include <Arduino.h>
#include <unity.h>

//******************************************************************************
// Preparation
//******************************************************************************
const float EXPONENTIAL_FACTOR = 0.2;
const float MINIMUM = 40.0;
const float MAXIMUM = 60.0;
const byte MEASURES = 2;
const byte VALUES = 5;

const float VALUES_INPUT[VALUES] = {
  38.9, 45.0, 56.7, 61.7, 42.3,
};

gbj_appsmooth<gbj_exponential, float> smoothExpFloat =
  gbj_appsmooth<gbj_exponential, float>();

gbj_appsmooth<gbj_exponential, int> smoothExpUint =
  gbj_appsmooth<gbj_exponential, int>();

void setup_float_norange()
{
  smoothExpFloat.begin(MEASURES, NAN);
  for (byte i = 0; i < smoothExpFloat.getMeasures(); i++)
  {
    smoothExpFloat.getMeasurePtr(i)->setFactor(EXPONENTIAL_FACTOR);
  }
}

void setup_float_range()
{
  smoothExpFloat.begin(MEASURES, NAN);
  for (byte i = 0; i < smoothExpFloat.getMeasures(); i++)
  {
    smoothExpFloat.getMeasurePtr(i)->setFactor(EXPONENTIAL_FACTOR);
    smoothExpFloat.setMinimum(MINIMUM * (i + 1), i);
    smoothExpFloat.setMaximum(MAXIMUM * (i + 1), i);
  }
}

void setup_uint_norange()
{
  smoothExpUint.begin(MEASURES);
  for (byte i = 0; i < smoothExpUint.getMeasures(); i++)
  {
    smoothExpUint.getMeasurePtr(i)->setFactor(EXPONENTIAL_FACTOR);
  }
}

//******************************************************************************
// Tests
//******************************************************************************
void test_version(void)
{
  String version = smoothExpFloat.VERSION;
  String valExpected = "GBJ_APPSMOOTH";
  String valActual = version.substring(0, version.indexOf(" "));
  TEST_ASSERT_EQUAL_STRING(valExpected.c_str(), valActual.c_str());
}

void test_factor_float(void)
{
  float valActual, valExpected;
  setup_float_norange();
  for (byte i = 0; i < MEASURES; i++)
  {
    valExpected = EXPONENTIAL_FACTOR;
    valActual = smoothExpFloat.getMeasurePtr(i)->getFactor();
    String msg = "i=" + String(i);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(valExpected, valActual, msg.c_str());
  }
}

void test_minimum_float(void)
{
  float valActual, valExpected;
  setup_float_range();
  for (byte i = 0; i < MEASURES; i++)
  {
    valExpected = MINIMUM * (i + 1);
    valActual = smoothExpFloat.getMinimum(i);
    String msg = "i=" + String(i);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(valExpected, valActual, msg.c_str());
  }
}

void test_maximum_float(void)
{
  float valActual, valExpected;
  setup_float_range();
  for (byte i = 0; i < MEASURES; i++)
  {
    valExpected = MAXIMUM * (i + 1);
    valActual = smoothExpFloat.getMaximum(i);
    String msg = "i=" + String(i);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(valExpected, valActual, msg.c_str());
  }
}

void test_float_norange(void)
{
  float valInput, valActual, valExpected;
  setup_float_norange();
  for (byte i = 0; i < MEASURES; i++)
  {
    for (byte j = 0; j < VALUES; j++)
    {
      valInput = VALUES_INPUT[j] * (i + 1);
      // Mimic algorithm
      if (j)
      {
        valExpected += EXPONENTIAL_FACTOR * (valInput - valExpected);
      }
      else
      {
        valExpected = valInput;
      }
      // Testee
      smoothExpFloat.setValue(valInput, i);
      valActual = smoothExpFloat.getValue(i);
      String msg = "i=" + String(i) + ", j=" + String(j);
      TEST_ASSERT_EQUAL_FLOAT_MESSAGE(valExpected, valActual, msg.c_str());
    }
  }
}

void test_float_range(void)
{
  float valInput, valActual, valExpected, valExpectedOld;
  setup_float_range();
  for (byte i = 0; i < MEASURES; i++)
  {
    bool flInit = false;
    for (byte j = 0; j < VALUES; j++)
    {
      valInput = VALUES_INPUT[j] * (i + 1);
      // Mimic algorithm
      if (valInput < (MINIMUM * (i + 1)) || valInput > (MAXIMUM * (i + 1)))
      {
        valExpected = NAN;
      }
      else if (flInit)
      {
        valExpectedOld += EXPONENTIAL_FACTOR * (valInput - valExpectedOld);
        valExpected = valExpectedOld;
      }
      else
      {
        valExpected = valInput;
        valExpectedOld = valExpected;
        flInit = true;
      }
      // Testee
      smoothExpFloat.setValue(valInput, i);
      valActual = smoothExpFloat.getValue(i);
      String msg = "i=" + String(i) + ", j=" + String(j);
      TEST_ASSERT_EQUAL_FLOAT_MESSAGE(valExpected, valActual, msg.c_str());
    }
  }
}

void test_factor_uint(void)
{
  float valActual, valExpected;
  setup_uint_norange();
  for (byte i = 0; i < MEASURES; i++)
  {
    valExpected = EXPONENTIAL_FACTOR;
    valActual = smoothExpUint.getMeasurePtr(i)->getFactor();
    String msg = "i=" + String(i);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(valExpected, valActual, msg.c_str());
  }
}

void test_uint_norange(void)
{
  unsigned int valInput, valActual, valExpected;
  float valExpectedOld;
  setup_uint_norange();
  for (byte i = 0; i < MEASURES; i++)
  {
    for (byte j = 0; j < VALUES; j++)
    {
      valInput = VALUES_INPUT[j] * (i + 1);
      // Mimic algorithm
      if (j)
      {
        valExpectedOld += EXPONENTIAL_FACTOR * (valInput - valExpectedOld);
        valExpected = valExpectedOld;
      }
      else
      {
        valExpected = valInput;
        valExpectedOld = valExpected;
      }
      // Testee
      smoothExpUint.setValue(valInput, i);
      valActual = smoothExpUint.getValue(i);
      String msg = "i=" + String(i) + ", j=" + String(j);
      TEST_ASSERT_EQUAL_UINT_MESSAGE(valExpected, valActual, msg.c_str());
    }
  }
}

//******************************************************************************
// Run
//******************************************************************************
void setup()
{
  delay(2000);
  UNITY_BEGIN();

  RUN_TEST(test_version);
  //
  RUN_TEST(test_factor_float);
  RUN_TEST(test_minimum_float);
  RUN_TEST(test_maximum_float);
  RUN_TEST(test_float_norange);
  RUN_TEST(test_float_range);
  //
  RUN_TEST(test_factor_uint);
  RUN_TEST(test_uint_norange);

  UNITY_END();
}

void loop() {}