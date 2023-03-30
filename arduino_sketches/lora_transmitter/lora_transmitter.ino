#include <ArduinoLowPower.h>
#include <LoRa.h>

const double MAX_VOLTAGE = 15.0;
int counter = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

double gen_voltage(int counter) {
  return MAX_VOLTAGE * sin(counter);
}

double gen_accelerometer(int counter) {

}

void loop() {
  Serial.print("Sending packet: ");

  double voltage = gen_voltage(counter);
  Serial.println(voltage);

  // send packet
  LoRa.beginPacket();
  LoRa.print(voltage);
  LoRa.endPacket();

  counter++;

  delay(5000);
}