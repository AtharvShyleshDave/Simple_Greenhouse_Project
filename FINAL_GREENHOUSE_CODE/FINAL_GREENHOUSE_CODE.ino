// AUTHOR - Atharv Shylesh Dave

/*

                                                  ""CODE FOR RUNNING THE EQUIPMENT FOR THE GREENHOUSE"":

> Done using Arduino Uno and 4 relay module 

> Relay output pin 1 is connected to pin 2 of Arduino
> Relay output pin 2 is connected to pin 3 of Arduino
> Relay output pin 3 is connected to pin 4 of Arduino
> Relay output pin 4 is connected to pin 5 of Arduino

> Relay input pin 1 = "INLET EXHAUST"
> Relay input pin 2 = "OUTLET EXHAUST"
> Relay input pins 3,4 = "MIST NOZZLES"

*/



/*
                  "STEP ONE: IDENTIFYING AND RECORDING TEMPERATURES FROM TWO SENSORS." 
*/

#include <OneWire.h>
#include <DallasTemperature.h>

// Define pins for temperature sensors
#define ONE_WIRE_BUS_T1 8
#define ONE_WIRE_BUS_T2 9

OneWire oneWireT1(ONE_WIRE_BUS_T1);
OneWire oneWireT2(ONE_WIRE_BUS_T2);

DallasTemperature sensorT1(&oneWireT1);
DallasTemperature sensorT2(&oneWireT2);

float TemperatureFromSensor1 = 0;
float TemperatureFromSensor2 = 0;


/* 
                  "STEP TWO: INITIALISING THE RELAY MODULE."
*/

int relayPins[] = {2, 3, 4, 5}; // 0=OUTLET, 1=INLET, 2 & 3=MIST


/*
                  "STEP THREE: DEFINING THE DELAY TIMES FOR OUTLET EXHAUST AND MIST SPRAY"
*/

int Second = 1000;
int Minute = 60 * Second;
int OUTLETexhaustDelay = 5 * Minute;
int MistSprayDelay = 5 * Second;


/*
                  "STEP FOUR: SETTING UP THE OUTPUTS."
*/

void setup() {
  // Initialize relays
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Start all OFF (active LOW)
  }

  // Start temperature sensors
  sensorT1.begin();
  sensorT2.begin();
}


/*
                  "STEP FIVE: READING THE TEMPERATURE."
*/

void getTemperatures() {
  sensorT1.requestTemperatures();
  sensorT2.requestTemperatures();

  TemperatureFromSensor1 = sensorT1.getTempCByIndex(0);
  TemperatureFromSensor2 = sensorT2.getTempCByIndex(0);
}


/*
                 "STEP SIX: DEFINING THE CONDITIONs FOR ALL DEVICES"
*/

void OutletExhaustON() {
  digitalWrite(relayPins[0], LOW);
}

void OutletExhaustOFF() {
  digitalWrite(relayPins[0], HIGH);
}

void InletExhaustON() {
  digitalWrite(relayPins[1], LOW);
}

void InletExhaustOFF() {
  digitalWrite(relayPins[1], HIGH);
}

void MistSpraysON() {
  digitalWrite(relayPins[2], LOW);
  digitalWrite(relayPins[3], LOW);
}

void MistSpraysOFF() {
  digitalWrite(relayPins[2], HIGH);
  digitalWrite(relayPins[3], HIGH);
}


/*
                 "STEP SEVEN: APPLYING THE ABOVE CONDITIONS ACCORDING TO THE SPECIFIED REQUIREMENT"
*/

void loop() {
  getTemperatures();

  float AverageTemperature = (TemperatureFromSensor1 + TemperatureFromSensor2) / 2;

  if (AverageTemperature > 28) {
    OutletExhaustON();
    InletExhaustON();

    unsigned long exhaustStart = millis();

    while ((millis() - exhaustStart) < OUTLETexhaustDelay) {
      // Toggle mist sprays ON and OFF every 5 seconds
      MistSpraysON();
      delay(MistSprayDelay);
      MistSpraysOFF();
      delay(MistSprayDelay);

      // Update temperatures to check if it dropped
      getTemperatures();
      AverageTemperature = (TemperatureFromSensor1 + TemperatureFromSensor2) / 2;
      if (AverageTemperature <= 24) {
        break;
      }
    }

    OutletExhaustOFF(); // Turn off outlet exhaust after delay
    // Keep inlet exhaust and mist sprays running until temp drops
    while (AverageTemperature > 24) {
      MistSpraysON();
      delay(MistSprayDelay);
      MistSpraysOFF();
      delay(MistSprayDelay);

      getTemperatures();
      AverageTemperature = (TemperatureFromSensor1 + TemperatureFromSensor2) / 2;
    }

    InletExhaustOFF();
    MistSpraysOFF();
  }
  else if (AverageTemperature <= 24) {
    OutletExhaustOFF();
    InletExhaustOFF();
    MistSpraysOFF();
  }

  delay(1000); // Check temperature every second
}
