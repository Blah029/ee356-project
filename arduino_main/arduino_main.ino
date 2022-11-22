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

// Pin designations
// Digital pins
const int num1_en = 12;
const int num1_data = 11;
const int num2_en = 10;
const int num2_data = 9;
const int op_en = 8;
const int op_data = 7;
const int disp_en = 6;          // change hardware pin from 2
const int disp_data = 5;        // change hardware pin from 4
const int clk = 4;              // change hardware pin from 3
// Analog pins
const int battery_level = 14;

// Possible number combinations
int prompts[40] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
                   15, 16, 17, 18, 20, 21, 24, 25, 27, 28, 30, 32, 35, 36, 40, 
                   42, 45, 48, 49, 54, 56, 63, 64, 72, 81};

// Function prototyping
int get_data(int pin);
void send_data(int pin, int digit_10, int digit_1);
void test();


// I/O pin setup
void setup() {
    // Digital output
    pinMode(num1_en, OUTPUT);       // Operand 1 enable
    pinMode(num2_en, OUTPUT);       // Operand 2 enable
    pinMode(op_en, OUTPUT);         // Operator enable
    pinMode(disp_en, OUTPUT);       // Display enable
    pinMode(disp_data, OUTPUT);     // Display serial out
    pinMode(clk, OUTPUT);           // Common clock
    // Digital input
    pinMode(num1_data, INPUT);      // Operand 1 serial in
    pinMode(num2_data, INPUT);      // Operand 2 serial in
    pinMode(op_data, INPUT);        // Operator serial in
    // Analog input
    pinMode(battery_level, INPUT);  // Battery voltage
}


// Main loop
void loop() {

}


// Convert 8-bit parallel to serial and read a two digit number
int get_data(int pin) {
    int data = shiftIn(pin, clk, MSBFIRST);
    return ((data >> 4) and 0b00001111)*10 + (data and 0b00001111);
}


// Output a two digit number and convert 8-bit serial to parallel
void send_data(int pin, int digit_10, int digit_1) {
    digitalWrite(disp_en, HIGH); // Negative logic - disable ouptut
    shiftOut(pin, clk, MSBFIRST, (digit_10 << 4) + digit_1);
    digitalWrite(disp_en, LOW);
}


// Test shift registers and seven segment display
void test() {
    while (0) {
    int k = 0; // battery level
    k = (int) (analogRead(battery_level)/1024)*9;
    digitalWrite(disp_en, HIGH);
    delay(1000);
    shiftOut(disp_data, clk, MSBFIRST, k);
    digitalWrite(disp_en, LOW);
    delay(1000);
    }
    
    // Example code for serial communication for the 7SSDs. -------------------
    digitalWrite(disp_en, LOW);
    delay(1000);
    for (int i = 0; i <= 9; i++) {
        for (int j = 0; j <= 9; j++) {
		    shiftOut(disp_data, clk, MSBFIRST, (i << 4) + j);
            delay(250);
        }
    }
    digitalWrite(disp_en, HIGH);
    delay(1000);
}