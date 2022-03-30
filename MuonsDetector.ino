// March 23

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <cmath>

File myFile;
const int chipSelect = 10;
static const unsigned long REFRESH_INTERVAL = 1000;
static unsigned long lastRefreshTime;
double cps = 0;
int voltage1;
int voltage2;
int voltage3;

void setup() {
   // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);

  Serial.begin(115200);
  // Wait for Arduino Serial Monitor to open
  while (!Serial) {
    delay(100);
  }

  // SET THE CORRECT TIME
  // setTime(16, 14, 0, 30, 3, 2022);
 
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }

  // Set up the sd card
  SPI.setMOSI(11);  
  SPI.setCS(10);
  SPI.setMISO(12);
  SPI.setSCK(13);
 
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  SD.remove("test.txt");
 
  // Open the file.
  myFile = SD.open("test.txt", O_RDWR); //Also O_WRITE works
 
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    // myFile.println("Time, CPM, CPS");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("1 error opening test.txt");
  }

  // How to overwrite the previous contents?!
 
  // re-open the file for reading:
  readSDData();

  // Sync setup time
  lastRefreshTime = millis();
}

void loop() {
  double tthousand = 10000;

  voltage1 = analogRead(1);
  voltage2 = analogRead(2);
  voltage3 = analogRead(3);

  double muon_detector_1 = voltage1;
  double muon_detector_2 = voltage2;
 
  if (muon_detector_1 < 10 || muon_detector_2 < 10) {
    cps = 0;
  } else {
    cps += 1;
  }

  if (millis() - lastRefreshTime >= REFRESH_INTERVAL) {
    lastRefreshTime += REFRESH_INTERVAL;
    writeDataToSD(cps);
    cps = 0;
  }
    
}


void writeDataToSD(String cps) {
  // open the file.
  myFile = SD.open("test.txt", FILE_WRITE); //Also O_WRITE works
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    digitalClockDisplay(myFile, cps);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("1 error opening test.txt");
  }
}

void readSDData(){
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("2 error opening test.txt");
  }
}


void digitalClockDisplay(File myFile, String cps) {
  // digital clock display of the time
  myFile.print(hour());
  Serial.print(hour());
  printDigits(minute(), myFile);
  printDigits(second(), myFile);
  myFile.print(","+cps);
  myFile.println();
  Serial.print("\n");
}

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    return pctime;
    if (pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
      pctime = 0L; // return 0 to indicate that the time is not valid
    }
  }
  return pctime;
}

void printDigits(int digits, File myFile){
  // utility function for digital clock display: prints preceding colon and leading 0
  myFile.print(":");
  if(digits < 10) myFile.print('0');
  myFile.print(digits);

  Serial.print(":");
  if(digits < 10)
  Serial.print('0');
  Serial.print(digits);
}

