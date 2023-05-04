#include <SPI.h>
#include <LoRa.h>
#include <SD.h>
#include <ctype.h>

const int NUM_DATA = 7; // 7 pieces of data

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Initializing SD card...");
  // checks if the card is present and can be initialized:
  if (!SD.begin(4)) {
    Serial.println("Card failed, or not present!");
    while (1); // halts the program
  }
  Serial.println("Card initialized!");

  Serial.println("Initialising LoRa...");
  // checks if LoRa starts successfully
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1); // halts the program
  }
  // LoRa.setSpreadingFactor(9);
  Serial.println("LoRa started successfully!");
}

bool verify_data(String data) {
  /* Verifies whether or not the data is a valid double */

  bool point_found = false;
  bool sign_found = false;

  for (auto c : data) {
    // ensures that a maximum of 1 decimal point is found per data (multiple points is invalid)
    if (c == '.') {
      if (point_found == false) { point_found = true; }
      else { return false; }
    }
    // ensures that a maximum of 1 '-' sign is found per data (multiple negative signs is invalid)
    else if (c == '-') {
      if (sign_found == false) { sign_found = true; }
      else {
        return false;
      }
    }
    // returns false if the data contained any character that isn't a digit (except '.' and '-')
    else if (!isDigit(c)) {
      return false;
    }
  }
  // if none of the criteria above are triggered, then the data is a valid double
  return true;
}

bool verify_packet(String packet_content, char delimiter) {
  /* Verifies the integrity of the packet (checks for missing data or corruption) */
  int num_spacers = 0;
  int delimiter_index = 0;

  String packet_copy = packet_content;

  while (packet_copy.length() > 0) {
    // checks if a data spacer is found
    delimiter_index = packet_copy.indexOf(delimiter);
    // stops the search if no delimiter characters were found
    if (delimiter_index == -1) {
      break;
    }
    ++num_spacers;

    // if the data is not a valid double, then the packet is corrupted (invalid)
    if (!verify_data(packet_copy.substring(0, delimiter_index))) { return false; }
    // cuts the string so it can be iterated through indexOf
    packet_copy = packet_copy.substring(delimiter_index + 1);
  }
  Serial.print("Number of spacers: ");
  Serial.println(num_spacers);

  // packet is considered valid if there are the correct amount of spacers (delimiter)
  if (num_spacers == NUM_DATA - 1) { return true; }
  // if incorrect number of spacers found, then some data is missing
  return false;
}

void log_data(String packet_content) {
  /* Writes received data to a CSV file in the SD card */

  File csv_file = SD.open("data.csv", FILE_WRITE);

  if (csv_file) {
    // appends the data to the file
    csv_file.println(packet_content);
    csv_file.close();
    Serial.println("Data written to file!");
  }
  else {
    Serial.println("Failed to open data.csv!");
  }
}

void loop() {
  // try to parse packet
  int packet_size = LoRa.parsePacket();
  String data_row = ""; // initialises the data row to be empty
  
  if (packet_size) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      // combines all characters in packet to a string
      data_row = data_row + ((char) LoRa.read());
    }
    Serial.print(data_row);

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    if (!verify_packet(data_row, ' ')) {
      Serial.println("Data packet corrupted!");
    }
    else {
      Serial.println("Data read successfully!");
      // converts the spaces to commas (for csv file)
      data_row.replace(" ", ",");
      Serial.println(data_row);

      // saves data to a CSV file
      log_data(data_row);
    }
  }
}
