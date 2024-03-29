#include <ArduinoLowPower.h>
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const float MAX_EMF_VOLTAGE = 20.0;
const float EMF_MAX_FREQUENCY = 15.0;
// sample rate was chosen to be 40Hz to match the sensor cycle rate
const float SAMPLE_PERIOD = 1.0 / 40.0; // satisfies nyquist (40 > 2 x 15 = 30)

const float SAMPLING_TIME = 5.0; // time taken to sample data before transmitting and sleeping
const int SLEEP_TIME = 19192; // ms (should last around 3 months at full charge)

// buffers for storing sampled data before transmitting packet
float time_buffer[500];
float volt_buffer[500];
float x_acc_buffer[500];
float y_acc_buffer[500];
float z_acc_buffer[500];
float x_rot_buffer[500];
float y_rot_buffer[500];
float z_rot_buffer[500];

float time = 0.0; // for simulating emf voltage
int sample_counter = 0;

Adafruit_MPU6050 mpu;
Adafruit_Sensor *acc_sensor, *gyro_sensor;

// variables to cancel out any errors in measurements
float x_rot_error = 0.0;
float y_rot_error = 0.0;
float z_rot_error = 0.0;
float x_acc_error = 0.0;
float y_acc_error = 0.0;
float z_acc_error = 0.0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(LED_BUILTIN, OUTPUT);

  // halts program if lora failed to start
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1) { LowPower.deepSleep(3600000); } // deep sleeps (1h) to save power
  }
  Serial.println("LoRa successful!");
  LoRa.setSignalBandwidth(250E3);
  
  // tries to initialize mpu6050 sensor
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { LowPower.deepSleep(3600000); } // deep sleeps (1h) to save power
  }
  Serial.println("MPU6050 setup successful!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // set accelerometer range to +-8G
  mpu.setGyroRange(MPU6050_RANGE_500_DEG); // set gyro range to +- 500 deg/s
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ); // set low pass filter bandwidth to 5 Hz (improves stability)
  // enables cycle mode: sensors output measurement evey 1/40s then sleep in between these measurements to save power
  mpu.setCycleRate(MPU6050_CYCLE_40_HZ);
  mpu.enableSleep(false);
  mpu.enableCycle(true);

  sensors_event_t accelerometer, gyroscope;
  // gets data from the sensor for calibration
  acc_sensor = mpu.getAccelerometerSensor();
  gyro_sensor = mpu.getGyroSensor();
  acc_sensor->getEvent(&accelerometer);
  gyro_sensor->getEvent(&gyroscope);
  // sets the error values to calibrate the sensors (must be still for good calibration)
  x_rot_error = gyroscope.gyro.x;
  y_rot_error = gyroscope.gyro.y;
  z_rot_error = gyroscope.gyro.z;
}

float gen_voltage(float time) {
  /* Generates a simulated value for emf voltage generated by generator */

  // generates a value between 15 and 7.5 Hz (varying frequency)
  float frequency_value = EMF_MAX_FREQUENCY * ((sin(PI * time) / 4) + 0.75); // frequency changes every 2 cycles
  float voltage = MAX_EMF_VOLTAGE * sin(frequency_value * 2 * PI * time);

  // prints voltage data (for plotting)
  Serial.print("emf_volt:");
  Serial.print(voltage);
  Serial.print(",");
  Serial.print("t:");
  Serial.println(time);

  return voltage;
}

void loop() {
  // int before_sensors = millis();

  sensors_event_t accelerometer, gyroscope;

  // generates and prints simulated emf voltage
  // float emf_volt = gen_voltage(time);

  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.3V):
  float voltage = sensorValue * (3.3 / 1023.0) * 1000;
  float emf_volt = (map(sensorValue, 0, 1023, -20000, 20000)) / 1000;
  // print out the value you read:
  Serial.print("emf_volt:");
  Serial.print(emf_volt);
  Serial.print(",");
  Serial.print("t:");
  Serial.println(time);
    
  // get new sensor events with the readings
  acc_sensor->getEvent(&accelerometer);
  gyro_sensor->getEvent(&gyroscope);
  
  // gets x y z acceleration
  float x_acc = accelerometer.acceleration.x;
  float y_acc = accelerometer.acceleration.y;
  float z_acc = accelerometer.acceleration.z;
  // prints out the values
  Serial.print("x_acc:");
  Serial.print(x_acc);
  Serial.print(",");
  Serial.print("y_acc:");
  Serial.print(y_acc);
  Serial.print(",");
  Serial.print("z_acc:");
  Serial.println(z_acc);
  
  // gets x y z rotation
  float x_rot = gyroscope.gyro.x - x_rot_error;
  float y_rot = gyroscope.gyro.y - y_rot_error;
  float z_rot = gyroscope.gyro.z - z_rot_error;
  // prints out the values
  Serial.print("x_rot:");
  Serial.print(x_rot);
  Serial.print(",");
  Serial.print("y_rot:");
  Serial.print(y_rot);
  Serial.print(",");
  Serial.print("z_rot:");
  Serial.println(z_rot);

  // saves sampled data to buffer arrays (for transmitting later)
  time_buffer[sample_counter] = time;
  volt_buffer[sample_counter] = emf_volt;
  x_acc_buffer[sample_counter] = x_acc;
  y_acc_buffer[sample_counter] = y_acc;
  z_acc_buffer[sample_counter] = z_acc;
  x_rot_buffer[sample_counter] = x_rot;
  y_rot_buffer[sample_counter] = y_rot;
  z_rot_buffer[sample_counter] = z_rot;

  // int after_sensors = millis();

  // float sensor_time = (float) (after_sensors - before_sensors) / 1000.0;
  // float time_taken = sensor_time + SAMPLE_PERIOD;

  // advances time and sample_counter
  time += SAMPLE_PERIOD;
  ++sample_counter;

  // prints a stopwatch and sample counter (both reset at sleep)
  Serial.print("stopwatch:");
  Serial.print(sample_counter * SAMPLE_PERIOD);
  Serial.print(",");
  Serial.print("sample:");
  Serial.println(sample_counter);

  // resets the timer every 24 hours
  if (time >= 86400.0) {
    time -= 86400.0;
  }
  // starts transmitting then sleeps when done
  if ((sample_counter * SAMPLE_PERIOD) >= SAMPLING_TIME) {
    // turns on LED to indicate that arduino is transmitting data
    digitalWrite(LED_BUILTIN, HIGH);

    int before_lora = millis();
    // sends packets with all saved data
    Serial.println("\nSending packets...");
    for (int i = 0; i < sample_counter; ++i) {
      LoRa.beginPacket();
      LoRa.print(time_buffer[i]);
      LoRa.print(" "); // space is used to separate each data value
      LoRa.print(volt_buffer[i]);
      LoRa.print(" ");
      LoRa.print(x_acc_buffer[i]);
      LoRa.print(" ");
      LoRa.print(x_rot_buffer[i]);
      LoRa.print(" ");
      LoRa.print(y_acc_buffer[i]);
      LoRa.print(" ");
      LoRa.print(y_rot_buffer[i]);
      LoRa.print(" ");
      LoRa.print(z_acc_buffer[i]);
      LoRa.print(" ");
      LoRa.print(z_rot_buffer[i]);
      LoRa.endPacket();
    }
    Serial.println("Packets sent!\n");
    int after_lora = millis();

    // gets time taken to transmit data
    float transmit_time = (float) (after_lora - before_lora) / 1000.0;

    // should be around x samples
    Serial.print("Transmitted ");
    Serial.print(sample_counter);
    Serial.println(" samples, sleeping...");

    // turns off LED before sleeping
    digitalWrite(LED_BUILTIN, LOW);

    sleep_mode();
    Serial.println("Waking up...");
    // turns LED on for 0.5s then turns it off to indicate wakeup
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    // resets sample counter
    sample_counter = 0;
    // shifts time accordingly
    time += transmit_time + 0.5 + (float) SLEEP_TIME / 1000.0;
  }

  // delays/waits for 1/15s (Sample period * 1000 due to s -> ms conversion)
  delay(SAMPLE_PERIOD * 1000);
}

void sleep_mode() {
  /* Function responsible for sleeping all components of the device in order to save power */

  Serial.flush(); // waits for all serial data to be transmitted before continuing
  Serial.end(); // ends serial communication
  mpu.enableSleep(true); // sleeps the sensor
  LowPower.deepSleep(SLEEP_TIME); // measured in milliseconds
  mpu.enableSleep(false); // wakes up the sensor
  Serial.begin(9600); // re-enables serial commmunication (monitor + plot)
  while (!Serial); // waits for serial connection before continuing
}
