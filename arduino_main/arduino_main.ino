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
// Synchronous clock
const int Pin_CLK               =  4;  // Synchronous clock signal

// Pins for the block inputs
// TODO: Check whether we can use a common enable line for the three blocks (that would be easier)
const int Pin_Num1_Data          = 11;  // Block 1 PISO(165) register Qh pin
const int Pin_Op_Data            =  7;  // Operator block PISO(165) register Qh pin
const int Pin_Num2_Data          =  9;  // Block 2 PISO(165) register Qh pin
int Pin_Block_Array[3]           = {Pin_Num1_Data, Pin_Op_Data, Pin_Num2_Data}; // Array of input data pins

// Pins for the display unit
const int Pin_Display_Enable     =  6;  // BCD-7S Decoder(48) BI/RBO pin - negative logic
const int Pin_Block_Shift_Load   = 12;  // Display SIPO(595) register SH/LD pin: high - shift, low - load
const int Pin_Block_CLK_Inhibit  = 10;  // Display SIPO(595) register CLK INH pin
const int Pin_Display_Data       =  5;  // Display SIPO(595) register SER pin
const int Pin_Display_Reg_Enable =  8;  // Display SIPO(595) register OE  pin - negative logic

// Analog pins
const int Pin_Battery_Level      = 14;  // Analog input pin for battery level

//Interrupt pins
const int Pin_Start_Interrupt    =  2;  // Start button pin
const int Pin_Submit_Interrupt   =  3;  // Submit button pin
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Constant data
// Possible number combinations
int ValidNumberList[40] = {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
                            15, 16, 17, 18, 20, 21, 24, 25, 27, 28, 30, 32, 35, 36, 40, 
                            42, 45, 48, 49, 54, 56, 63, 64, 72, 81};
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Global Variables
int ByteField[3] = {0, 0, 0}; // Bit field to store block data
                              // i = 0: Num1 block
                              // i = 1: Operator block
                              // i = 2: Num2 block
int score   = 0; // Score accumulated by user
int number  = 0; // Number to be made using num1, num2, and op 

int num1 = 0; // User input operand1
int mode = 0; // User input mode select
int op   = 0; // User input operator. 0 - addition, 1 - subtraction, 2 - mulitplication
int num2 = 0; // User input operand 2
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Function prototyping (declarations)
void ReadFromBlocks();
void SendDigitsToDisplay(int data_pin, int digit_10, int digit_1);
int  ReadBatteryLevel();
void GenerateNumber();
void ModeFunction0();
void ModeFunction1to3();
void StartButtonPress();
void SubmitButtonPress();

void DisplayTest01();
void DisplayTest02();
void DisplayTest03();
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Function definitions

// Reads the parallel-in serial out registers (blocks)
// into bit fields BlockNum1, BlockOp, BlockNum2.
void ReadFromBlocks() {
    for (int i = 0; i < 3; i++) {
        // Write a pulse to (SH/LD) pin (load the data to register)
        digitalWrite(Pin_Block_Shift_Load, LOW);  delayMicroseconds(5);
        digitalWrite(Pin_Block_Shift_Load, HIGH); delayMicroseconds(5);

        digitalWrite(Pin_CLK, HIGH);
        digitalWrite(Pin_Block_CLK_Inhibit, LOW);

        // Read raw data of byte i
        ByteField[i] = ~shiftIn(Pin_Block_Array[i], Pin_CLK, MSBFIRST);

        digitalWrite(Pin_Block_CLK_Inhibit, HIGH);

        digitalWrite(Pin_CLK, LOW);
    }
    num1 = ((ByteField[0] >> 4) & 0b00001111)*10 + (ByteField[0] & 0b00001111);
    mode = (ByteField[1] >> 4) & 0b00001111;
    op = ByteField[1] & 0b00001111;
    num2 = ((ByteField[2] >> 4) & 0b00001111)*10 + (ByteField[2] & 0b00001111);
}


// Output a two digit number and convert 8-bit serial to parallel
// Send two digits to the display
void SendDigitsToDisplay(int data_pin, int digit_10, int digit_1) {
    digitalWrite(Pin_Display_Reg_Enable, LOW);
    shiftOut(data_pin, Pin_CLK, MSBFIRST, (digit_10 << 4) + digit_1);
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
    srand(millis());
    number = ValidNumberList[rand()%40];
    SendDigitsToDisplay(Pin_Display_Data, (int)number/10, number%10);
}


// Mode 0 - continuous problems, no score keeping. TODO: Implement bar graph
void ModeFunction0 (){
    int result = 0; // Result of input operation
    // Operation based on input
    if (op == 0) {
        int result = num1 + num2;
    }
    else if (op == 1) {
        int result = num1 - num2;
    }
    else if (op == 2) {
        int result = num1 * num2;
    }
    // Comparison
    if (result == number) {
        //fucntion to blink bar graph
    }
    
}


// Modes 1,2, and 3 - keep score and penalty
void ModeFunction1to3() {
    int penalty = mode - 1; // Score reduction for incorrect answers
    int result = 0;         // Result of input operation
    // Operation based on input
    if (op == 0) {
        int result = num1 + num2;
    }
    else if (op == 1) {
        int result = num1 - num2;
    }
    else if (op == 2) {
        int result = num1 * num2;
    }
    // Comparison
    if (result == number) {
        score++;
    }
    else {
        score = score - penalty;
    }
}


// To run when user presses start. TODO: implement LED bara graph.
void StartButtonPress() {
    score = 0;
    //funciton to clear bar graph
    SendDigitsToDisplay(Pin_Display_Data, (int)number/10, number%10);
    GenerateNumber();
}


// To run when user submits an answer. TODO: implement LED bra graph
void SubmitButtonPress() {
    if (mode == 0) {
        ModeFunction0();
        
    }
    if (mode > 0 && mode <= 3) {
        ModeFunction1to3();
    }
    //function to update bar graph
    if (score == 10) {
        //function to flash bar graph
    }
}
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Test shift registers and seven segment display
// Example code for serial communication for the 7SSDs
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


// Display input block values on SSD
void DisplayTest02 (){
    
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

    //DisplayTest00To99();
}


void DisplayTest03() {
    SendDigitsToDisplay(Pin_Display_Data, 3,3);
}
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Setup and Main Loop
void setup() {
    // I/O pin setup
    // Digital outputs
    pinMode(Pin_Block_Shift_Load,  OUTPUT); // 
    pinMode(Pin_Block_CLK_Inhibit, OUTPUT); // 

    pinMode(Pin_Display_Enable,     OUTPUT); // Display  enable
    pinMode(Pin_Display_Reg_Enable, OUTPUT); // Display reg enable
    pinMode(Pin_Display_Data,      OUTPUT); // Display serial out
    pinMode(Pin_CLK,               OUTPUT); // Synchronous clock

    // Digital inputs
    pinMode(Pin_Num1_Data,         INPUT);  // Operand 1 serial in
    pinMode(Pin_Num2_Data,         INPUT);  // Operand 2 serial in
    pinMode(Pin_Op_Data,           INPUT);  // Operator  serial in

    // Analog inputs
    pinMode(Pin_Battery_Level,  INPUT);  // Battery voltage

    // Pre-loop commands
    digitalWrite(Pin_Display_Enable, LOW); // Switch off the display (negative logic)
    SendDigitsToDisplay(Pin_Display_Data, 0, 0);
    digitalWrite(Pin_Display_Enable, HIGH); // Negative logic
}

// Main loop
void loop() {
    StartButtonPress();
    delay(500);
}
// ------------------------------------------------------------------------------------------------
