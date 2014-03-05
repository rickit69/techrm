
#include "DHT.h" // Load DHT library
#define DHTPIN 10 // Setting pin 10 for data coming from DHT
#define DHTTYPE DHT22   // DHT 22  
//#define DHTTYPE DHT11   // DHT 11    !!  DHT11 DOES NOT provide decimal digits for temperature and humidity, DHT22 does.
DHT dht(DHTPIN, DHTTYPE);


#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

#include "MemoryFree.h"

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);


// include the SD library:
#include <SD.h>
Sd2Card card;
SdVolume volume;
SdFile root;

// set up variables using the SD utility library functions:
File myFile;
const int chipSelect = 9;

byte battery_icon_0[8] = {
  B01110,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
};

byte battery_icon_1[8] = {
  B01110,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
};

byte battery_icon_2[8] = {
  B01110,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
};


byte battery_icon_3[8] = {
  B01110,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte battery_icon_4[8] = {
  B01110,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte battery_icon_5[8] = {
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

int battery_voltage_sensor;
float battery_voltage;
float max_battery_voltage = 9.0;
char state, state1;
int minutes_updatefile = 10;
int seconds_updatedatehour = 30;
int seconds_updatemeasurelcd = 5;
int seconds_updatemeasurebattery = 10;
String pathbase = "templog/";
byte rx;    // received char (from serial port)

void setup() {
  Wire.begin();
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  
  Serial.begin(57600);    // Serial monitor communication  default 57600
  Serial3.begin(115200);    // BT communication  default 9600
  
  rtc.begin();
  
  if (! rtc.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }

}

void loop() {
  const unsigned long totMinutes = minutes_updatefile * 60 * 1000UL;
  static unsigned long lastSampleTime = 0 - totMinutes;  // initialize such that a reading is due the first time through loop()
  unsigned long now = millis();
  if (now - lastSampleTime >= totMinutes)
  {
     lastSampleTime += totMinutes;
     updatefile(); 
  }


  const unsigned long totMinutes1 = seconds_updatedatehour * 1000UL;
  static unsigned long lastSampleTime1 = 0 - totMinutes1;  // initialize such that a reading is due the first time through loop()
  unsigned long now1 = millis();
  if (now1 - lastSampleTime1 >= totMinutes1)
  {
     lastSampleTime1 += totMinutes1;
     lcd.setCursor(0, 0);
     lcd.print(date_hour_1());
  }


  const unsigned long totMinutes2 = seconds_updatemeasurelcd * 1000UL;
  static unsigned long lastSampleTime2 = 0 - totMinutes2;  // initialize such that a reading is due the first time through loop()
  unsigned long now2 = millis();
  if (now2 - lastSampleTime2 >= totMinutes2)
  {
     lastSampleTime2 += totMinutes2;
     lcd.setCursor(0, 1);
     lcd.print(measure_lcd());
  }

  const unsigned long totMinutes3 = seconds_updatemeasurebattery * 1000UL;
  static unsigned long lastSampleTime3 = 0 - totMinutes3;  // initialize such that a reading is due the first time through loop()
  unsigned long now3 = millis();
  if (now3 - lastSampleTime3 >= totMinutes3)
  {
     lastSampleTime3 += totMinutes3;
    
     battery_voltage_sensor = analogRead(A0); 
     battery_voltage = battery_voltage_sensor * (max_battery_voltage / 1023.0);
     //Serial.println(battery_voltage);
     
     if (battery_voltage > 8.5 and battery_voltage <= 9.0)
      {
        lcd.createChar(0, battery_icon_5);
      }
      else if (battery_voltage > 8.0 and battery_voltage <= 8.5)
      {
        lcd.createChar(0, battery_icon_4);
      }
      else if (battery_voltage > 7.5 and battery_voltage <= 8.0)
      {
        lcd.createChar(0, battery_icon_3);
      }
      else if (battery_voltage > 7.0 and battery_voltage <= 7.5)
      {
        lcd.createChar(0, battery_icon_2);
      }
      else if (battery_voltage > 6.5 and battery_voltage <= 7.0)
      {
        lcd.createChar(0, battery_icon_1);
      }
      else if (battery_voltage > 6.0 and battery_voltage <= 6.5)
      {
        lcd.createChar(0, battery_icon_0);
      }
      else
      {
        lcd.createChar(0, battery_icon_0);
      }

     lcd.setCursor(15, 0);  
     lcd.write(byte(0));
  }

  if (Serial3.available() >0) // Check if serial buffer is not empty

  {
    String cmd = "";
    while(1) {
      if(Serial3.available() > 0) {
        rx = Serial3.read();
        cmd = cmd + (char)rx;
        //if(rx != 13){cmd = cmd + (char)rx;}    // 13 is ASCII code for carriage return which usually comes after '?'
        if(rx == 63){                            // 63 is ASCII code for '?', our "end command"
          break;
        }  
      }                 
    }
    Serial3.flush(); // flush the serial buffer 
    cmd.replace("\r\n", "");
    
    Serial.println(cmd);
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

    if (cmd.substring(0,4) == "mth&") // I received the "measure t/h" command
    {
      Serial3.print( "*" + measure_sd() + "#");  
    }  
    
    if (cmd.substring(0,4) == "rdt&") // I received the "read date/time" command
    {
      Serial3.print("*" + date_hour() + "#");  
    } 
    
    if (cmd.substring(0,4) == "tsd&") // I received the "type sd card" command
    {
      Serial3.print("*" + typesdcard() + "#");  
    } 
    
    if (cmd.substring(0,4) == "ssd&") // I received the "size sd card" command
    {
      Serial3.print("*" + sizesdcard() + " MB#");  
    } 
    
    if (cmd.substring(0,3) == "fm&") // I received the "free memory size" command
    {
      Serial3.print("*" + (String)freeMemory() + "#");  
    } 
    
    if (cmd.substring(0,4) == "sdt&") // I received the "update date and time" command
    {
      updatedatetime(cmd);  
    } 
    if (cmd.substring(0,4) == "rbt&") // I received the "read voltage battery" command
    {
      Serial3.print("*"); 
      Serial3.print(battery_voltage);
      Serial3.print("V#");
    } 
  }


//    if(Serial3.available() > 0){        replica la stringa ricevuta dal BT sul serial monitor
//      state = Serial3.read();
//      Serial.print(state);
//    }
    
//    if(Serial.available() > 0){         mi serve per dare i comandi AT al BT attraverso il serial monitor (il pin KEY del BT deve essere alto)
//      state1 = Serial.read();
//      Serial3.print(state1);
//  }


    
}


void updatedatetime(String cmd) {
  cmd.replace("?", "");
  uint16_t years = getValue(cmd, '&', 1).toInt();
  uint8_t months = getValue(cmd, '&', 2).toInt();
  uint8_t days = getValue(cmd, '&', 3).toInt();
  uint8_t hours = getValue(cmd, '&', 4).toInt();
  uint8_t minutes = getValue(cmd, '&', 5).toInt();
  uint8_t seconds = getValue(cmd, '&', 6).toInt();

  rtc.adjust(DateTime (years, months, days, hours, minutes, seconds));
}


String sizesdcard() {
  uint32_t volumesize;
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  volumesize /= 1024;
  return (String) volumesize;  
}


String typesdcard() {
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      return "SD1";
    case SD_CARD_TYPE_SD2:
      return "SD2";
    case SD_CARD_TYPE_SDHC:
      return "SDHC";
    default:
      return "Unknown";
  } 
}

void readfile(String path, unsigned long startb, unsigned long stopb) {
  path = pathbase + path;
  char patharray[path.length() + 1];
  path.toCharArray(patharray, path.length() + 1);
  SD.begin(chipSelect);
  myFile = SD.open(patharray);
  Serial3.write("*\n");

  if (myFile) {
    // read from the file until there's nothing else in it:
    for (unsigned long i = startb; i <= stopb - 1; i++) {
      //Serial.println(i);
      myFile.seek(i);
      Serial3.write(myFile.peek());
    }
    // close the file:
    myFile.close(); 
  }  

  Serial3.write("\n#");

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




void sizefile(String path) {
  path = pathbase + path;
  char patharray[path.length() + 1];
  path.toCharArray(patharray, path.length() + 1);
  SD.begin(chipSelect);
  myFile = SD.open(patharray);
  Serial3.print('*');
  Serial3.print(myFile.size());
  Serial3.print('#');
  myFile.close();
}



String measure_lcd() {
  char temperature_c[6];
  char humidity_c[6];
  float h = dht.readHumidity(); // reads humidity and puts the value in the h variable
  float t = dht.readTemperature(false); // reads temperature and puts the value in the t variable
  dtostrf(t, 0, 1, temperature_c);  // Here we need to convert the float to a string
  dtostrf(h, 0, 1, humidity_c);  // Here we need to convert the float to a string
  return "T:" + (String)temperature_c + "C  " + "H:" + (String)humidity_c + "%";
}


String measure_sd() {
  char temperature_c[6];
  char humidity_c[6];
  float h = dht.readHumidity(); // reads humidity and puts the value in the h variable
  float t = dht.readTemperature(false); // reads temperature and puts the value in the t variable
  dtostrf(t, 0, 1, temperature_c);  // Here we need to convert the float to a string
  dtostrf(h, 0, 1, humidity_c);  // Here we need to convert the float to a string
  return (String)temperature_c + "," + (String)humidity_c;
}

String sanitize(String hms) {
  if(hms.length() < 2) {
    hms = "0" + hms;
  }
  return hms;
}


String date_hour() {
  DateTime now = rtc.now();  
  return sanitize((String)now.day()) + '/' + sanitize((String)now.month()) + '/' + sanitize((String)now.year()) + " " + sanitize((String)now.hour()) + ':' + sanitize((String)now.minute()) + ':' + sanitize((String)now.second());
}

String date_hour_1() {
  DateTime now = rtc.now();  
  char buf[4];
  itoa (now.year(), buf, 10);
  String years = (String)buf[2] + (String)buf[3];
  return sanitize((String)now.day()) + '/' + sanitize((String)now.month()) + '/' + sanitize(years) + " " + sanitize((String)now.hour()) + ':' + sanitize((String)now.minute());
}


void updatefile() {
  DateTime now = rtc.now();
  String path_folder = pathbase + sanitize(((String)now.year()).substring(2,5)) + "/" + sanitize((String)now.month()) + "/";
  String path_file = path_folder + sanitize((String)now.day()); 
  String line = measure_sd() + "," + sanitize((String)now.hour()) + ":" + sanitize((String)now.minute());
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



