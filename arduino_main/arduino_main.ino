// ------------------------------------------------------------------------------------------------
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
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Pin designations
// Pins for the block inputs
// TODO: Check whether we can use a common enable line for the three blocks (that would be easier)
const int Pin_Num1_Enable    = 12;  // Block 1 Enable
const int Pin_Num1_Data      = 11;  // Block 1 Data
const int Pin_Op_Enable      =  8;  // Operator block Enable
const int Pin_Op_Data        =  7;  // Operator block Data
const int Pin_Num2_Enable    = 10;  // Block 2 Enable
const int Pin_Num2_Data      =  9;  // Block 2 Data

// Pins for the display unit
const int Pin_Display_Enable =  6;  // Display Unit Enable TODO: change hardware pin from 2
const int Pin_Display_Data   =  5;  // Display Unit Data   TODO: change hardware pin from 4

// Common clock
const int Pin_CLK            =  4;  // Synchronous clock signal TODO: change hardware pin from 3

// Analog pins
const int Pin_Battery_Level  = 14;  // Analog input pin for battery level
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Constant data
// Possible number combinations
int ValidNumberList[40] = {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
                            15, 16, 17, 18, 20, 21, 24, 25, 27, 28, 30, 32, 35, 36, 40, 
                            42, 45, 48, 49, 54, 56, 63, 64, 72, 81};
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Function prototyping (declarations)
int  ReadData(int data_pin);
void SendDigitsToDisplay(int data_pin, int digit_10, int digit_1);
int  ReadBatteryLevel();
void test();
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Function definitions
// Convert 8-bit parallel to serial and read a two digit number
int ReadData(int data_pin) {
    int data = shiftIn(data_pin, Pin_CLK, MSBFIRST);
    return data; // returns the raw 8 bits
    return ((data >> 4) and 0b00001111)*10 + (data and 0b00001111);
}

// Output a two digit number and convert 8-bit serial to parallel
void SendDigitsToDisplay(int data_pin, int digit_10, int digit_1) {
    shiftOut(data_pin, Pin_CLK, MSBFIRST, (digit_10 << 4) + digit_1);
}

// Read the battery level from analog pin "Pin_Battery_Level"
// and return a 10-bit integer representing the battery level.
int ReadBatteryLevel() {
    int BatteryLevel10bit = (int) (analogRead(Pin_Battery_Level));
    return BatteryLevel10bit;
}
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Test shift registers and seven segment display
void test() {
    // Example code for serial communication for the 7SSDs. -------------------
    digitalWrite(Pin_Display_Enable, HIGH); // Negative logic
    delay(1000);
    for (int i = 0; i <= 9; i++) {
        for (int j = 0; j <= 9; j++) {
            SendDigitsToDisplay(Pin_Display_Data, i, j);
            delay(250);
        }
    }
    digitalWrite(Pin_Display_Enable, LOW); // Negative logic
    delay(1000);
}
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Setup and Main Loop
void setup() {
    // I/O pin setup
    // Digital outputs
    pinMode(Pin_Num1_Enable,    OUTPUT);     // Operand 1 enable
    pinMode(Pin_Op_Enable,      OUTPUT);     // Operator enable
    pinMode(Pin_Num2_Enable,    OUTPUT);     // Operand 2 enable
    pinMode(Pin_Display_Enable, OUTPUT);     // Display enable
    pinMode(Pin_Display_Data,   OUTPUT);     // Display serial out
    pinMode(Pin_CLK,            OUTPUT);     // Synchronous clock

    // Digital inputs
    pinMode(Pin_Num1_Data,      INPUT);      // Operand 1 serial in
    pinMode(Pin_Num2_Data,      INPUT);      // Operand 2 serial in
    pinMode(Pin_Op_Data,        INPUT);      // Operator  serial in

    // Analog inputs
    pinMode(Pin_Battery_Level,  INPUT);  // Battery voltage
}

// Main loop
void loop() {
    test();
}
// ------------------------------------------------------------------------------------------------
