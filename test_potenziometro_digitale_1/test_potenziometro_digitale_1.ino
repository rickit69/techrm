int CS_signal = 2;                      // Chip Select signal onsul pin 2 of Arduino
int CLK_signal = 4;                     // Clock signal on pin 4 of Arduino
int MOSI_signal = 5;                    // MOSI signal on pin 5 of Arduino
byte cmd_byte2 = B00010001 ;            // Command byte
int initial_value = 100;                // Setting up the initial value

void initialize() {                     // send the command byte of value 100 (initial value)
    spi_out(CS_signal, cmd_byte2, initial_value);
}

void spi_out(int CS, byte cmd_byte, byte data_byte){                        // we need this function to send command byte and data byte to the chip
    
    digitalWrite (CS, LOW);                                                 // to start the transmission, the chip select must be low
    spi_transfer(cmd_byte); // invio il COMMAND BYTE
    delay(2);
    spi_transfer(data_byte); // invio il DATA BYTE
    delay(2);
    digitalWrite(CS, HIGH);                                                 // to stop the transmission, the chip select must be high
}

void spi_transfer(byte working) {
    for(int i = 1; i <= 8; i++) {                                           // Set up a loop of 8 iterations (8 bits in a byte)
     if (working > 127) { 
       digitalWrite (MOSI_signal,HIGH) ;                                    // If the MSB is a 1 then set MOSI high
     } else { 
       digitalWrite (MOSI_signal, LOW) ; }                                  // If the MSB is a 0 then set MOSI low                                           
    
    digitalWrite (CLK_signal,HIGH) ;                                        // Pulse the CLK_signal high
    working = working << 1 ;                                                // Bit-shift the working byte
    digitalWrite(CLK_signal,LOW) ;                                          // Pulse the CLK_signal low
    }
}

void setup() {
    pinMode (CS_signal, OUTPUT);
    pinMode (CLK_signal, OUTPUT);
    pinMode (MOSI_signal, OUTPUT);

    initialize();

    Serial.begin(9600);                                                     // setting the serial speed
    Serial.println("ready!");
}

void loop() {
    for (int i = 0; i < 255; i++) {
        spi_out(CS_signal, cmd_byte2, i); 
        Serial.println(i); delay(10); 
    }
    for (int i = 255; i > 0; --i) {
        spi_out(CS_signal, cmd_byte2, i);
        Serial.println(i);
        delay(10);
    }
}
