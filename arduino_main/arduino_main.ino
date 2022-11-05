/* 
 * EE356: Electronic Product Design
 * Course Project
 * 
 * A portable teaching aid that can be used to teach basic arithmetic
 * operations to primary school children ages 6-8 years old.

 * E/17/146 Jayawickrama J. P. D.
 * E/17/234 Pandukabhaya V. K. M.
 * E/17/371 Warnakulasuriya R.
 */

// GLOBAL VARIABLE DECLARATIONS
// Pin designations
#define SERIAL_DISP_1_EN   2
#define SERIAL_DISP_2_EN   5
#define SERIAL_DISP_1_CLK  3
#define SERIAL_DISP_1_DATA 4
#define SERIAL_DISP_2_CLK  6
#define SERIAL_DISP_2_DATA 7

/*
int num1_enable = 12;
int num1_in = 11;
int num2_enable = 10;
int num2_in = 9;

int op_enable = 8;
int op_in = 7;
int disp_enable = 6;
int disp_out = 5;

int clk = 4;
*/

// Possible number combinations
int prompts[40] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
                   15, 16, 17, 18, 20, 21, 24, 25, 27, 28, 30, 32, 35, 36, 40, 
                   42, 45, 48, 49, 54, 56, 63, 64, 72, 81};

// Set up I/O pins
void setup() {
    pinMode(SERIAL_DISP_1_EN, OUTPUT);   // num1_enable - latch
    pinMode(SERIAL_DISP_1_CLK, OUTPUT);  // clk1
    pinMode(SERIAL_DISP_1_DATA, OUTPUT); // num1_in - data
    pinMode(SERIAL_DISP_2_EN, OUTPUT);   // num2_enable - latch
    pinMode(SERIAL_DISP_2_CLK, OUTPUT);  // clk2
    pinMode(SERIAL_DISP_2_DATA, OUTPUT); // num2_in - data

    /*
    pinMode(num1_enable, OUTPUT);   // Operand 1 enable
    pinMode(num2_enable, OUTPUT);   // Operand 2 enable
    pinMode(op_enable, OUTPUT);     // Operator enable
    pinMode(disp_enable, OUTPUT);   // Display enable
    pinMode(disp_out, OUTPUT);      // Display serial out
    pinMode(clk, OUTPUT);           // Common clock
    pinMode(num1_in, INPUT);        // Operand 1 serial in
    pinMode(num2_in, INPUT);        // Operand 2 serial in
    pinMode(op_in, INPUT);          // Operator serial in
    */
}


// Main loop
void loop() {
    // Example code for serial communication for the 7SSDs. -------------------
    digitalWrite(SERIAL_DISP_1_EN, HIGH);
    delay(1000);
    for (int i = 0; i <= 9; i++) {
        shiftOut(SERIAL_DISP_1_DATA, SERIAL_DISP_1_CLK, MSBFIRST, i);
        delay(250);
    }
    digitalWrite(SERIAL_DISP_1_EN, LOW);
    delay(1000);

    digitalWrite(SERIAL_DISP_2_EN, HIGH);
    delay(1000);
    for (int i = 0; i <= 9; i++) {
        shiftOut(SERIAL_DISP_2_DATA, SERIAL_DISP_2_CLK, MSBFIRST, i);
        delay(250);
    }
    digitalWrite(SERIAL_DISP_2_EN, LOW);
    delay(1000);
    // ------------------------------------------------------------------------
}
