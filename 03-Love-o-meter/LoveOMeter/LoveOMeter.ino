#include <assert.h>

const int SENSOR_PIN = A0;
const float BASE_TEMP = 20;

// using PWM outputs
const int OUTPUTS[] = {3, 5, 6};
const size_t OUTPUTS_COUNT = sizeof(OUTPUTS) / sizeof(int);

const int TEMPERATURES[] = {24, 27, 30};
const size_t TEMPERATURE_COUNT = sizeof(TEMPERATURES) / sizeof(int);

const int PWM_MIN = 0;
const int PWM_MAX = 255;

const bool DEBUG = false;

const size_t READ_VALUES_COUNT = 25;
float readValues = BASE_TEMP;
size_t readValuesCount = 0;

// ***************************************************************************
// Description : 
//   Setup function
// ***************************************************************************
void setup() {
  Serial.begin(9600);

  for (size_t i = 0; i < OUTPUTS_COUNT; i++)
  {
    analogWrite(OUTPUTS[i], LOW);
  }
}

// ***************************************************************************
// Description : 
//   Computes the intensity of a LED, returning a PWM value (0, 255).
//
// Parameters :
//   value      Value to map
//   min        Min value
//   min        Max value
//   
// Precondition :
//   min < max
//
// Returns :
//   0 if value is < min or > max
//   225 if value is in the middle of (min, max).
//   a map value if else.
// ***************************************************************************
int ComputeIntensity(float value, float min, float max)
{
  assert(min < max);
  
  if (value < min)
  {
    return PWM_MIN;
  }
  else if (value <= max)
  {
    const float middle = (max + min) / 2;
    return PWM_MAX - abs(value - middle) / (max - middle) * PWM_MAX;
  } else
  {
    return PWM_MIN;
  }
}

// ***************************************************************************
// Description : 
//   Main loop
// ***************************************************************************
void loop() {
  const int sensorVal = analogRead(SENSOR_PIN);
  const float voltage = (sensorVal / 1024.0) * 5;
  float temperature = (voltage - 0.5) * 100;

  readValues += temperature;
  readValuesCount++;

  if (false)
  {
    Serial.print("Temperature value : ");
    Serial.println(temperature);
  }

  if ((readValuesCount % READ_VALUES_COUNT) == 0)
  {
    // + 1 because of the initial value;
    temperature = readValues / (READ_VALUES_COUNT + 1);
    readValues = temperature;
    if (DEBUG)
    {
      Serial.print("Average temp : ");
      Serial.println(temperature);
    }

    for (size_t i = 0; i < OUTPUTS_COUNT; i++)
    {
      float range = 0;
      if (i < (OUTPUTS_COUNT - 1))
      {
        range = (TEMPERATURES[i + 1] - TEMPERATURES[i]);
      }
      else
      {
        range = (TEMPERATURES[i] - TEMPERATURES[i - 1]);
      }
      range = range * 3.0 / 4;

      const int writeValue = ComputeIntensity(temperature, TEMPERATURES[i] - range, TEMPERATURES[i] + range);
      analogWrite(OUTPUTS[i], writeValue);
      if (DEBUG)
      {
        Serial.print(writeValue / 255.0 * 100);
        Serial.print("%  ");
      }
    }
    if (DEBUG)
    {
      Serial.println();
    }
  }

  delay(10);
}

