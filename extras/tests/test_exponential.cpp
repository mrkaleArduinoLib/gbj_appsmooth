/*
  NAME:
  Unit tests of application library "gbj_appsmoothing" for smoothing
  with the generic library "gbj_exponential".

  DESCRIPTION:
  The test file provides test cases for smoothing measures of various data
  types.
  - The test runner is Unity Project - ThrowTheSwitch.org.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#define SERIAL_NODEBUG
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
const float DIFFERENCE = 10.0;
const byte MEASURES = 2;

const float SAMPLE_LIST[] = { 42.3, 38.9, 45.0, 56.7, 61.7, 52.3, 41.1, 48.4 };
const byte SAMPLES = sizeof(SAMPLE_LIST) / sizeof(SAMPLE_LIST[0]);

gbj_appsmooth<gbj_exponential, float> smoothFloat =
  gbj_appsmooth<gbj_exponential, float>();

gbj_appsmooth<gbj_exponential, unsigned int> smoothUint =
  gbj_appsmooth<gbj_exponential, unsigned int>();

void setup_float_norange()
{
  smoothFloat.begin(MEASURES);
  for (byte i = 0; i < smoothFloat.getMeasures(); i++)
  {
    smoothFloat.getMeasurePtr(i)->setFactor(EXPONENTIAL_FACTOR);
  }
}

void setup_float_norange_difference()
{
  smoothFloat.begin(MEASURES);
  for (byte i = 0; i < smoothFloat.getMeasures(); i++)
  {
    smoothFloat.getMeasurePtr(i)->setFactor(EXPONENTIAL_FACTOR);
    smoothFloat.setDifference(DIFFERENCE * (i + 1), i);
  }
}

void setup_float_range()
{
  smoothFloat.begin(MEASURES);
  for (byte i = 0; i < smoothFloat.getMeasures(); i++)
  {
    smoothFloat.getMeasurePtr(i)->setFactor(EXPONENTIAL_FACTOR);
    smoothFloat.setMinimum(MINIMUM * (i + 1), i);
    smoothFloat.setMaximum(MAXIMUM * (i + 1), i);
  }
}

void setup_float_range_difference()
{
  smoothFloat.begin(MEASURES);
  for (byte i = 0; i < smoothFloat.getMeasures(); i++)
  {
    smoothFloat.getMeasurePtr(i)->setFactor(EXPONENTIAL_FACTOR);
    smoothFloat.setMinimum(MINIMUM * (i + 1), i);
    smoothFloat.setMaximum(MAXIMUM * (i + 1), i);
    smoothFloat.setDifference(DIFFERENCE * (i + 1), i);
  }
}

void setup_uint_norange()
{
  smoothUint.begin(MEASURES);
  for (byte i = 0; i < smoothUint.getMeasures(); i++)
  {
    smoothUint.getMeasurePtr(i)->setFactor(EXPONENTIAL_FACTOR);
  }
}

//******************************************************************************
// Tests
//******************************************************************************
void test_version(void)
{
  String version, valExpected, valActual;
  version = String(smoothFloat.VERSION);
  valExpected = "GBJ_APPSMOOTH";
  valActual = version.substring(0, version.indexOf(" "));
  TEST_ASSERT_EQUAL_STRING(valExpected.c_str(), valActual.c_str());
}

void test_factor_float(void)
{
  float valActual, valExpected;
  setup_float_norange();
  for (byte i = 0; i < MEASURES; i++)
  {
    valExpected = EXPONENTIAL_FACTOR;
    valActual = smoothFloat.getMeasurePtr(i)->getFactor();
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
    valActual = smoothFloat.getMinimum(i);
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
    valActual = smoothFloat.getMaximum(i);
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
    for (byte j = 0; j < SAMPLES; j++)
    {
      valInput = SAMPLE_LIST[j] * (i + 1);
      // Algorithm
      if (j)
      {
        valExpected += EXPONENTIAL_FACTOR * (valInput - valExpected);
      }
      else
      {
        valExpected = valInput;
      }
      // Testee
      smoothFloat.setValue(valInput, i);
      valActual = smoothFloat.getValue(i);
      String msg = "i=" + String(i) + ", j=" + String(j);
      TEST_ASSERT_EQUAL_FLOAT_MESSAGE(valExpected, valActual, msg.c_str());
    }
  }
}

void test_float_norange_difference(void)
{
  float valInput, valInputOld, valActual, valExpected, valExpectedOld;
  setup_float_norange_difference();
  for (byte i = 0; i < MEASURES; i++)
  {
    bool flInit = false;
    valExpectedOld = 0;
    for (byte j = 0; j < SAMPLES; j++)
    {
      valInput = SAMPLE_LIST[j] * (i + 1);
      if (flInit && fabs(valInput - valInputOld) > (DIFFERENCE * (i + 1)))
      {
        valExpected = valExpectedOld;
      }
      else if (flInit)
      {
        valInputOld = valInput;
        valExpectedOld += EXPONENTIAL_FACTOR * (valInput - valExpectedOld);
        valExpected = valExpectedOld;
      }
      else
      {
        valExpected = valInputOld = valInput;
        valExpectedOld = valExpected;
        flInit = true;
      }
      // Testee
      smoothFloat.setValue(valInput, i);
      valActual = smoothFloat.getValue(i);
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
    valExpectedOld = 0;
    for (byte j = 0; j < SAMPLES; j++)
    {
      valInput = SAMPLE_LIST[j] * (i + 1);
      // Algorithm
      if (valInput < (MINIMUM * (i + 1)) || valInput > (MAXIMUM * (i + 1)))
      {
        valExpected = valExpectedOld;
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
      smoothFloat.setValue(valInput, i);
      valActual = smoothFloat.getValue(i);
      String msg = "i=" + String(i) + ", j=" + String(j);
      TEST_ASSERT_EQUAL_FLOAT_MESSAGE(valExpected, valActual, msg.c_str());
    }
  }
}

void test_float_range_difference(void)
{
  float valInput, valInputOld, valActual, valExpected, valExpectedOld;
  setup_float_range_difference();
  for (byte i = 0; i < MEASURES; i++)
  {
    bool flInit = false;
    valExpectedOld = 0;
    for (byte j = 0; j < SAMPLES; j++)
    {
      valInput = SAMPLE_LIST[j] * (i + 1);
      // Algorithm
      if (valInput < (MINIMUM * (i + 1)) || valInput > (MAXIMUM * (i + 1)) ||
          (flInit && fabs(valInput - valInputOld) > (DIFFERENCE * (i + 1))))
      {
        valExpected = valExpectedOld;
      }
      else if (flInit)
      {
        valInputOld = valInput;
        valExpectedOld += EXPONENTIAL_FACTOR * (valInput - valExpectedOld);
        valExpected = valExpectedOld;
      }
      else
      {
        valExpected = valInputOld = valInput;
        valExpectedOld = valExpected;
        flInit = true;
      }
      // Testee
      smoothFloat.setValue(valInput, i);
      valActual = smoothFloat.getValue(i);
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
    valActual = smoothUint.getMeasurePtr(i)->getFactor();
    String msg = "i=" + String(i);
    TEST_ASSERT_EQUAL_UINT_MESSAGE(valExpected, valActual, msg.c_str());
  }
}

void test_uint_norange(void)
{
  unsigned int valInput, valActual, valExpected;
  float valExpectedOld;
  setup_uint_norange();
  for (byte i = 0; i < MEASURES; i++)
  {
    for (byte j = 0; j < SAMPLES; j++)
    {
      valInput = SAMPLE_LIST[j] * (i + 1);
      // Algorithm
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
      smoothUint.setValue(valInput, i);
      valActual = smoothUint.getValue(i);
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
  RUN_TEST(test_float_norange_difference);
  RUN_TEST(test_float_range);
  RUN_TEST(test_float_range_difference);
  //
  RUN_TEST(test_factor_uint);
  RUN_TEST(test_uint_norange);

  UNITY_END();
}

void loop() {}