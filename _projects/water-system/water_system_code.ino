#include <AFMotor.h>
#include <Servo.h>

#define Start A3
#define Estop A2
#define LLpin 52

AF_DCMotor DCmotor(1);
AF_DCMotor cfpump(2);
AF_DCMotor pump(4);
Servo Servo1;

// Variables for system state
bool begin = HIGH;
bool stop = HIGH;
bool Level = LOW;
int servo_pos = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;
int seconds = 0;
bool systemActive = false;
unsigned long servoMillis = 0;
const int servoDelay = 20;
int servoDirection = 1;
unsigned long systemStartTime = 0;  // Store when the system started
bool servoStarted = false;          // Track if the servo has started

// Turbidity sensor variables
const int turbidityPin1 = A0;  // First turbidity sensor
const int turbidityPin2 = A1;  // Second turbidity sensor
float turbidityVoltage1 = 0.0;
float turbidityVoltage2 = 0.0;

// State machine variables
int systemState = 0;  // Tracks the current state
unsigned long stateStartTime = 0;  // Tracks when the current state started

void setup() {
  Serial.begin(9600);

  // Pin initialization
  pinMode(Estop, INPUT_PULLUP);
  pinMode(Start, INPUT_PULLUP);
  pinMode(LLpin, INPUT_PULLUP);

  // Motor and servo initialization
  Servo1.attach(9);
  DCmotor.setSpeed(255);
  DCmotor.run(RELEASE);
  cfpump.setSpeed(255);
  cfpump.run(RELEASE);
  pump.setSpeed(170);
  pump.run(RELEASE);

  Serial.println("System initialized. Waiting for Start button...");
  Servo1.write(0);  // Initialize servo position
}

void loop() {
  // Read button states
  begin = digitalRead(Start);
  stop = digitalRead(Estop);  
  Level = digitalRead(LLpin);

  if (begin == LOW && !systemActive) {
    // System startup
    systemActive = true;
    servoStarted = false;           // Reset the servo start flag
    systemStartTime = millis();     // Record the system start time
    stateStartTime = millis();      // Initialize state time tracking
    systemState = 0;                // Start from the first state
    Serial.println("System started!");
  } 
  else if (stop == LOW || Level == LOW) {
    // Emergency stop
    emergencyStop();
  }

  if (systemActive) {
    // Handle the system state
    runStateMachine();

    // Update turbidity sensor readings
    updateTurbiditySensors();

    // Log sensor data every second
    logSensorData();

    // Update servo movement
    if (!servoStarted && millis() - systemStartTime >= 4000) {
      servoStarted = true;         // Mark servo as started
    }
    if (servoStarted) {
      updateServo();
    }
  }
}

// Function to handle the state machine
void runStateMachine() {
  unsigned long elapsed = millis() - stateStartTime;

  switch (systemState) {
    case 0:  // Start DC motor
      DCmotor.run(FORWARD);
      if (elapsed >= 1000) {  
        cfpump.run(FORWARD);
        stateStartTime = millis();  // Reset time for the next state
        systemState = 1;  // Move to the next state
      }
      break;

    case 1: 
      if (elapsed >= 7000) { 
        cfpump.run(RELEASE); 
        stateStartTime = millis();  // Reset time for the next state
        systemState = 2;  // Move to the next state
      }
      break;

    case 2:  
      if (elapsed >= 10000) {
        DCmotor.run(RELEASE);
        stateStartTime = millis();  // Reset time for the next state
        systemState = 3;  // Move to the next state
      }
      break;

    case 3:  // Start p pump
      if (elapsed >= 10000) {
        pump.run(FORWARD);
      }
      break;

    default:
      break;
  }
}

// Function to stop the system
void emergencyStop() {
  systemActive = false;
  servoStarted = false;
  systemState = 0;
  Serial.println("Emergency Stop activated!");
  DCmotor.run(RELEASE);
  cfpump.run(RELEASE);
  pump.run(RELEASE);
  Servo1.write(0);  // Reset servo to 0 position
}

// Function to move the servo
void updateServo() {
  if (millis() - servoMillis >= servoDelay) {
    servoMillis = millis();

    if (servo_pos >= 180) servoDirection = -1;

    servo_pos += servoDirection;
    Servo1.write(servo_pos);
  }
}

// Function to update turbidity sensor readings
void updateTurbiditySensors() {
  int sensor1 = analogRead(turbidityPin1);
  int sensor2 = analogRead(turbidityPin2);

  // Convert to voltage
  turbidityVoltage1 = sensor1 * (5.0 / 1024.0);
  turbidityVoltage2 = sensor2 * (5.0 / 1024.0);
}

// Function to log sensor data every second
void logSensorData() {
  currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    seconds++;

    Serial.print(seconds);
    Serial.print("\t");
    Serial.print(turbidityVoltage1);
    Serial.print("\t\t");
    Serial.println(turbidityVoltage2);
  }
}
