//--------------------------------FULL LCD + ENCODER----------------------------------------


#include "Arduino.h"

// ---------------- DISPLAY PINS ----------------

// DIGIT 1 (Rightmost - ONES)
const bsp_io_port_pin_t seg1[] = {
  BSP_IO_PORT_03_PIN_02,
  BSP_IO_PORT_03_PIN_01,
  BSP_IO_PORT_00_PIN_04,
  BSP_IO_PORT_00_PIN_12,
  BSP_IO_PORT_00_PIN_13,
  BSP_IO_PORT_03_PIN_03,
  BSP_IO_PORT_00_PIN_03
};

// DIGIT 2 (TENS)
const bsp_io_port_pin_t seg2[] = {
  BSP_IO_PORT_02_PIN_05,
  BSP_IO_PORT_02_PIN_04,
  BSP_IO_PORT_00_PIN_15,
  BSP_IO_PORT_05_PIN_02,
  BSP_IO_PORT_05_PIN_01,
  BSP_IO_PORT_02_PIN_06,
  BSP_IO_PORT_00_PIN_14
};

// DIGIT 3 (HUNDREDS)
const bsp_io_port_pin_t seg3[] = {
  BSP_IO_PORT_04_PIN_10,
  BSP_IO_PORT_04_PIN_09,
  BSP_IO_PORT_01_PIN_13,
  BSP_IO_PORT_01_PIN_12,
  BSP_IO_PORT_01_PIN_11,
  BSP_IO_PORT_04_PIN_11,
  BSP_IO_PORT_01_PIN_01
};

// DIGIT 4 (Leftmost - THOUSANDS)
const bsp_io_port_pin_t seg4[] = {
  BSP_IO_PORT_04_PIN_02,
 // BSP_IO_PORT_02_PIN_13,
  BSP_IO_PORT_02_PIN_12, //not working 213, 212 crystal pin wires
  BSP_IO_PORT_01_PIN_09,
  BSP_IO_PORT_01_PIN_08,
  BSP_IO_PORT_04_PIN_00,
  BSP_IO_PORT_04_PIN_01,
  BSP_IO_PORT_01_PIN_10
};

const int totalSeg = 7;

// COMMON ANODE MAP
const uint8_t digitMap[10][7] = {
  {0,0,0,0,0,0,1}, // 0
  {1,0,0,1,1,1,1}, // 1
  {0,0,1,0,0,1,0}, // 2
  {0,0,0,0,1,1,0}, // 3
  {1,0,0,1,1,0,0}, // 4
  {0,1,0,0,1,0,0}, // 5
  {0,1,0,0,0,0,0}, // 6
  {0,0,0,1,1,1,1}, // 7
  {0,0,0,0,0,0,0}, // 8
  {0,0,0,0,1,0,0}  // 9
};

// ---------------- ENCODER ----------------
#define ENCODER_A 7
#define ENCODER_B 6

long encoderCount = 0;
int lastState;

// ---------------- DISPLAY FUNCTION ----------------
void displayNumber(int number)
{
  int thousands = number / 1000;
  int hundreds  = (number / 100) % 10;
  int tens      = (number / 10) % 10;
  int ones      = number % 10;

  for (int i = 0; i < totalSeg; i++)
  {
    // IMPORTANT: match physical wiring (RIGHT → LEFT)
    R_IOPORT_PinWrite(NULL, seg1[i], digitMap[ones][i] ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
    R_IOPORT_PinWrite(NULL, seg2[i], digitMap[tens][i] ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
    R_IOPORT_PinWrite(NULL, seg3[i], digitMap[hundreds][i] ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
    R_IOPORT_PinWrite(NULL, seg4[i], digitMap[thousands][i] ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
  }
}

// ---------------- SETUP ----------------
void setup()
{
  Serial.begin(9600);

  for (int i = 0; i < totalSeg; i++)
  {
    R_IOPORT_PinCfg(NULL, seg1[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
    R_IOPORT_PinCfg(NULL, seg2[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
    R_IOPORT_PinCfg(NULL, seg3[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
    R_IOPORT_PinCfg(NULL, seg4[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);

    // COMMON ANODE → HIGH = OFF
    R_IOPORT_PinWrite(NULL, seg1[i], BSP_IO_LEVEL_HIGH);
    R_IOPORT_PinWrite(NULL, seg2[i], BSP_IO_LEVEL_HIGH);
    R_IOPORT_PinWrite(NULL, seg3[i], BSP_IO_LEVEL_HIGH);
    R_IOPORT_PinWrite(NULL, seg4[i], BSP_IO_LEVEL_HIGH);
  }

  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  lastState = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);

  displayNumber(0);
}

// ---------------- LOOP ----------------
void loop()
{
  int currentState = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);

  if (currentState != lastState)
  {
    // CW
    if ((lastState == 0b00 && currentState == 0b01) ||
        (lastState == 0b01 && currentState == 0b11) ||
        (lastState == 0b11 && currentState == 0b10) ||
        (lastState == 0b10 && currentState == 0b00))
    {
      encoderCount++;
    }
    // CCW
    else if ((lastState == 0b00 && currentState == 0b10) ||
             (lastState == 0b10 && currentState == 0b11) ||
             (lastState == 0b11 && currentState == 0b01) ||
             (lastState == 0b01 && currentState == 0b00))
    {
      encoderCount--;
    }

    // LIMIT 0–9999
    if (encoderCount > 9999) encoderCount = 9999;
    if (encoderCount < 0) encoderCount = 0;

    Serial.println(encoderCount);

    displayNumber(encoderCount);

    lastState = currentState;
  }
}

//--------------------------------ALL 4TH DIGIT ONLY LCD------------------------------------

// #include "Arduino.h"

// // -------- DIGIT 1 (RIGHT MOST) --------
// const bsp_io_port_pin_t seg1[] = {
//   BSP_IO_PORT_03_PIN_02, // A
//   BSP_IO_PORT_03_PIN_01, // B
//   BSP_IO_PORT_00_PIN_04, // C
//   BSP_IO_PORT_00_PIN_12, // D
//   BSP_IO_PORT_00_PIN_13, // E
//   BSP_IO_PORT_03_PIN_03, // F
//   BSP_IO_PORT_00_PIN_03  // G
// };

// // -------- DIGIT 2 --------
// const bsp_io_port_pin_t seg2[] = {
//   BSP_IO_PORT_02_PIN_05, // A
//   BSP_IO_PORT_02_PIN_04, // B
//   BSP_IO_PORT_00_PIN_15, // C
//   BSP_IO_PORT_05_PIN_02, // D
//   BSP_IO_PORT_05_PIN_01, // E
//   BSP_IO_PORT_02_PIN_06, // F
//   BSP_IO_PORT_00_PIN_14  // G
// };

// // -------- DIGIT 3 --------
// const bsp_io_port_pin_t seg3[] = {
//   BSP_IO_PORT_04_PIN_10, // A
//   BSP_IO_PORT_04_PIN_09, // B
//   BSP_IO_PORT_01_PIN_13, // C
//   BSP_IO_PORT_01_PIN_12, // D
//   BSP_IO_PORT_01_PIN_11, // E
//   BSP_IO_PORT_04_PIN_11, // F
//   BSP_IO_PORT_01_PIN_01  // G
// };

// // -------- DIGIT 4 --------
// const bsp_io_port_pin_t seg4[] = {
//   BSP_IO_PORT_04_PIN_02, // A
//   BSP_IO_PORT_02_PIN_13, // B
//   BSP_IO_PORT_01_PIN_09, // C
//   BSP_IO_PORT_01_PIN_08, // D
//   BSP_IO_PORT_04_PIN_00, // E
//   BSP_IO_PORT_04_PIN_01, // F
//   BSP_IO_PORT_01_PIN_10  // G
// };

// const int totalSeg = 7;

// // COMMON ANODE (LOW = ON)
// const uint8_t digitMap[10][7] = {
//   {0,0,0,0,0,0,1}, // 0
//   {1,0,0,1,1,1,1}, // 1
//   {0,0,1,0,0,1,0}, // 2
//   {0,0,0,0,1,1,0}, // 3
//   {1,0,0,1,1,0,0}, // 4
//   {0,1,0,0,1,0,0}, // 5
//   {0,1,0,0,0,0,0}, // 6
//   {0,0,0,1,1,1,1}, // 7
//   {0,0,0,0,0,0,0}, // 8
//   {0,0,0,0,1,0,0}  // 9
// };

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   for (int i = 0; i < totalSeg; i++) {

//     R_IOPORT_PinCfg(NULL, seg1[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
//     R_IOPORT_PinCfg(NULL, seg2[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
//     R_IOPORT_PinCfg(NULL, seg3[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
//     R_IOPORT_PinCfg(NULL, seg4[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);

//     // OFF initially
//     R_IOPORT_PinWrite(NULL, seg1[i], BSP_IO_LEVEL_HIGH);
//     R_IOPORT_PinWrite(NULL, seg2[i], BSP_IO_LEVEL_HIGH);
//     R_IOPORT_PinWrite(NULL, seg3[i], BSP_IO_LEVEL_HIGH);
//     R_IOPORT_PinWrite(NULL, seg4[i], BSP_IO_LEVEL_HIGH);
//   }
// }

// void displayAll(int digit) {

//   for (int i = 0; i < totalSeg; i++) {

//     if (digitMap[digit][i] == 0) {
//       // ON
//       R_IOPORT_PinWrite(NULL, seg1[i], BSP_IO_LEVEL_LOW);
//       R_IOPORT_PinWrite(NULL, seg2[i], BSP_IO_LEVEL_LOW);
//       R_IOPORT_PinWrite(NULL, seg3[i], BSP_IO_LEVEL_LOW);
//       R_IOPORT_PinWrite(NULL, seg4[i], BSP_IO_LEVEL_LOW);
//     } else {
//       // OFF
//       R_IOPORT_PinWrite(NULL, seg1[i], BSP_IO_LEVEL_HIGH);
//       R_IOPORT_PinWrite(NULL, seg2[i], BSP_IO_LEVEL_HIGH);
//       R_IOPORT_PinWrite(NULL, seg3[i], BSP_IO_LEVEL_HIGH);
//       R_IOPORT_PinWrite(NULL, seg4[i], BSP_IO_LEVEL_HIGH);
//     }
//   }
// }

// void loop() {

//   for (int d = 0; d <= 9; d++) {

//     Serial.print("Displaying: ");
//     Serial.println(d);

//     displayAll(d);

//     delay(1000);
//   }
// }  

//-----------------------------rest all digits -------------------------------------//

// #include "Arduino.h"

// // Segment pins
// const bsp_io_port_pin_t segPins[] = {
//   BSP_IO_PORT_03_PIN_02, // A
//   BSP_IO_PORT_03_PIN_01, // B
//   BSP_IO_PORT_00_PIN_04, // C
//   BSP_IO_PORT_00_PIN_12, // D
//   BSP_IO_PORT_00_PIN_13, // E (OFF)
//   BSP_IO_PORT_03_PIN_03, // F
//   BSP_IO_PORT_00_PIN_03  // G
// };

// const int totalSeg = sizeof(segPins) / sizeof(segPins[0]);

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   for (int i = 0; i < totalSeg; i++) {
//     R_IOPORT_PinCfg(NULL, segPins[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
//   }
// }

// void loop() {

//   // Common Anode logic:
//   // LOW = ON
//   // HIGH = OFF

//   for (int i = 0; i < totalSeg; i++) {

//     if (segPins[i] == BSP_IO_PORT_00_PIN_13) {
//       // ❌ Keep this OFF
//       R_IOPORT_PinWrite(NULL, segPins[i], BSP_IO_LEVEL_HIGH);
//     } else {
//       // ✅ Turn ON all others
//       R_IOPORT_PinWrite(NULL, segPins[i], BSP_IO_LEVEL_LOW);
//     }
//   }

//   delay(2000);

//   // Turn OFF all
//   for (int i = 0; i < totalSeg; i++) {
//     R_IOPORT_PinWrite(NULL, segPins[i], BSP_IO_LEVEL_HIGH);
//   }

//   delay(2000);
// }

//-------------------------------------------------------------------------------------//

// #include "Arduino.h"

// // Segment pins (A,B,C,D,E,F,G)
// const bsp_io_port_pin_t segPins[] = {
//   BSP_IO_PORT_03_PIN_02, // A
//   BSP_IO_PORT_03_PIN_01, // B
//   BSP_IO_PORT_00_PIN_04, // C
//   BSP_IO_PORT_00_PIN_12, // D
//   BSP_IO_PORT_00_PIN_13, // E
//   BSP_IO_PORT_03_PIN_03, // F
//   BSP_IO_PORT_00_PIN_03  // G
// };

// const int totalSeg = 7;

// // Digit map for COMMON ANODE (LOW = ON, HIGH = OFF)
// const uint8_t digitMap[10][7] = {
//   {0,0,0,0,0,0,1}, // 0
//   {1,0,0,1,1,1,1}, // 1
//   {0,0,1,0,0,1,0}, // 2
//   {0,0,0,0,1,1,0}, // 3
//   {1,0,0,1,1,0,0}, // 4
//   {0,1,0,0,1,0,0}, // 5
//   {0,1,0,0,0,0,0}, // 6
//   {0,0,0,1,1,1,1}, // 7
//   {0,0,0,0,0,0,0}, // 8
//   {0,0,0,0,1,0,0}  // 9
// };

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   // Configure all segment pins
//   for (int i = 0; i < totalSeg; i++) {
//     R_IOPORT_PinCfg(NULL, segPins[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
//     R_IOPORT_PinWrite(NULL, segPins[i], BSP_IO_LEVEL_HIGH); // OFF initially
//   }
// }

// void displayDigit(int digit) {

//   for (int i = 0; i < totalSeg; i++) {

//     if (digitMap[digit][i] == 0) {
//       // LOW = ON
//       R_IOPORT_PinWrite(NULL, segPins[i], BSP_IO_LEVEL_LOW);
//     } else {
//       // HIGH = OFF
//       R_IOPORT_PinWrite(NULL, segPins[i], BSP_IO_LEVEL_HIGH);
//     }
//   }
// }

// void loop() {

//   for (int d = 0; d <= 9; d++) {

//     Serial.print("Displaying: ");
//     Serial.println(d);

//     displayDigit(d);

//     delay(1000);
//   }
// }

//-------------------------------------DOUBLE DIGIT---------------------------------------------------//

// #include "Arduino.h"

// // -------- LAST DIGIT (RIGHT MOST) --------
// const bsp_io_port_pin_t seg1[] = {
//   BSP_IO_PORT_03_PIN_02, // A
//   BSP_IO_PORT_03_PIN_01, // B
//   BSP_IO_PORT_00_PIN_04, // C
//   BSP_IO_PORT_00_PIN_12, // D
//   BSP_IO_PORT_00_PIN_13, // E
//   BSP_IO_PORT_03_PIN_03, // F
//   BSP_IO_PORT_00_PIN_03  // G
// };

// // // -------- 2ND LAST DIGIT --------
// const bsp_io_port_pin_t seg2[] = {
//   BSP_IO_PORT_02_PIN_05, // A
//   BSP_IO_PORT_02_PIN_04, // B
//   BSP_IO_PORT_00_PIN_15, // C
//   BSP_IO_PORT_05_PIN_02, // D
//   BSP_IO_PORT_05_PIN_01, // E
//   BSP_IO_PORT_02_PIN_06, // F
//   BSP_IO_PORT_00_PIN_14, // G
// };

// const int totalSeg = 7;

// // COMMON ANODE (LOW = ON)
// const uint8_t digitMap[10][7] = {
//   {0,0,0,0,0,0,1}, // 0
//   {1,0,0,1,1,1,1}, // 1
//   {0,0,1,0,0,1,0}, // 2
//   {0,0,0,0,1,1,0}, // 3
//   {1,0,0,1,1,0,0}, // 4
//   {0,1,0,0,1,0,0}, // 5
//   {0,1,0,0,0,0,0}, // 6
//   {0,0,0,1,1,1,1}, // 7
//   {0,0,0,0,0,0,0}, // 8
//   {0,0,0,0,1,0,0}  // 9
// };

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   // Configure BOTH digits
//   for (int i = 0; i < totalSeg; i++) {
//     R_IOPORT_PinCfg(NULL, seg1[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
//     R_IOPORT_PinCfg(NULL, seg2[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);

//     // OFF initially
//     R_IOPORT_PinWrite(NULL, seg1[i], BSP_IO_LEVEL_HIGH);
//     R_IOPORT_PinWrite(NULL, seg2[i], BSP_IO_LEVEL_HIGH);
//   }
// }

// // Display same digit on both
// void displayBoth(int digit) {

//   for (int i = 0; i < totalSeg; i++) {

//     if (digitMap[digit][i] == 0) {
//       // ON
//       R_IOPORT_PinWrite(NULL, seg1[i], BSP_IO_LEVEL_LOW);
//       R_IOPORT_PinWrite(NULL, seg2[i], BSP_IO_LEVEL_LOW);
//     } else {
//       // OFF
//       R_IOPORT_PinWrite(NULL, seg1[i], BSP_IO_LEVEL_HIGH);
//       R_IOPORT_PinWrite(NULL, seg2[i], BSP_IO_LEVEL_HIGH);
//     }
//   }
// }

// void loop() {

//   for (int d = 0; d <= 9; d++) {

//     Serial.print("Displaying: ");
//     Serial.println(d);

//     displayBoth(d);

//     delay(1000);
//   }
// }

//-----------------------------------------Triple digit---------------------------------------------//


// #include "Arduino.h"

// // -------- DIGIT 1 (RIGHT MOST) --------
// const bsp_io_port_pin_t seg1[] = {
//   BSP_IO_PORT_03_PIN_02, // A
//   BSP_IO_PORT_03_PIN_01, // B
//   BSP_IO_PORT_00_PIN_04, // C
//   BSP_IO_PORT_00_PIN_12, // D
//   BSP_IO_PORT_00_PIN_13, // E
//   BSP_IO_PORT_03_PIN_03, // F
//   BSP_IO_PORT_00_PIN_03  // G
// };

// // -------- DIGIT 2 --------
// const bsp_io_port_pin_t seg2[] = {
//   BSP_IO_PORT_02_PIN_05, // A
//   BSP_IO_PORT_02_PIN_04, // B
//   BSP_IO_PORT_00_PIN_15, // C
//   BSP_IO_PORT_05_PIN_02, // D
//   BSP_IO_PORT_05_PIN_01, // E
//   BSP_IO_PORT_02_PIN_06, // F
//   BSP_IO_PORT_00_PIN_14, // G
// };

// // -------- DIGIT 3 --------
// const bsp_io_port_pin_t seg3[] = {
//   BSP_IO_PORT_04_PIN_10, // A
//   BSP_IO_PORT_04_PIN_09, // B
//   BSP_IO_PORT_01_PIN_13, // C
//   BSP_IO_PORT_01_PIN_12, // D
//   BSP_IO_PORT_01_PIN_11, // E
//   BSP_IO_PORT_04_PIN_11, // F
//   BSP_IO_PORT_01_PIN_01  // G
// };

// const int totalSeg = 7;

// // COMMON ANODE (LOW = ON)
// const uint8_t digitMap[10][7] = {
//   {0,0,0,0,0,0,1}, // 0
//   {1,0,0,1,1,1,1}, // 1
//   {0,0,1,0,0,1,0}, // 2
//   {0,0,0,0,1,1,0}, // 3
//   {1,0,0,1,1,0,0}, // 4
//   {0,1,0,0,1,0,0}, // 5
//   {0,1,0,0,0,0,0}, // 6
//   {0,0,0,1,1,1,1}, // 7
//   {0,0,0,0,0,0,0}, // 8
//   {0,0,0,0,1,0,0}  // 9
// };

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   for (int i = 0; i < totalSeg; i++) {

//     // Configure all digits
//     R_IOPORT_PinCfg(NULL, seg1[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
//     R_IOPORT_PinCfg(NULL, seg2[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);
//     R_IOPORT_PinCfg(NULL, seg3[i], IOPORT_CFG_PORT_DIRECTION_OUTPUT);

//     // OFF initially
//     R_IOPORT_PinWrite(NULL, seg1[i], BSP_IO_LEVEL_HIGH);
//     R_IOPORT_PinWrite(NULL, seg2[i], BSP_IO_LEVEL_HIGH);
//     R_IOPORT_PinWrite(NULL, seg3[i], BSP_IO_LEVEL_HIGH);
//   }
// }

// void displayAll(int digit) {

//   for (int i = 0; i < totalSeg; i++) {

//     if (digitMap[digit][i] == 0) {
//       // ON
//       R_IOPORT_PinWrite(NULL, seg1[i], BSP_IO_LEVEL_LOW);
//       R_IOPORT_PinWrite(NULL, seg2[i], BSP_IO_LEVEL_LOW);
//       R_IOPORT_PinWrite(NULL, seg3[i], BSP_IO_LEVEL_LOW);
//     } else {
//       // OFF
//       R_IOPORT_PinWrite(NULL, seg1[i], BSP_IO_LEVEL_HIGH);
//       R_IOPORT_PinWrite(NULL, seg2[i], BSP_IO_LEVEL_HIGH);
//       R_IOPORT_PinWrite(NULL, seg3[i], BSP_IO_LEVEL_HIGH);
//     }
//   }
// }

// void loop() {

//   for (int d = 0; d <= 9; d++) {

//     Serial.print("Displaying: ");
//     Serial.println(d);

//     displayAll(d);

//     delay(1000);
//   }
// }

