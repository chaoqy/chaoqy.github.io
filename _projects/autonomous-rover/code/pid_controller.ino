#include <LSM6DS3.h>

// --- Encoder pins ---
#define ENC_L_A 4
#define ENC_L_B 5
#define ENC_R_A 2
#define ENC_R_B 3

// --- Motor pins ---
#define EA  6  // Wheel 1 PWM (must be PWM pin)
#define I1  11 // Wheel 1 direction
#define I2  10
#define EB  9  // Wheel 2 PWM (must be PWM pin)
#define I3  12 // Wheel 2 direction
#define I4  8

// --- Robot constants ---
#define TPR 3000       // ticks per motor revolution
#define RHO 0.0625     // wheel radius [m]
#define INTERVAL 10    // loop interval [ms]

// --- PID constants ---
float k_P = 180;
float k_I = 80;
float k_D = 20;

// --- State variables ---
volatile long L_encoder_ticks = 0;
volatile long R_encoder_ticks = 0;

long L_encoder_total = 0;
long R_encoder_total = 0;

float v_L = 0;
float v_R = 0;
float v_LD = 0;
float v_RD = 0;

float e_k_L = 0, e_k_R = 0;
float e_I_L = 0, e_I_R = 0;
float e_D_L = 0, e_D_R = 0;
float e_k_L_last = 0, e_k_R_last = 0;

unsigned long t_last = 0;
unsigned long last_command_time = 0;
const unsigned long SERIAL_TIMEOUT = 500; // ms

// --- IMU ---
float a_x, a_y, a_z;
float omega_x, omega_y, omega_z;

// --- Encoder interrupts ---
void L_encoder_interrupt() {
  if(digitalRead(ENC_L_B)) L_encoder_ticks++;
  else L_encoder_ticks--;
}

void R_encoder_interrupt() {
  if(digitalRead(ENC_R_B)) R_encoder_ticks++;
  else R_encoder_ticks--;
}

// --- Setup ---
void setup() {
  // Motor pins
  pinMode(EA, OUTPUT); pinMode(EB, OUTPUT);
  pinMode(I1, OUTPUT); pinMode(I2, OUTPUT);
  pinMode(I3, OUTPUT); pinMode(I4, OUTPUT);

  // Encoder pins
  pinMode(ENC_L_A, INPUT); pinMode(ENC_L_B, INPUT);
  pinMode(ENC_R_A, INPUT); pinMode(ENC_R_B, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENC_L_A), L_encoder_interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_R_A), R_encoder_interrupt, RISING);

  // IMU
  IMU.begin();

  // Serial
  Serial.begin(115200);

  t_last = millis();
}

// --- Motor control ---
void drive(int pwrL, int pwrR) {
  // Left motor
  digitalWrite(I3, pwrL < 0);
  digitalWrite(I4, pwrL >= 0);
  analogWrite(EB, constrain(abs(pwrL), 0, 255));

  // Right motor
  digitalWrite(I1, pwrR >= 0);
  digitalWrite(I2, pwrR < 0);
  analogWrite(EA, constrain(abs(pwrR), 0, 255));
}

// --- PID ---
short PID_control(float e_now, float e_int, float e_der) {
  short u = (short)(k_P * e_now + k_I * e_int + k_D * e_der);
  return constrain(u, -250, 250);
}

// --- Read sensors ---
void readInternal(float deltaT) {
  IMU.readAcceleration(a_x, a_y, a_z);
  IMU.readGyroscope(omega_x, omega_y, omega_z);

  // Snapshot encoder ticks
  long L_ticks_snapshot, R_ticks_snapshot;
  noInterrupts();
  L_ticks_snapshot = L_encoder_ticks;
  R_ticks_snapshot = R_encoder_ticks;
  L_encoder_ticks = 0;
  R_encoder_ticks = 0;
  interrupts();

  // Update total encoders
  L_encoder_total += L_ticks_snapshot;
  R_encoder_total += R_ticks_snapshot;

  // Estimate velocity [m/s]
  if(deltaT > 0) {
    float omega_L = 2.0 * PI * ((float)L_ticks_snapshot / TPR) / deltaT;
    float omega_R = 2.0 * PI * ((float)R_ticks_snapshot / TPR) / deltaT;
    v_L = RHO * omega_L;
    v_R = RHO * omega_R;
  }
}

// --- Calculate PID and drive ---
void calcSpeed(float deltaT) {
  // Errors
  e_k_L = v_LD - v_L;
  e_k_R = v_RD - v_R;

  // Integral
  e_I_L += e_k_L * deltaT;
  e_I_R += e_k_R * deltaT;

  // Derivative
  e_D_L = (e_k_L - e_k_L_last) / deltaT;
  e_D_R = (e_k_R - e_k_R_last) / deltaT;

  e_k_L_last = e_k_L;
  e_k_R_last = e_k_R;

  // Compute control
  short u_L = PID_control(e_k_L, e_I_L, e_D_L);
  short u_R = PID_control(e_k_R, e_I_R, e_D_R);

  drive(u_L, u_R);
}

// --- Receive velocity commands via Serial ---
void receiveCommands() {
  static char buffer[64];
  static int index = 0;

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      buffer[index] = '\0';
      // Expected format: V|v_LD|v_RD
      if(buffer[0] == 'V' && buffer[1] == '|') {
        char* first = buffer + 2;
        char* second = strchr(first, '|');
        if(second != NULL) {
          *second = '\0';
          second++;
          v_LD = atof(first);
          v_RD = atof(second);
          last_command_time = millis();
        }
      }
      index = 0;
    } else if(index < 63) {
      buffer[index++] = c;
    }
  }
}

// --- Safety timeout ---
void checkSafetyTimeout() {
  if(millis() - last_command_time > SERIAL_TIMEOUT) {
    v_LD = 0;
    v_RD = 0;
    e_I_L = 0;
    e_I_R = 0;
  }
}

// --- Send encoder data ---
void sendEncoder() {
  Serial.print(L_encoder_total);
  Serial.print(",");
  Serial.println(R_encoder_total);
}

// --- Main loop ---
void loop() {
  unsigned long t_now = millis();
  float deltaT = (t_now - t_last) / 1000.0;

  receiveCommands();
  checkSafetyTimeout();

  if(t_now - t_last >= INTERVAL) {
    readInternal(deltaT);
    calcSpeed(deltaT);
    sendEncoder();
    t_last = t_now;
  }
}
