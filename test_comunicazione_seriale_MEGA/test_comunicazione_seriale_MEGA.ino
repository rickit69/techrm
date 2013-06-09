
#include <stdio.h>
#include <stdlib.h>
#include "WiServer.h"


#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2


// Wireless configuration parameters start ----------------------------------------
unsigned char local_ip[] = {192,168,0,3};   // IP address wifi board
unsigned char gateway_ip[] = {192,168,0,1}; // IP address gateway
unsigned char subnet_mask[] = {255,255,255,0};  // Subnet mask
const prog_char ssid[] PROGMEM = {"SSIDWIFI"};  // SSID wireless (max 32 bytes): replace SSIDWIFI with yours

unsigned char security_type = 1;    // 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"12345678"};   // max 64 characters

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



char cmd_url[13] = "";

// Pagina di default servita dal server
boolean sendPage(char* URL) {  
 
    
    
      
    if((URL[0] == '/') && (URL[1] == 'r') && (URL[2] == 'f') && (URL[3] == '&')) {
      strncpy(cmd_url, URL + 1 /* Offset */, 13 /* Length */);
      readfile(cmd_url);
      // URL found
      return (true);
    }

    // URL not found
    return (false);
}







//This is the sender

byte rx = 0;

void setup()
{
 Serial.begin(115200);
 Serial3.begin(115200); // set up Serial library at 115200 bps 
 
 char welcome[12] = "MEGA ready!";  
 Serial.println(welcome);
 
 

 // Inizializzo il WiServer
  WiServer.init(sendPage);
  WiServer.enableVerboseMode (false);
}

void loop()
{
 
 WiServer.server_task();
 delay(1);
 
}




char size_array_char[6] = "";
unsigned long size_unsigned_long = 0;
char cmd_get_file_size[13] = "sf&";
char current_character_size = 0;

unsigned long getfilesize(char cmd[]) {    
  
  //   Resetting variables
  size_unsigned_long = 0;
  
  for(int i = 0; i <= strlen(size_array_char); i++) {
    size_array_char[i] = 0;
  }
  size_array_char[strlen(size_array_char) + 1] = '\0';
  
  cmd_get_file_size[0] = 's';
  cmd_get_file_size[1] = 'f';
  cmd_get_file_size[2] = '&';
  for(int i = 3; i <= strlen(cmd); i++) {
    cmd_get_file_size[i] = cmd[i];
  }
  cmd_get_file_size[strlen(cmd_get_file_size) + 1] = '\0';
  Serial3.println(cmd_get_file_size);

  int i = 0;
  while(1) {
   if(Serial3.available() > 0) {
     current_character_size = (char)Serial3.read(); 
     if((current_character_size != 42) && (current_character_size != 45)) {    //ASCII * = 42, ASCII - = 45
       size_array_char[i] = current_character_size; 
       i++;
     }    
     if(current_character_size == 45) {   // 45 ASCI code for -
       break;
     }   
   } 
  }
  size_array_char[strlen(size_array_char) + 1] = '\0';  
  size_unsigned_long = atoi(size_array_char);

  return size_unsigned_long;
}


char cmd_readfile[25] = "";
char cmd_start_s[6] = "";
char cmd_stop_s[6] = "";


void readfile(char cmd[]) {
   unsigned long sizechunk = 1000;
   unsigned long size_i = getfilesize(cmd);
      
   unsigned long start_i = 0;
   unsigned long stop_i;
   if(size_i < sizechunk) {
     stop_i = start_i + size_i;
   } else {
     stop_i = start_i + sizechunk; 
   }

   cmd[11] = '&';

   while(start_i < size_i) {
     
     itoa(start_i,cmd_start_s,10);
     itoa(stop_i,cmd_stop_s,10);
     Serial.println(cmd_start_s);
     
     strcat(cmd_readfile, cmd);
     strcat(cmd_readfile, cmd_start_s);
     strcat(cmd_readfile, "&");
     strcat(cmd_readfile, cmd_stop_s);
     strcat(cmd_readfile, "?");

     cmd_readfile[25] = '\0';
     Serial.println(cmd_readfile);

     transmit(cmd_readfile);
     start_i = stop_i;
     if(size_i - stop_i >= sizechunk) {
       stop_i = stop_i + sizechunk;
     } else {
         stop_i = size_i;
     }
     
     for(int i = 0; i<= 24; i++) {
       cmd_readfile[i] = 0;
     }
     cmd_readfile[25] = '\0';

    }
   
    
   
}




void transmit(char cmd[]) {

  char aaa;
  Serial3.println(cmd);

  while(1) {
   if(Serial3.available() > 0) {
     aaa = (char)Serial3.read();
     WiServer.print(aaa);
   } 
   if(aaa == 45) {   // 45 è il codice ASCI del trattino -
       break;
     }
  }
  Serial3.flush();
} 



