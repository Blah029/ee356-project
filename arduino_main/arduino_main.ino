/*EE356: Electronic Product Design project

A portable teaching aid that can be used to teach basic arithmetic
operations to primary school children ages 6-8 years old.

E/17/146 Jayawickrama J. P. D.
E/17/234 Pandukabhaya V. K. M.
E/17/371 Warnakulasuriya R.
*/

int num1_enable = 12;
int num1_in = 11;
int num2_enable = 10;
int num2_in = 9;
int op_enable = 8;
int op_in = 7;
int disp_enable = 6;
int disp_out = 5;
int clk = 4;
int prompts[40] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
                   15, 16, 17, 18, 20, 21, 24, 25, 27, 28, 30, 32, 35, 36, 40, 
                   42, 45, 48, 49, 54, 56, 63, 64, 72, 81};


// Set up I/O pins
void setup() {
    pinMode(num1_enable, OUTPUT);   // Operand 1 enable
    pinMode(num2_enable, OUTPUT);   // Operand 2 enable
    pinMode(op_enable, OUTPUT);     // Operator enable
    pinMode(disp_enable, OUTPUT);   // Display enable
    pinMode(disp_out, OUTPUT);      // Disoplay serial out
    pinMode(clk, OUTPUT);           // Common clock
    pinMode(num1_in, INPUT);        // Operand 1 serial in
    pinMode(num2_in, INPUT);        // Operand 2 serial in
    pinMode(op_in, INPUT);          // Operator serial in
}


// Main loop
void loop() {
}
