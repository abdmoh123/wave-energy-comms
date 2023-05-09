#include <ArduinoLowPower.h>
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const double MAX_EMF_VOLTAGE = 20.0;
const double EMF_MAX_FREQUENCY = 15.0;
const double SAMPLE_PERIOD = 1.0 / (4.0 * EMF_MAX_FREQUENCY); // x2 satisfies nyquist

double time = 0.0; // for simulating emf voltage
int sample_counter = 0;

Adafruit_MPU6050 mpu;
Adafruit_Sensor *acc_sensor, *gyro_sensor, *temp_sensor;
sensors_event_t accelerometer, gyroscope, thermometer;
// variables to cancel out any errors in measurements
double x_rot_error = 0.0;
double y_rot_error = 0.0;
double z_rot_error = 0.0;
double x_acc_error = 0.0;
double y_acc_error = 0.0;
double z_acc_error = 0.0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

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
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // set accelerometer range to +-8G
  mpu.setGyroRange(MPU6050_RANGE_500_DEG); // set gyro range to +- 500 deg/s
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ); // set low pass filter bandwidth to 5 Hz (improves stability)

  // gets data from the sensor for calibration
  acc_sensor = mpu.getAccelerometerSensor();
  gyro_sensor = mpu.getGyroSensor();
  acc_sensor->getEvent(&accelerometer);
  gyro_sensor->getEvent(&gyroscope);
  // sets the error values to calibrate the sensors
  // x_acc_error = 0; // ignores influence of gravity
  // y_acc_error = -0.06;
  // z_acc_error = -0.38;
  x_rot_error = gyroscope.gyro.x;
  y_rot_error = gyroscope.gyro.y;
  z_rot_error = gyroscope.gyro.z;
}

double gen_voltage(double time) {
  /* Generates a simulated value for emf voltage generated by generator */

  // generates a value between 15 and 7.5 Hz (varying frequency)
  double frequency_value = EMF_MAX_FREQUENCY * ((sin(PI * time) / 4) + 0.75); // frequency changes every 2 cycles
  double voltage = MAX_EMF_VOLTAGE * sin(frequency_value * 2 * PI * time);

  // prints voltage data (for plotting)
  Serial.print("emf_volt:");
  Serial.print(voltage);
  Serial.print(",");
  Serial.print("t:");
  Serial.println(time);

  return voltage;
}

void loop() {
  int before_sensors = millis();

  // generates and prints simulated emf voltage
  double emf_volt = gen_voltage(time);
    
  // get new sensor events with the readings
  acc_sensor->getEvent(&accelerometer);
  gyro_sensor->getEvent(&gyroscope);
  temp_sensor->getEvent(&thermometer);
  
  // gets x y z acceleration
  double x_acc = accelerometer.acceleration.x;
  double y_acc = accelerometer.acceleration.y;
  double z_acc = accelerometer.acceleration.z;
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
  double x_rot = gyroscope.gyro.x - x_rot_error;
  double y_rot = gyroscope.gyro.y - y_rot_error;
  double z_rot = gyroscope.gyro.z - z_rot_error;
  // prints out the values
  Serial.print("x_rot:");
  Serial.print(x_rot);
  Serial.print(",");
  Serial.print("y_rot:");
  Serial.print(y_rot);
  Serial.print(",");
  Serial.print("z_rot:");
  Serial.println(z_rot);

  int after_sensors = millis();

  Serial.println("\nSending packet...");
  // sends packet with data
  LoRa.beginPacket();
  LoRa.print(time);
  LoRa.print(" ");
  LoRa.print(emf_volt);
  LoRa.print(" "); // space is used to separate each data value
  LoRa.print(x_acc);
  LoRa.print(" ");
  LoRa.print(x_rot);
  LoRa.print(" ");
  LoRa.print(y_acc);
  LoRa.print(" ");
  LoRa.print(y_rot);
  LoRa.print(" ");
  LoRa.print(z_acc);
  LoRa.print(" ");
  LoRa.print(z_rot);
  LoRa.endPacket();
  Serial.println("Packet sent!\n");

  int after_lora = millis();
  double sensor_time = (double) (after_sensors - before_sensors) / 1000.0;
  double transmit_time = (double) (after_lora - after_sensors) / 1000.0;
  double time_taken = SAMPLE_PERIOD + transmit_time + sensor_time;

  // advances time and counter
  time += time_taken;
  ++sample_counter;

  Serial.println(sample_counter * time_taken);

  // resets the timer every 24 hours
  if (time >= 86400.0) {
    time -= 86400.0;
  }
  // sleeps the arduino after 5 seconds
  if ((sample_counter * time_taken) >= 5.0) {
    // should be around x samples
    Serial.print("Transmitted ");
    Serial.print(sample_counter);
    Serial.println(" samples, sleeping...");
    double sleep_time = 10.0; // 1s
    LowPower.deepSleep((int) sleep_time * 1000); // measured in milliseconds
    Serial.println("Waking up...");
    // resets sample counter
    sample_counter = 0;
    time += sleep_time;
  }

  // delays/waits for 1/15s (Sample period * 1000 due to s -> ms conversion)
  delay(SAMPLE_PERIOD * 1000);
}
