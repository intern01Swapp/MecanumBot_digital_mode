#include <Arduino.h>
#include <RMCS2303drive.h>

RMCS2303 rmcs;

const byte ID_FL = 1, ID_FR = 2, ID_RL = 3, ID_RR = 4;
const byte IDS[4] = {ID_FL, ID_FR, ID_RL, ID_RR};

int PP_gain = 5;           // not critical in speed mode but stored anyway
int PI_gain = 3;
int VF_gain = 3;
int LPR     = 7;            // your encoder PPR
int accel   = 1000;        // acceleration value (applies in speed/position modes) :contentReference[oaicite:7]{index=7}
int speed_rpm = 100;       // initial base-motor target RPM

int INP_CONTROL_MODE = 256;

// === FUNCTION DECLARATIONS (MUST BE BEFORE setup) ===
void handleCommand(char c);
void forward();
void backward();
void turnLeft();
void turnRight();
void rotateRight();
void rotateLeft();
void stopMotors();
void forceMotors();
void parseRPM();

void setSpeedAll(int rpm) { for (byte i=0;i<4;i++) rmcs.Speed(IDS[i], rpm); }
void enableAll(byte dir)  { for (byte i=0;i<4;i++) rmcs.Enable_Digital_Mode(IDS[i], dir); }
void disableAll(byte dir) { for (byte i=0;i<4;i++) rmcs.Disable_Digital_Mode(IDS[i], dir); }

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);           // Mega → ESP32 (for bridge)
  Serial2.println("Mega is ready");


  rmcs.begin(&Serial1, 9600);
  rmcs.Serial_selection(0);
  Serial.println(F("RMCS-2303 — 4WD Digital Speed Control"));
  for (int i=0;i<4;i++) {
    rmcs.WRITE_PARAMETER(IDS[i], INP_CONTROL_MODE, PP_gain, PI_gain, VF_gain, LPR, accel, speed_rpm);
  }
  setSpeedAll(speed_rpm);
  Serial.println(F("Mega ready"));
}

void loop() {
  if (Serial2.available()) {
    char c = Serial2.read();
    if (c == '\n' || c == '\r') return;

    // Optional: show on Serial Monitor
    Serial.write(c);

    handleCommand(c);
  }
  // --- Speed Feedback Section ---
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 5000) {   // every 5 seconds
    lastPrint = millis();

    for (int i = 0; i < 4; i++) {       // read all 4 motor drivers
      long rpm = rmcs.Speed_Feedback(IDS[i]);
      Serial2.print("ID "); Serial2.print(IDS[i]);
      Serial2.print(" Speed: "); Serial2.print(rpm);
      Serial2.println(" RPM");
      delay(50);   // short gap between Modbus requests
    }
  }

}
  

  // // Feedback to ESP32 → PC
  // static uint32_t t0 = 0;
  // if (millis() - t0 > 500) {
  //   t0 = millis();
  //   Serial2.print("FL:"); Serial2.print(rmcs.Speed_Feedback(ID_FL));
  //   Serial2.print(" FR:"); Serial2.print(rmcs.Speed_Feedback(ID_FR));
  //   Serial2.print(" RL:"); Serial2.print(rmcs.Speed_Feedback(ID_RL));
  //   Serial2.print(" RR:"); Serial2.println(rmcs.Speed_Feedback(ID_RR));
  // }


// === FUNCTION DEFINITIONS ===
void handleCommand(char c) {
  if (c == 'w') forward();
  else if (c == 'z') backward();
  else if (c == 'a') turnLeft();
  else if (c == 'd') turnRight();
  else if (c == 'e') rotateRight();
  else if (c == 'q') rotateLeft();
  else if (c == 's') stopMotors();
  else if (c == 'x') forceMotors();
  else if (c == 'v') parseRPM();

}

void forward() {
  rmcs.Speed(ID_FL,speed_rpm); rmcs.Speed(ID_RL,speed_rpm);
  rmcs.Speed(ID_FR,speed_rpm); rmcs.Speed(ID_RR,speed_rpm);
  rmcs.Enable_Digital_Mode(ID_FL,1); rmcs.Enable_Digital_Mode(ID_RL,1);
  rmcs.Enable_Digital_Mode(ID_FR,0); rmcs.Enable_Digital_Mode(ID_RR,1);
  Serial2.println(F("Forward"));
}

void backward() {
  rmcs.Speed(ID_FL,speed_rpm); rmcs.Speed(ID_RL,speed_rpm);
  rmcs.Speed(ID_FR,speed_rpm); rmcs.Speed(ID_RR,speed_rpm);
  rmcs.Enable_Digital_Mode(ID_FL,0); rmcs.Enable_Digital_Mode(ID_RL,0);
  rmcs.Enable_Digital_Mode(ID_FR,1); rmcs.Enable_Digital_Mode(ID_RR,0);
  Serial2.println(F("Backward"));
}

void turnLeft() {
  rmcs.Speed(ID_FL,speed_rpm); rmcs.Speed(ID_RL,speed_rpm);
  rmcs.Speed(ID_FR,speed_rpm); rmcs.Speed(ID_RR,speed_rpm);
  rmcs.Enable_Digital_Mode(ID_FL,0); rmcs.Enable_Digital_Mode(ID_RL,1);
  rmcs.Enable_Digital_Mode(ID_FR,0); rmcs.Enable_Digital_Mode(ID_RR,0);
  Serial2.println(F("Turn Left"));
}

void turnRight() {
  rmcs.Speed(ID_FL,speed_rpm); rmcs.Speed(ID_RL,speed_rpm);
  rmcs.Speed(ID_FR,speed_rpm); rmcs.Speed(ID_RR,speed_rpm);
  rmcs.Enable_Digital_Mode(ID_FL,1); rmcs.Enable_Digital_Mode(ID_RL,0);
  rmcs.Enable_Digital_Mode(ID_FR,1); rmcs.Enable_Digital_Mode(ID_RR,1);
  Serial2.println(F("Turn Right"));
}

void rotateRight() {
  rmcs.Speed(ID_FL,speed_rpm); rmcs.Speed(ID_RL,speed_rpm);
  rmcs.Speed(ID_FR,speed_rpm); rmcs.Speed(ID_RR,speed_rpm);
  rmcs.Enable_Digital_Mode(ID_FL,1); rmcs.Enable_Digital_Mode(ID_RL,1);
  rmcs.Enable_Digital_Mode(ID_FR,1); rmcs.Enable_Digital_Mode(ID_RR,0);
  Serial2.println(F("Rotate Right"));
}

void rotateLeft() {
  rmcs.Speed(ID_FL,speed_rpm); rmcs.Speed(ID_RL,speed_rpm);
  rmcs.Speed(ID_FR,speed_rpm); rmcs.Speed(ID_RR,speed_rpm);
  rmcs.Enable_Digital_Mode(ID_FL,0); rmcs.Enable_Digital_Mode(ID_RL,0);
  rmcs.Enable_Digital_Mode(ID_FR,0); rmcs.Enable_Digital_Mode(ID_RR,1);
  Serial2.println(F("Rotate Left"));
}

void stopMotors() {
  int current = speed_rpm;      // current target speed
  int step = 100;                // decrease step (adjust for smoothness)
  int delay_ms = 50;            // delay between steps (adjust ramp rate)

  // Gradually reduce speed
  for (int spd = current; spd >= 0; spd -= step) {
    setSpeedAll(spd);
    delay(delay_ms);
  }

  // Finally disable motors
  disableAll(0);
  disableAll(1);

  Serial2.println(F("Soft Stop"));
}

void forceMotors() {
 
  // Finally disable motors
  disableAll(0);
  disableAll(1);

  Serial2.println(F("Force Stop"));
}



int setRPM = 0;

void parseRPM() {
  String num = "";
  uint32_t timeout = millis() + 500;   // allow more time for slow network

  // Read until newline or timeout
  while (millis() < timeout) {
    if (Serial2.available()) {
      char d = Serial2.read();
      if (d == '\n' || d == '\r') break;
      if (isDigit(d)) num += d;
    }
  }

  // ✅ Waited long enough; now parse the number
  if (num.length() > 0) {
    setRPM = num.toInt();
    if (setRPM < 0) setRPM = 0;
    if (setRPM > 65535) setRPM = 65535;

    setSpeedAll(setRPM);
    speed_rpm = setRPM; 

    Serial2.print("Speed set to ");
    Serial2.println(setRPM);
  } else {
    Serial2.println("Invalid speed input");
  }

  // ✅ Clear any leftover characters AFTER processing
  while (Serial2.available()) Serial2.read();
}
