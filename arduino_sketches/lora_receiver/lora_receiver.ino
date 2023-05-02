#include <SPI.h>
#include <LoRa.h>
#include <SD.h>

String data_row = "";
const int NUM_DATA = 8; // 8 pieces of data
bool dispose_packet = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Initialising LoRa...");
  // checks if LoRa starts successfully
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1); // halts the program
  }
  LoRa.setSpreadingFactor(9);
  Serial.println("LoRa started successfully!");

  Serial.println("Initializing SD card...");
  // checks if the card is present and can be initialized:
  if (!SD.begin(4)) {
    Serial.println("Card failed, or not present!");
    while (1); // halts the program
  }
  Serial.println("Card initialized!");

}

double * separate_data(String input_string, char delimiter) {
  /* Converts a data packet string into an array of data */

  double temp_data_array[NUM_DATA];

  for (int i = 0; i < NUM_DATA; ++i) {
    // if the packet was corrupted, the number of spaces might be lower than expected
    if (input_string.length() == 0) {
      // packet is disposed if it is corrupted
      dispose_packet = true;
      break;
    }

    // separates data in packet using the delimiter ' '.
    int index = input_string.indexOf(delimiter);
    // converts string to double. If conversion fails (due to corruption), it could = 0.
    temp_data_array[i] = input_string.substring(0, index).toDouble();

    // read data is removed from string
    input_string = input_string.substring(index + 1);
  }

  return temp_data_array;
}

void log_data(double * input_data_array) {
  /* Writes received data to a CSV file in the SD card */

  File csv_file = SD.open("data.csv", FILE_WRITE);

  if (csv_file) {
    csv_file.print(input_data_array[0]);
    for (int i = 1; i < NUM_DATA; ++i) {
      csv_file.print(',');
      csv_file.print(input_data_array[i]);
    }
    csv_file.println(""); // adds a new line to the file
  }
  else {
    Serial.println("Failed to open data.csv!");
  }
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      data_row.concat((char) LoRa.read());
    }
    Serial.print(data_row);

    // seperates data in packet and puts it in an array
    double * data_array = {};
    data_array = separate_data(data_row, ' ');
    // resets data row
    data_row = "";

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    if (dispose_packet) {
      Serial.println("Data packet corrupted!");
      dispose_packet = false;
    }
    else {
      Serial.println("Data read successfully!");
      // prints contents of the array
      Serial.print("[");
      for (int i = 0; i < NUM_DATA; ++i) {
        Serial.print(data_array[i]);
        Serial.print(" ");
      }
      Serial.println("]");

      // saves data to a CSV file
      log_data(data_array);
    }
  }
}
