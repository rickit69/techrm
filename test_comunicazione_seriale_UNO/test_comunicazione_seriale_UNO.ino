
#include "DHT.h" // Load DHT library
#define DHTPIN 2 // Setting pin 2 for data coming from DHT
//#define DHTTYPE DHT22   // DHT 22  
#define DHTTYPE DHT11   // DHT 11    !!  DHT11 DOES NOT provide decimal digits for temperature and humidity, DHT22 does.
DHT dht(DHTPIN, DHTTYPE);

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;


// include the SD library:
#include <SD.h>

// set up variables using the SD utility library functions:
File myFile;
const int chipSelect = 10;
int minutes = 10;

//  SERIAL COMMANDS FORMAT
//  rf&yy/mm/dd&startbyte&stopbyte?    read the file with path yy/mm/dd (year, month, day) from startbyte to stopbyte




byte rx;    // received char (from serial port)
String pathbase = "templog/";


void setup() {
  Serial.begin(115200);
  Serial.flush(); 
  Wire.begin();
  RTC.begin(); // initialize the clock
  pinMode(10, OUTPUT);
  dht.begin(); // initialize the sensor

}



void loop() {
  
  const unsigned long totMinutes = minutes * 60 * 1000UL;
  static unsigned long lastSampleTime = 0 - totMinutes;  // initialize such that a reading is due the first time through loop()
  unsigned long now = millis();
  if (now - lastSampleTime >= totMinutes)
  {
     lastSampleTime += totMinutes;
     updatefile(); 
  }
  


  if (Serial.available() >0) // Check if serial buffer is not empty

  {
    String cmd = "";
    while(1) {
      if(Serial.available() > 0) {
        rx = Serial.read();
        cmd = cmd + (char)rx;
        //if(rx != 13){cmd = cmd + (char)rx;}    // 13 is ASCII code for carriage return which usually comes after '?'
        if(rx == 63){                          // 63 is ASCII code for '?', our "end command"
          break;
        }  
      }                 
    }
    Serial.flush(); // flush the serial buffer 
    cmd.replace("\r\n", "");
    
    
    if (cmd.substring(0,3) == "rf&") // I received the "read file" command
    {
      unsigned long startb = getValue(cmd, '&', 2).toInt();
      unsigned long stopb = getValue(cmd, '&', 3).toInt();
      String path = getValue(cmd, '&', 1);                          
      path.replace("?", "");
      readfile(path, startb, stopb);   
    } //else {Serial.println("Not a valid command-");}
    
    if (cmd.substring(0,3) == "sf&") // I received the "size file" command
    {
      String path = getValue(cmd, '&', 1);                          
      path.replace("?", "");
      sizefile(path);  
    } 

//    if (cmd.substring(0,4) == "mth&") // I received the "measure t/h" command
//    {
//      updatefile();  
//    }  
  }
}


void sizefile(String path) {
  path = pathbase + path;
  char patharray[path.length() + 1];
  path.toCharArray(patharray, path.length() + 1);
  SD.begin(chipSelect);
  myFile = SD.open(patharray);
  Serial.print('*');
  Serial.print(myFile.size());
  Serial.print('-');
  myFile.close();
}


void readfile(String path, unsigned long startb, unsigned long stopb) {
  path = pathbase + path;
  char patharray[path.length() + 1];
  path.toCharArray(patharray, path.length() + 1);
  SD.begin(chipSelect);
  myFile = SD.open(patharray);
  Serial.write("*\n");
  
  
  
  
  if (myFile) {
    // read from the file until there's nothing else in it:
    for (unsigned long i = startb; i <= stopb - 1; i++) {
      //Serial.println(i);
      myFile.seek(i);
      Serial.write(myFile.peek());
    }
    // close the file:
    myFile.close(); 
  }  
  
  
//  if (myFile) {
//    // read from the file until there's nothing else in it:
//    while (myFile.available()) {
//      Serial.write(myFile.read());
//    }
//    // close the file:
//    myFile.close(); 
//  }
  Serial.write("\n-");
}

// getValue performs a split of the string
// see also  http://stackoverflow.com/questions/9072320/arduino-split-string-into-string-array
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {
    0, -1  };
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String measure() {
  char temperature_c[6];
  char humidity_c[6];
  float h = dht.readHumidity(); // reads humidity and puts the value in the h variable
  float t = dht.readTemperature(false); // reads temperature and puts the value in the t variable
  dtostrf(t, 0, 2, temperature_c);  // Here we need to convert the float to a string
  dtostrf(h, 0, 2, humidity_c);  // Here we need to convert the float to a string
  return (String)temperature_c + "," + (String)humidity_c;
  //DateTime now = RTC.now();
  //return (String)temperature_c + "," + (String)humidity_c + "," + sanitize((String)now.hour()) + ":" + sanitize((String)now.minute()) + ":" + sanitize((String)now.second());
}

String sanitize(String hms) {
  if(hms.length() < 2) {
    hms = "0" + hms;
  }
  return hms;
}


void updatefile() {
  DateTime now = RTC.now();
  String path_folder = pathbase + sanitize(((String)now.year()).substring(2,5)) + "/" + sanitize((String)now.month()) + "/";
  String path_file = path_folder + sanitize((String)now.day()); 
  //String line = measure() + "," + sanitize((String)now.hour()) + ":" + sanitize((String)now.minute()) + ":" + sanitize((String)now.second());
  String line = measure() + "," + sanitize((String)now.hour()) + ":" + sanitize((String)now.minute());
  SD.begin(chipSelect);
  char path_folder_array[path_folder.length() + 1];
  path_folder.toCharArray(path_folder_array, path_folder.length() + 1);
  char path_file_array[path_file.length() + 1];
  path_file.toCharArray(path_file_array, path_file.length() + 1);
  
  if(!SD.exists(path_folder_array)) {SD.mkdir(path_folder_array);}  

  myFile = SD.open(path_file_array, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.println(line);
    // close the file:
    myFile.close();
  } 
}


