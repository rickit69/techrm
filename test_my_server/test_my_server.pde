
/*
 * Programma di test che utilizza WiServer per servire una semplice pagina web (riferimento: http://asynclabs.com/wiki/index.php?title=WiServer)
 *
 * Ricardo Medda - 13 agosto 2012
 */

#include <stdio.h>
#include <stdlib.h>
#include <WiServer.h>

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
				 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,	// Key 1
				 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,	// Key 2
				 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,	// Key 3
				};

// setting wireless (infrastructure or ad hoc)
// infrastructure -> put WIRELESS_MODE_INFRA
// adhoc -> put WIRELESS_MODE_ADHOC
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;
// End wireless configuration parameters section ----------------------------------------


int stato = 0;
String stato_s;
const int ledPin =  12;      // Pin where to connect the red led (series with a 470 ohm resistor)
int ledState = LOW;          // Initial status of the led (off)

// Turn on the red led
void Led_Rosso_ON() {
  digitalWrite (ledPin, HIGH);
}

// Turn off the red led
void Led_Rosso_OFF() {
  digitalWrite (ledPin, LOW);
}






// Default page from the server
boolean sendPage(char* URL) {
  
    // Check if the URL in the requuest is "/" or "/stato.html"
    if ((strcmp(URL, "/") == 0) || (strcmp(URL, "/stato.html") == 0)) {     
         Invia_Pagina_Web();    
             
        // URL found
        return (true);
    } 
    if (strcmp(URL, "/on.html") == 0) {
    stato = 1;
    Led_Rosso_ON();  
    // URL found
    return (true);
    }
    if (strcmp(URL, "/off.html") == 0) {
    stato = 0;
    Led_Rosso_OFF();     
 
    // URL found
    return (true);
    }
  
    // URL not found
    return (false);
}

// This function creates and transmits the web page, depending on the I/O
void Invia_Pagina_Web() {
  stato_s = String(stato);
  // Creates the page using the print and/or println function of WiServer
  WiServer.print("<?xml version=\"1.0\" ?>");
  WiServer.print("<stato>");
  WiServer.print(stato_s);
  WiServer.print("</stato>");  
  
}

void setup() {
  
  Serial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  Led_Rosso_OFF();  
  
  // Initializing WiServer
  WiServer.init(sendPage);
  WiServer.enableVerboseMode (true); 
  
}
   
   
   
   

void loop()  {
  
  switch (stato) {
    case 0:
      Led_Rosso_OFF();
      break;
    case 1:
      Led_Rosso_ON();
      break;
  }
  
  WiServer.server_task(); 

  //Serial.println(stato);
  
  
  
  delay(10);
    
}




