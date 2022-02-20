/*
  NAME:
  Unit tests of application library "gbj_appsmoothing" for smoothing
  with the generic library "gbj_running".

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
#include "gbj_running.h"
#include <Arduino.h>
#include <unity.h>

//******************************************************************************
// Preparation
//******************************************************************************
const float MINIMUM = 300.0;
const float MAXIMUM = 800.0;
const byte SAMPLES_LIB = 5;
const byte MEASURES = 2;

const float SAMPLE_LIST[] = { 423, 753, 217, 42, 898, 712, 728,
                              510, 835, 77,  2,  956, 623, 319 };
const byte SAMPLES = sizeof(SAMPLE_LIST) / sizeof(SAMPLE_LIST[0]);

gbj_appsmooth<gbj_running, float> smoothFloat =
  gbj_appsmooth<gbj_running, float>();
gbj_appsmooth<gbj_running, int> smoothUint = gbj_appsmooth<gbj_running, int>();

void setup_float_norange()
{
  smoothFloat.begin(MEASURES);
}

void setup_float_range()
{
  smoothFloat.begin(MEASURES);
  for (byte i = 0; i < smoothFloat.getMeasures(); i++)
  {
    smoothFloat.getMeasurePtr(i)->setAverage();
    smoothFloat.setMinimum(MINIMUM * (i + 1), i);
    smoothFloat.setMaximum(MAXIMUM * (i + 1), i);
  }
}

void setup_uint_norange()
{
  smoothUint.begin(MEASURES);
  for (byte i = 0; i < smoothFloat.getMeasures(); i++)
  {
    smoothUint.getMeasurePtr(i)->setAverage();
  }
}

//******************************************************************************
// Tests
//******************************************************************************
void test_measures(void)
{
  byte valExpected, valActual;
  valExpected = SAMPLES_LIB;
  valActual = gbj_running::SAMPLES;
  TEST_ASSERT_EQUAL_UINT8(valExpected, valActual);
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
  float valActual, valExpected;
  setup_float_norange();
  for (byte i = 0; i < MEASURES; i++)
  {
    for (byte j = 0; j < SAMPLES; j++)
    {
      // Algorithm
      int kStart = max(0, j + 1 - SAMPLES_LIB);
      valExpected = 0;
      for (byte k = kStart; k <= j; k++)
      {
        valExpected += SAMPLE_LIST[k] * (i + 1);
      }
      valExpected /= j + 1 - kStart;
      // Testee
      smoothFloat.setValue(SAMPLE_LIST[j] * (i + 1), i);
      valActual = smoothFloat.getValue(i);
      String msg = "i=" + String(i) + ", j=" + String(j);
      TEST_ASSERT_EQUAL_FLOAT_MESSAGE(valExpected, valActual, msg.c_str());
    }
  }
}

void test_float_range(void)
{
  float valInput, valActual, valExpected;
  byte items;
  setup_float_range();
  for (byte i = 0; i < MEASURES; i++)
  {
    for (byte j = 0; j < SAMPLES; j++)
    {
      // Algorithm
      int k = j;
      items = 0;
      valExpected = 0;
      do
      {
        valInput = SAMPLE_LIST[k] * (i + 1);
        if (valInput >= (MINIMUM * (i + 1)) && valInput <= (MAXIMUM * (i + 1)))
        {
          valExpected += valInput;
          items++;
        }
      } while (items < SAMPLES_LIB && --k >= 0);
      valExpected /= items;
      // Testee
      valInput = SAMPLE_LIST[j] * (i + 1);
      smoothFloat.setValue(valInput, i);
      valActual = smoothFloat.getValue(i);
      String msg = "i=" + String(i) + ", j=" + String(j);
      TEST_ASSERT_EQUAL_FLOAT_MESSAGE(valExpected, valActual, msg.c_str());
    }
  }
}

void test_uint_norange(void)
{
  unsigned int valActual, valExpected;
  setup_uint_norange();
  for (byte i = 0; i < MEASURES; i++)
  {
    for (byte j = 0; j < SAMPLES; j++)
    {
      // Algorithm
      int kStart = max(0, j + 1 - SAMPLES_LIB);
      valExpected = 0;
      for (byte k = kStart; k <= j; k++)
      {
        valExpected += SAMPLE_LIST[k] * (i + 1);
      }
      valExpected /= j + 1 - kStart;
      // Testee
      smoothUint.setValue(SAMPLE_LIST[j] * (i + 1), i);
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

  RUN_TEST(test_measures);
  RUN_TEST(test_minimum_float);
  RUN_TEST(test_maximum_float);
  RUN_TEST(test_float_norange);
  RUN_TEST(test_float_range);
  //
  RUN_TEST(test_uint_norange);

  UNITY_END();
}

void loop() {}