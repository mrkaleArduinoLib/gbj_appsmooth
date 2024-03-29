<a id="library"></a>

# gbj\_appsmooth
This is an application library, which is used usually as a project library for particular PlatformIO project. It unifies and encapsulates the functionality of `statistical smoothing` with valid data range filtering of subsequent samples with help of various generic smoothing libraries, e.g., _gbj_exponential_, _gbj_running_, etc. The encapsulation provides following advantages:

* Functionality is hidden from the main sketch.
* The library follows the principle `separation of concerns`.
* Update in library is valid for all involved projects.
* The library manages the smoothing and provides the same interface for various smoothing algorithms.


## Fundamental functionality
* The library is templated, so that it can utilize various smoothing libraries for samples of various data types.
* It provides filtering of samples against their valid range by setting minimal and maximal valid values.
* It provides filtering of samples against their absolute change by setting valid aboslute maximal change of values.


<a id="dependency"></a>

## Dependency
#### Arduino platform
* **Arduino.h**: Main include file for the Arduino SDK.
* **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.

#### Espressif ESP8266 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Espressif ESP32 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Particle platform
* **Particle.h**: Includes alternative (C++) data type definitions.


<a id="tests"></a>

## Unit testing
The subfolder `tests` in the folder `extras`, i.e., `gbj_appsmooth/extras/tests`, contains testing files, usually one for each generic smoothing library, with unit tests of library [gbj_appsmooth](#library) executable by [Unity](http://www.throwtheswitch.org/unity) test runner. Each testing file should be placed in an individual test folder of a particular project with mandatory prefix `test_`, but in arbitrary place in an folder hierarchy. The good practice is to create a separate folder for this library and separate test subfolders with the same name as the test files have, usually in the structure `test/gbj_appsmooth/test_<testname>/<testfile>`.
* **test_exponential.cpp**: Test file providing test cases for statistical smoothing by exponential filtering.
* **test_running.cpp**: Test file providing test cases for statistical smoothing by running statistics.


<a id="interface"></a>

## Interface
* [gbj_appsmooth()](#gbj_appsmooth)
* [begin()](#begin)

### Setters
* [setMinimum()](#setRange)
* [setMaximum()](#setRange)
* [setDifference()](#setRange)
* [resetMinimum()](#resetRange)
* [resetMaximum()](#resetRange)
* [resetMaximum()](#resetRange)
* [resetDifference()](#resetRange)
* [setValue()](#setValue)

### Getters
* [getValue()](#getValue)
* [getInput()](#getInput)
* [getMinimum()](#getRange)
* [getMaximum()](#getRange)
* [getDifference()](#getDifference)
* [getMeasures()](#getMeasures)
* [getMeasurePtr()](#getMeasurePtr)
* [isValid()](#isValid)
* [isInvalid()](#isValid)


<a id="gbj_appsmooth"></a>

## gbj_appsmooth()

#### Description
Constructor creates the class instance object and is overloaded with various combinations of valid range limits and maximal valid change applied implicitly for all smoothed measures.
* It is templated for particular generic smoothing library type and samples data types.
* If constructor has just one argument, that one is considered as the maximum of general valid range.
* If two arguments are defined for range limits, the constructor sorts them internally if needed.
* General limits can be overriden at individual measures by corresponding [setters](#setRange).
* If only valid maximal change is needed without valid range limits or just one range limit, the constructor should be created without any arguments and valid maximal change defined separately by corresponding [setter](#setDifference).

#### Syntax
    gbj_appsmooth<class SMT, typename DAT>()
    gbj_appsmooth<class SMT, typename DAT>(DAT valMax)
    gbj_appsmooth<class SMT, typename DAT>(DAT valMax, DAT valMin)
    gbj_appsmooth<class SMT, typename DAT>(DAT valMax, DAT valMin, DAT valDif)

#### Parameters
* **SMT**: Type of a generic smoothing library instance object.
  * *Valid values*: by include files
  * *Default value*: none


* **DAT**: Data type of samples of all smoothed measures.
  * *Valid values*: various
  * *Default value*: float


* **valMax**: Maximum of general valid range of samples of all smoothed measures.
  * *Valid values*: various
  * *Default value*: templated


* **valMin**: Minimum of general valid range of samples of all smoothed measures.
  * *Valid values*: various
  * *Default value*: templated


* **valMin**: Maximal absolute valid change between subsequent data items.
  * *Valid values*: various
  * *Default value*: templated

#### Returns
Object performing smoothing management.

#### Example
```cpp
#include "gbj_appsmooth.h"
#include "gbj_exponential.h"
#include "gbj_running.h"

gbj_appsmooth<gbj_exponential> smoothFloat = gbj_appsmooth<gbj_exponential>();
gbj_appsmooth<gbj_exponential, long> smoothLong = gbj_appsmooth<gbj_running, long>(3000);
gbj_appsmooth<gbj_exponential, int> smoothInt = gbj_appsmooth<gbj_running, int>(50, 10);
```
For just valid change
```cpp
#include "gbj_appsmooth.h"
#include "gbj_exponential.h"

gbj_appsmooth<gbj_exponential> smoothFloat = gbj_appsmooth<gbj_exponential>();

void setup()
{
  smoothFloat.begin();
  smoothFloat.setDifference(12.3);
}
```

[Back to interface](#interface)


<a id="begin"></a>

## begin()

#### Description
The initialization method of the instance object, which should be called in the setup section of a sketch.
* The method creates internal cache in form of array of individual measures' cache structures.
* One member of a measure structure is the internal instance object of generic smoothing library, so that its entire interface is available.

#### Syntax
    void begin(byte measures)

#### Parameters
* **measures**: Number of measures to be smoothed.
  * *Valid values*: 1 ~ 255
  * *Default value*: 1

#### Returns
None

[Back to interface](#interface)


<a id="setRange"></a>

## setMinimum(), setMaximum(), setDifference()

#### Description
The particular overloaded method sets the corresponding extreme of the valid range or valid change for sample values of particular measure or all of them.
* If there is no measure index provided, the method sets the extreme or change for all measures defined by the method [begin()](#begin).
* The maximal valid change is internally sanitized to absolute value of input value.

#### Syntax
    void setMinimum(DAT data, byte idx)
    void setMinimum(DAT data)
    void setMaximum(DAT data, byte idx)
    void setMaximum(DAT data)
    void setDifference(DAT data, byte idx)
    void setDifference(DAT data)

#### Parameters
* **data**: Valid extreme or change sample value applied to input value.
  * *Valid values*: by templated data type
  * *Default value*: none


* **idx**: Optional index or sequence order of a measure, for which the extreme of valid range or valid absolute maximal change is set. If it is not provided, the method is applied on all measures.
  * *Valid values*: 0 ~ (measure - 1) from [begin](#begin)
  * *Default value*: none

#### Returns
None

#### See also
[resetMinimum(), resetMaximum(), resetDifference()](#resetRange)

[begin()](#begin)

[Back to interface](#interface)


<a id="resetRange"></a>

## resetMinimum(), resetMaximum(), resetDifference()

#### Description
The particular overloaded method resets the corresponding extreme of the valid range or valid chande for sample values of particular measure or all of them.
* The methods does not change cached extreme and/or change values. They just set internal flag determining, that no check of input values should be applied against valid range or change on.
* After resetting the effective valid range or change is as after initialization by the method [begin()](#begin) and is determined just by data type of samples.

#### Syntax
    void resetMinimum(byte idx)
    void resetMinimum()
    void resetMaximum(byte idx)
    void resetMaximum()
    void resetDifference(byte idx)
    void resetDifference()

#### Parameters
* **idx**: Optional index or sequence order of a measure, for which the extreme or change is reset. If it is not provided, the method is applied on all measures.
  * *Valid values*: 0 ~ (measure - 1) from [begin](#begin)
  * *Default value*: none

#### Returns
None

#### See also
[setMinimum(), setMaximum(), setDifference()](#setRange)

[begin()](#begin)

[Back to interface](#interface)


<a id="setValue"></a>

## setValue()

#### Description
The method checks provided input sample value against valid range and/or valid change, which result returns, and calculates new smoothed value.
* The valid range of input sample values is defined explicitly either by corresponding [setter(s)](#setRange) or implicitly by data type of samples.
* The correct input value is neither less than defined minimum nor greater then defined maximum of the valid range, and at the same time its absolute difference from previous correct input value is not greater than defined maximal valid change.
* Only the correct input values are cached. Invalid input values are ignored whatsoever.
* The new smooth value, which is output value, is calculated only if input value is correct. If it is not, the output value stays intact as "last good value". It is available by corresponding [getter](#getValue).

#### Syntax
    bool setValue(DAT data, byte idx)

#### Parameters
* **data**: Input sample value to be filtered and smoothed.
  * *Valid values*: by templated data type and valid range
  * *Default value*: none


* **idx**: Index or sequence order of a measure, which value should be smoothed counting from zero.
  * *Valid values*: 0 ~ (measure - 1) from [begin](#begin)
  * *Default value*: 0

#### Returns
Flag determining result of checking the input value against the valid range and/or change.

#### See also
[getValue()](#getValue)

[begin()](#begin)

[Back to interface](#interface)


<a id="getValue"></a>

## getValue()

#### Description
The method returns recently calculated smoothed value.
* If the input value has been correct, it is a new smoothed value.
* If the input value has been incorrect, it is a recent smoothed value as a "last good value".
* If this getter is used before the corresponding [setter](#setValue) since initiation by method [begin()](#init), the implicit initial value of used generic smoothing library is returned, usualy `0`, as a default smoothed value.

#### Syntax
    DAT getValue(byte idx)

#### Parameters
* **idx**: Index or sequence order of a measure, which value has been smoothed counting from zero.
  * *Valid values*: 0 ~ (measure - 1) from [begin](#begin)
  * *Default value*: 0

#### Returns
New or recently smoothed value.

#### See also
[setValue()](#setValue)

[begin()](#begin)

[Back to interface](#interface)


<a id="getInput"></a>

## getInput()

#### Description
The method returns recently provided correct input value from the cache, which fits to potential valid range and change.

#### Syntax
    DAT getInput(byte idx)

#### Parameters
* **idx**: Index or sequence order of a measure, which correct input value should be returned counting from zero.
  * *Valid values*: 0 ~ (measure - 1) from [begin](#begin)
  * *Default value*: 0

#### Returns
Recent correct input value.

#### See also
[setValue()](#setValue)

[Back to interface](#interface)


<a id="isValid"></a>

## isValid(), isInvalid()

#### Description
The particular method returns flag determining corresponding result of checking the input value against the valid range and/or valid change.
* If there is no sample value extreme as well the valid change defined, the check is not provided and method always returns positive flag.
* The flag about invalidity signals that the smoothed value returned by [getValue()](#getValue) and input value returned by [getInput()](#getInput) are obsolete.
* The particular flag is useful to decide whether the smoothed value is worth or not, e.g., to be published or sent to an IoT platform.

#### Syntax
    bool isValid(byte idx)
    bool isInvalid(byte idx)

#### Parameters
* **idx**: Index or sequence order of a measure, which checking result should be returned counting from zero.
  * *Valid values*: 0 ~ (measure - 1) from [begin](#begin)
  * *Default value*: 0

#### Returns
Flag determining result of checking the input value against the valid range and/or change.

#### See also
[setMinimum(), setMaximum(), setDifference()](#setRange)

[Back to interface](#interface)


<a id="getRange"></a>

## getMinimum(), getMaximum(), getDifference()

#### Description
The particular method returns recently set extreme value for the valid range or absolute maximal valid change.
* It the extreme value or valid change has been reset meanwhile, the getter returns recent value, but it is not aplied on cached input values.

#### Syntax
    DAT getMinimum(byte idx)
    DAT getMaximum(byte idx)
    DAT getDifference(byte idx)

#### Parameters
* **idx**: Index or sequence order of a measure, which valid extreme or change should be returned counting from zero.
  * *Valid values*: 0 ~ (measure - 1) from [begin](#begin)
  * *Default value*: 0

#### Returns
Recent extreme value of the valid range or valid absolute maximal change.

#### See also
[setMinimum(), setMaximum(), setDifference()](#setRange)

[resetMinimum(), resetMaximum(), resetDifference()](#resetRange)

[Back to interface](#interface)


<a id="getMeasures"></a>

## getMeasures()

#### Description
The method returns number of smoothed measures as it has been defined at [initialization](#begin).

#### Syntax
    byte getMeasures()

#### Parameters
None

#### Returns
Number of smoothed measures.

#### See also
[begin()](#begin)

[Back to interface](#interface)


<a id="getMeasurePtr"></a>

## getMeasurePtr()

#### Description
The method returns the pointer to the internal instance object of generic smoothing library of particular measure. It allows to utilize entire interface of that library.

#### Syntax
     SMT *getMeasurePtr(byte idx)

#### Parameters
* **idx**: Index or sequence order of a measure, which smoothing library instance object pointer should be returned counting from zero.
  * *Valid values*: 0 ~ (measure - 1) from [begin](#begin)
  * *Default value*: 0

#### Returns
Pointer to a measure's instance object of the generic smoothing library.

#### Example
```cpp
gbj_appsmooth<gbj_exponential> smoothExp = gbj_appsmooth<gbj_exponential>();
gbj_appsmooth<gbj_running, unsigned int> smoothRun = gbj_appsmooth<gbj_running, unsigned int>();
void setup()
{
    smoothExp.begin(3);
    for (byte i = 0; i < smoothExp.getMeasures(); i++)
    {
        smoothExp.getMeasurePtr(i)->setFactor(0.3);
    }

    smoothRun.begin(2);
    for (byte i = 0; i < smoothRun.getMeasures(); i++)
    {
        smoothRun.getMeasurePtr(i)->setMedian();
    }
}
```
#### See also
[gbj_appsmooth()](#gbj_appsmooth)

[Back to interface](#interface)
