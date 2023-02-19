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

// Pin designations
const int Pin_CLK                =  4;  // Synchronous clock signal

// Pins for the block inputs
// TODO: Check whether we can use a common enable line for the three blocks (that would be easier)
const int Pin_Num1_Data          = 11;  // Block 1 PISO(165)reg Qh' pin (Ard 11 -> 165 num1 pin 7)
const int Pin_Op_Data            =  7;  // Operator block PISO(165)reg Qh' pin (Ard 7 -> 165 op pin 7)
const int Pin_Num2_Data          =  9;  // Block 2 PISO(165)reg Qh' pin (Ard 7 -> 165 num2 pin 7)
const int Pin_Block_Shift_Load   = 12;  // Block PISO(165) register SH/LD pin (6): H: shift, L: load
const int Pin_Block_CLK_Inhibit  = 17;  // Block PISO(165) register CLK INH pin (Ard A3 -> Pin 15)

// Array of input data pins
int Pin_Block_Array[3]           = {Pin_Num1_Data, Pin_Op_Data, Pin_Num2_Data};

// Pins for the display unit
const int Pin_Display_Enable     =  6;  // BCD-7S Decoder(47) (Ard D6 -> BI/RBO' pin)                  // unused in prototype/PCB
const int Pin_Display_Data       =  5;  // Display SIPO(595) register SER pin (Ard D5 -> 595 pin 14)
const int Pin_Display_Reg_Enable =  8;  // Display SIPO(595) register RCLK pin (Ard D8 -> 595 pin 12)
const int Pin_Green_LED          = 15;  // Correct answer feedback LED pin (Ard A1 -> Green LED +)
const int Pin_Red_LED            = 16;  // Wrong answer feedback LED pin (Ard A2 -> Red LED +)
const int Pin_BarGraph_PWM       = 10;  // LED bar graph PWM output (Ard D10 -> LM3914 Filter input)

// Analog pins
const int Pin_Battery_Level      = 14;  // Analog input pin for battery level                          // unused in prototype/PCB

//Interrupt pins
const int Pin_Next_Interrupt     =  2;  // Next button pin
const int Pin_Submit_Interrupt   =  3;  // Submit button pin

// ------------------------------------------------------------------------------------------------

// Constant data

// Possible number combinations
int ValidNumberList[40] = {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
                            15, 16, 17, 18, 20, 21, 24, 25, 27, 28, 30, 32, 35, 36, 40, 
                            42, 45, 48, 49, 54, 56, 63, 64, 72, 81};

int Serial_Baud_Rate             = 9600;   // Serial baud rate (bps)
long debouncing_time             = 100000; // Debouncing time (in us) for push buttons

// ------------------------------------------------------------------------------------------------

// Global Variables
int ByteField[3] = {0, 0, 0};           // Bit field to store block data
                                        // i = 0: Num1 block
                                        // i = 1: Operator block
                                        // i = 2: Num2 block

// User inputs: variables for storage
int num1                         = 0;   // User input operand1
int op                           = 0;   // User input operator
                                        // 0 - Addition, 1 - Subtraction, 2 - Mulitplication
int num2                         = 0;   // User input operand 2
int mode                         = 0;   // User input mode select

int score                        = 0;   // Score accumulated by user
int number                       = 0;   // Number to be made using num1, num2, and op

// Debouncing
volatile unsigned long last_us   = 0;   // Last timestamp (in us) of a button press

// Flags for program flow
volatile int loop_flag           = 0;   // Break out of blink loop after interrupt
volatile int blocks_flag         = 0;   // Check proper connection of input blocks
                                        // 1 if all three blocks are set; 0 otherwise


// ------------------------------------------------------------------------------------------------

// Function prototypes (declarations)
void NextButtonPress();
void SubmitButtonPress();

void ReadFromBlocks();
void SendDigitsToDisplay(int data_pin, int digit_10, int digit_1);
int  ReadBatteryLevel();
void GenerateNumber();
void ModeFunction0();
void ModeFunction1to3();
void BlinkLED(int LED_pin, int On_duration, int Off_duration, int Blink_count);

void DisplayTest01();
void DisplayTest02();
void DisplayTest03();

// ------------------------------------------------------------------------------------------------

// Generic functions

void BlinkLED(int LED_pin, int On_duration, int Off_duration, int Blink_count) {
    for (int p = 1; p <= Blink_count; p++) {
        digitalWrite(LED_pin, HIGH);
        delay(On_duration);
        digitalWrite(LED_pin, LOW);
        delay(Off_duration);
    }
}

// ------------------------------------------------------------------------------------------------

// Interrupt service routines (ISRs).

// Interrupt flags
volatile bool InterruptOccurred       = false;
volatile bool NextButtonISROccurred   = false;
volatile bool SubmitButtonISROccurred = false;

// A wrapper function for push button debouncing 
void Debounce(void (*)());
void Debounce(void (*function)()) {
    if ((long) (micros() - last_us) >= debouncing_time) {
        InterruptOccurred = true;
        //noInterrupts();

        NextButtonISROccurred   = false;
        SubmitButtonISROccurred = false;
        
        (*function)(); // run the function given in argument
        last_us = micros();
        
        delay(1000); // Inhibit further bounces for this many ms

        InterruptOccurred = false;
        //interrupts();
    }
}


// ISR for Next button press
void ISRNextButtonPress() {
    if (!InterruptOccurred)
        NextButtonISROccurred   = true;
}


// ISR for Submit button press
void ISRSubmitButtonPress() {
    if (!InterruptOccurred)
        SubmitButtonISROccurred = true;
}


// To run when user presses next. TODO: implement the LED bar graph
void NextButtonPress() {
    loop_flag = 0; // Break the loop
    
    Serial.println(" ");
    Serial.println("Next button interrupt occurred");

    digitalWrite(Pin_Green_LED, LOW);
    digitalWrite(Pin_Red_LED,   LOW);

    if (score == 10) {
        analogWrite(Pin_BarGraph_PWM, 0);
        // TODO: clear bar graph
    }

    // Generate a random number and send it to the display
    GenerateNumber();
}


// To run when user submits an answer. TODO: implement the LED bar graph, fix debounce.
void SubmitButtonPress() {
    blocks_flag = 0; // Break the loop

    Serial.println(" ");
    Serial.println("Submit button interrupt occurred");

    digitalWrite(Pin_Green_LED, LOW);
    digitalWrite(Pin_Red_LED, LOW);
    ReadFromBlocks();

    if (blocks_flag == 1) { // Check if all three blocks are inserted
        if (mode == 0) {
            ModeFunction0();
        }
        else if (mode > 0 && mode <= 3) {
            ModeFunction1to3();
        }
        // TODO: update the bar graph
        if (score == 10) {
            loop_flag = 1;
            while (loop_flag) {
                BlinkLED(Pin_Green_LED, 250, 250, 1);
                // TODO: Turn on all bar graph LEDs
                // TODO: Turn off all bar graph LEDs
                
                //digitalWrite(Pin_Green_LED, HIGH);
                //delay(250);
                //digitalWrite(Pin_Green_LED, LOW);
                //delay(250);
            }
        }
    }
    else {
        // Error: At least one block is not inserted
        Serial.println("At least one block is not inserted");
        BlinkLED(Pin_Red_LED, 500, 500, 1);
        BlinkLED(Pin_Green_LED, 500, 500, 1);
    }

    /*
    for (int p = 0; p <= 255; p++) {
        analogWrite(Pin_BarGraph_PWM, p);
        delayMicroseconds(500000);
        Serial.print(p);
        Serial.println(micros());
    }
    for (int p = 255; p >= 0; p--) {
        analogWrite(Pin_BarGraph_PWM, p);
        delayMicroseconds(500000);
        Serial.print(p);
        Serial.println(micros());
    }
    */
}

// ------------------------------------------------------------------------------------------------

// Main functions

// Reads the parallel-in serial out registers (blocks)
// into bit fields BlockNum1, BlockOp, BlockNum2.
void ReadFromBlocks() {
    Serial.println("Read from blocks");

    for (int i = 0; i < 3; i++) {
        // Write a pulse to (SH/LD) pin (load the data to register)
        digitalWrite(Pin_Block_Shift_Load, LOW);  delayMicroseconds(5);
        digitalWrite(Pin_Block_Shift_Load, HIGH); delayMicroseconds(5);

        digitalWrite(Pin_CLK, HIGH); // Rise-edge CLK
        digitalWrite(Pin_Block_CLK_Inhibit, LOW);

        // Read raw data of byte i
        ByteField[i] = ~shiftIn(Pin_Block_Array[i], Pin_CLK, MSBFIRST);
        digitalWrite(Pin_Block_CLK_Inhibit, HIGH);
        digitalWrite(Pin_CLK, LOW); // Fall-edge CLK
    }
    // Interpret the read bit field and obtain the digits and the operator
    // if: check whether the MSB of each block is HIGH (i.e. block inserted)
    if (
        ((ByteField[0] >> 7) & 0b00000001) && 
        ((ByteField[1] >> 7) & 0b00000001) && 
        ((ByteField[2] >> 7) & 0b00000001)
        ) {
        blocks_flag = 1;
        num1 = ((ByteField[0] >> 4) & 0b00000111)*10 + (ByteField[0] & 0b00001111);
        mode = (ByteField[1] >> 4) & 0b00000111;
        op = ByteField[1] & 0b00001111;
        num2 = ((ByteField[2] >> 4) & 0b00000111)*10 + (ByteField[2] & 0b00001111);
    }
    else {
        // At least one block is not inserted
        blocks_flag = 0;
        num1 = 0;
        mode = 0;
        op = 0;
        num2 = 0;
    }
}


// Output a two digit number and convert 8-bit serial to parallel
// Send two digits to the display
void SendDigitsToDisplay(int data_pin, int digit_10, int digit_1) {
    int send_data = (digit_10 << 4) + digit_1;
    send_data = ((digit_10 >= 0 && digit_10 <= 9) && (digit_1 >= 0 && digit_1 <= 9)) ? send_data : 0;

    digitalWrite(Pin_Display_Reg_Enable, LOW);
    shiftOut(data_pin, Pin_CLK, MSBFIRST, send_data);
    digitalWrite(Pin_Display_Reg_Enable, HIGH);
}


// Read the battery level from analog pin "Pin_Battery_Level"
// and return a 10-bit integer representing the battery level.
int ReadBatteryLevel() {
    int BatteryLevel10bit = (int) (analogRead(Pin_Battery_Level));
    return BatteryLevel10bit;
}


// Select a random number from the list and display
void GenerateNumber() {
    Serial.println("Random number generated");

    // Random number generator
    int rnd_num = random(40);
    number = ValidNumberList[rnd_num];

    // Display the generated number
    int digit_10 = (int) (number / 10);
    int digit_1  = (int) (number % 10);

    digitalWrite(Pin_Display_Enable, LOW);
    SendDigitsToDisplay(Pin_Display_Data, digit_10, digit_1);
    digitalWrite(Pin_Display_Enable, HIGH);

    Serial.println(number);
}


// Mode 0 - continuous problems, no score keeping. TODO: Implement bar graph
void ModeFunction0() {
    Serial.println("Mode 0");

    int result = 0; // Result of input operation

    // Operation based on input
    if (op == 0) {
        Serial.println("Mode 0 addition");
        result = num1 + num2;
    }
    else if (op == 1) {
        Serial.println("Mode 0 subtraction");
        result = num1 - num2;
    }
    else if (op == 2) {
        Serial.println("Mode 0 multiplication");
        result = num1 * num2;
    }
    // Comparison
    if (result == number) {
        Serial.println("Mode 0 correct");
        BlinkLED(Pin_Green_LED, 125, 125, 3);
        digitalWrite(Pin_Green_LED, HIGH);
    }
    else {
        Serial.println("Mode 0 incorrect");
        BlinkLED(Pin_Red_LED, 125, 125, 3);
        digitalWrite(Pin_Red_LED, HIGH);
    }
    Serial.println(num1);
    Serial.println(op);
    Serial.println(num2);
    Serial.println(result);
    Serial.println(number);
}


// Modes 1,2, and 3 - keep score and penalty
void ModeFunction1to3() {
    Serial.println("Mode 1to3");

    int penalty = mode - 1; // Score reduction for incorrect answers
    int result = 0;         // Result of input operation
    // Operation based on input
    if (op == 0) {
        Serial.println("Mode 1to3 addition");
        result = num1 + num2;
    }
    else if (op == 1) {
        Serial.println("Mode 1to3 subtraction");
        result = num1 - num2;
    }
    else if (op == 2) {
        Serial.println("Mode 1to3 multiplication");
        result = num1 * num2;
    }
    // Comparison
    if (result == number) {
        Serial.println("Mode 1to3 correct");
        score++;
        BlinkLED(Pin_Green_LED, 125, 125, 3);
        digitalWrite(Pin_Green_LED, HIGH);
    }
    else {
        Serial.println("Mode 1to3 incorrect");
        score = score - penalty;
        BlinkLED(Pin_Red_LED, 125, 125, 3);
        digitalWrite(Pin_Red_LED, HIGH);
    }
    Serial.println(num1);
    Serial.println(op);
    Serial.println(num2);
    Serial.println(result);
    Serial.println(number);
}


// Setup
void setup() {
    // I/O pin setup

    // Digital outputs
    pinMode(Pin_Block_Shift_Load,   OUTPUT);
    pinMode(Pin_Block_CLK_Inhibit,  OUTPUT); 

    pinMode(Pin_Display_Enable,     OUTPUT); // Display  enable
    pinMode(Pin_Display_Reg_Enable, OUTPUT); // Display reg enable
    pinMode(Pin_Display_Data,       OUTPUT); // Display serial out
    pinMode(Pin_CLK,                OUTPUT); // Synchronous clock
    pinMode(Pin_Green_LED,          OUTPUT);
    pinMode(Pin_Red_LED,            OUTPUT);
    pinMode(Pin_BarGraph_PWM,       OUTPUT);

    // Digital inputs
    pinMode(Pin_Num1_Data,          INPUT);  // Operand 1 serial in
    pinMode(Pin_Num2_Data,          INPUT);  // Operand 2 serial in
    pinMode(Pin_Op_Data,            INPUT);  // Operator  serial in

    // Analog inputs
    pinMode(Pin_Battery_Level,      INPUT);  // Battery voltage

    // Starting chime
    BlinkLED(Pin_Green_LED, 125, 125, 1);
    BlinkLED(Pin_Red_LED,   125, 125, 1);

    // Interrupt pins: push buttons
    pinMode(Pin_Next_Interrupt,     INPUT_PULLUP);
    pinMode(Pin_Submit_Interrupt,   INPUT_PULLUP);

    // Set up interrupts
    attachInterrupt(digitalPinToInterrupt(Pin_Next_Interrupt),   ISRNextButtonPress,   RISING);
    attachInterrupt(digitalPinToInterrupt(Pin_Submit_Interrupt), ISRSubmitButtonPress, RISING);

    // Pre-loop commands
    digitalWrite(Pin_Display_Enable, LOW);
    digitalWrite(Pin_Green_LED, LOW);
    digitalWrite(Pin_Red_LED, LOW);

    Serial.begin(Serial_Baud_Rate); // Begin serial monitor
    randomSeed(analogRead(Pin_Battery_Level));
    
    ReadFromBlocks();
    //NextButtonPress();
    GenerateNumber();
}


// Main loop
void loop() {
    // Process interrupt flags
    if (NextButtonISROccurred)
        Debounce(NextButtonPress);
    else if (SubmitButtonISROccurred)
        Debounce(SubmitButtonPress);
}
// ------------------------------------------------------------------------------------------------





















// ------------------------------------------------------------------------------------------------

// Test functions

// Test shift registers and seven segment display
// Example code for serial communication for the 7SSDs
/*
void DisplayTest01() {
    SendDigitsToDisplay(Pin_Display_Data, 0, 0);
    digitalWrite(Pin_Display_Enable, HIGH);
    delay(1000);
    for (int i = 0; i <= 9; i++) {
        for (int j = 0; j <= 9; j++) {
            SendDigitsToDisplay(Pin_Display_Data, i, j);
            delay(50);
        }
    }
    digitalWrite(Pin_Display_Enable, LOW);
    delay(1000);
}
*/
// Display input block values on SSD
/*
void DisplayTest02 () {
    
    // TODO: do this reading part when an interrupt occurs
    ReadFromBlocks();

    // Process raw data
    int b1d1 = (ByteField[0] >> 4) & 0b00001111;
    int b1d0 =  ByteField[0]       & 0b00001111;
    int opd1 = (ByteField[1] >> 4) & 0b00001111;
    int opd0 =  ByteField[1]       & 0b00001111;
    int b2d1 = (ByteField[2] >> 4) & 0b00001111;
    int b2d0 =  ByteField[2]       & 0b00001111;

    SendDigitsToDisplay(Pin_Display_Data, b1d1, b1d0);
    digitalWrite(Pin_Display_Enable, HIGH);
    delay(1000);
    SendDigitsToDisplay(Pin_Display_Data, opd1, opd0);
    digitalWrite(Pin_Display_Enable, HIGH);
    delay(1000);
    SendDigitsToDisplay(Pin_Display_Data, b2d1, b2d0);
    digitalWrite(Pin_Display_Enable, HIGH);
    delay(1000);

    digitalWrite(Pin_Display_Enable, LOW);
    delay(1000);
}
*/
/*
// Send two predefined digits to the display
void DisplayTest03() {
    SendDigitsToDisplay(Pin_Display_Data, 3,3);
}
*/
// ------------------------------------------------------------------------------------------------

