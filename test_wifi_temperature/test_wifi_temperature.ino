
/*
 * Programma di test che utilizza WiServer per servire una semplice pagina web (riferimento: http://asynclabs.com/wiki/index.php?title=WiServer)
 *
 * Ricardo Medda - 23 marzo 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <WiServer.h>


#include <OneWire.h> 

int DS18S20_Pin = 3; //DS18S20 Signal pin on digital 3

//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 3



#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters start ----------------------------------------
unsigned char local_ip[] = {192,168,0,3};	// IP address wifi board
unsigned char gateway_ip[] = {192,168,0,1};	// IP address gateway
unsigned char subnet_mask[] = {255,255,255,0};	// Subnet mask
const prog_char ssid[] PROGMEM = {"SSIDWIFI"};	// SSID wireless (max 32 bytes): replace SSIDWIFI with yours

unsigned char security_type = 1;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"12345678"};	// max 64 characters

// Keys WEP 128-bit
// for example, if your key is AA BB CC DD EE FF 11 22 33 44 55 66 77, put:
prog_uchar wep_keys[] PROGMEM = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,  // Key 0
                 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,  // Key 1
                 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,  // Key 2
                 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,  // Key 3
                };

// setting wireless (infrastructure or ad hoc)
// infrastructure -> put WIRELESS_MODE_INFRA
// adhoc -> put WIRELESS_MODE_ADHOC
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;
// End wireless configuration parameters section ----------------------------------------


float temperature = 0;
char temperature_s[5];



// Default page from the server
boolean sendPage(char* URL) {
  
    if ((strcmp(URL, "/") == 0) || (strcmp(URL, "/stato.html") == 0)) {     
         Invia_Pagina_Web();    
             
        // URL trovata
        return (true);
    }
  
    // Check if the URL in the request is "/rt.html"
    if ((strcmp(URL, "/rt.html") == 0)) {     
         Invia_Pagina_Web();    
             
        // URL found
        return (true);
    } 

  
    // URL not found
    return (false);
}

// This function creates and transmits the web page, depending on the I/O
void Invia_Pagina_Web() {
  float temperature = getTemp();
  
  dtostrf(temperature, 0, 2, temperature_s);  // Here we need to convert the float to a string

  // Creates the page using the print and/or println function of WiServer
  WiServer.print("<?xml version=\"1.0\" ?>");
  WiServer.print("<temperature>");
  WiServer.print(temperature_s);
  WiServer.print("</temperature>");  
  
}

void setup() {
  
  Serial.begin(9600);
  

  // Initializing WiServer
  WiServer.init(sendPage);
  WiServer.enableVerboseMode (true); 
  
}
   
   
   
   

void loop()  {
  
  

  
  WiServer.server_task(); 

 
  delay(10);
    
}

// This useful function is from the blog http://bildr.org/2011/07/ds18b20-arduino/
float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -2000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -3000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}




