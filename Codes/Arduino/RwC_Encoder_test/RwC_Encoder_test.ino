/*
  Test with original Adafruit I2C Lib, 10-30 PPR being captured (Not Precise)
*/
#include "Adafruit_seesaw.h"
#include <Wire.h>

#define SEESAW_ADDR 0x1
#define PULSES_PER_ROTATION 600  // Pulses per rotation

Adafruit_seesaw ss = Adafruit_seesaw(&Wire1);

int32_t last_enc_deltas[4] = { 0, 0, 0, 0 };
unsigned long last_time = 0;

void setup() {
  Wire1.setSDA(6);  // Set GPIO6 as SDA
  Wire1.setSCL(7);  // Set GPIO7 as SCL
  Wire1.begin();

  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Looking for seesaw!");// M-M, M-M

  if (!ss.begin(SEESAW_ADDR)) {
    Serial.println("Couldn't find seesaw on default address");
    while (1) delay(10);
  }
  Serial.println("seesaw started");
  uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
  if (version != 5752) {
    Serial.print("Wrong firmware loaded? ");
    Serial.println(version);
    while (1) delay(10);
  }
  Serial.println("Found Product 5752");

  // Initialize encoder interrupt
  for (int e = 0; e < 4; e++) {
    ss.enableEncoderInterrupt(e);
  }

  Serial.println("Turning on interrupts");
}

void loop() {
  unsigned long current_time = millis();
  unsigned long time_diff = current_time - last_time;
  
  // Only calculate speed once every 100ms or so
  /*
  if (time_diff >= 100) {
    // For storing RPM values of all encoders
    float rpm[4] = {0, 0, 0, 0};

    for (int e = 0; e < 4; e++) {
      //int32_t delta = ss.getEncoderDelta(e);  // Get the delta (change) since the last read
    int32_t Position = ss.getEncoderPosition(e);
      //delta = abs(delta);  // Take the absolute value of the delta

      // Calculate the speed in terms of encoder pulses per second
      //float speed_pulses_per_second = (float)delta / (time_diff / 1000.0);  // Pulses per second
    //float speed_pulses_per_second = (float)delta;
      // Convert pulses per second to RPM
      //rpm[e] = (speed_pulses_per_second * 60.0) / PULSES_PER_ROTATION;
      rpm[e] = (float) Position;
    }

    // Output RPM data for Serial Plotter
    // Format: "RPM1,RPM2,RPM3,RPM4"
    Serial.print(rpm[0]); Serial.print(",");
    Serial.print(rpm[1]); Serial.print(",");
    Serial.print(rpm[2]); Serial.print(",");
    Serial.println(rpm[3]); // End with a new line

    // Update the time reference for the next loop
    last_time = current_time;
  }

  */
    // For storing RPM values of all encoders
    float rpm[4] = {0, 0, 0, 0};

    for (int e = 0; e < 4; e++) {
      //int32_t delta = ss.getEncoderDelta(e);  // Get the delta (change) since the last read
    int32_t Position = ss.getEncoderPosition(e);
      //delta = abs(delta);  // Take the absolute value of the delta

      // Calculate the speed in terms of encoder pulses per second
      //float speed_pulses_per_second = (float)delta / (time_diff / 1000.0);  // Pulses per second
    //float speed_pulses_per_second = (float)delta;
      // Convert pulses per second to RPM
      //rpm[e] = (speed_pulses_per_second * 60.0) / PULSES_PER_ROTATION;
      rpm[e] = (float) Position;
    }

    // Output RPM data for Serial Plotter
    // Format: "RPM1,RPM2,RPM3,RPM4"
    Serial.print(rpm[0]); Serial.print(",");
    Serial.print(rpm[1]); Serial.print(",");
    Serial.print(rpm[2]); Serial.print(",");
    Serial.println(rpm[3]); // End with a new line

  // Don't overwhelm serial port
  delay(10);
}

