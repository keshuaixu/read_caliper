const int CAL_DATA = 2;
const int CAL_CLK = 3;
volatile boolean flag = false;
volatile int32_t finalValue = 0;
volatile int32_t value = 0;
volatile uint8_t bits_so_far = 0;
volatile uint32_t lastInterrupt = 0;
int previousValue = 0;




void setup() {
  Serial.begin(115200);
 
  pinMode(CAL_CLK, INPUT);  // Clock pin = 2
  pinMode(CAL_DATA, INPUT); // Data pin = 3
 
  attachInterrupt(digitalPinToInterrupt(CAL_CLK), dataReceived, RISING);
}

void loop() {
 if(flag) {
   if(finalValue != previousValue) {
     Serial.println(finalValue,DEC);
     previousValue = finalValue;
   }
   flag = 0;
 }
}

void dataReceived() {
  unsigned char data;
  data = 1^(digitalRead(CAL_DATA)); // read DATA value as soon as possible
 
  uint32_t now = millis(); // What time is it now?
  if((now - lastInterrupt) > 50) {
    // More than 50 ms have passed since the last clock pulse.
    // Let's start with a new value
    finalValue = value; // This is what gets sent to the computer
    bits_so_far = 0;
    value = 0;
    flag = 1; // Let the main loop know the value is ready
  } else {
      if (bits_so_far < 15){          // But we need only 16bits
         if (data)                    // if 0 received, invert = >1
            value |= 0x8000;          // then set most significant bit
         value = value >> 1;          // and move it one right
      } else if (bits_so_far == 20) { // Bit 21 indicates (+/-)
         if (data)
            value = (~value)+1;                // make it negative
      }
      bits_so_far++;
   }
   lastInterrupt = now;
}
