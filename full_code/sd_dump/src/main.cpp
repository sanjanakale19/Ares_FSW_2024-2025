#define DEBUG_MODE

/*
  Listfiles

  This example shows how print out the files in a
  directory on a SD card

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe
  modified 2 Feb 2014
  by Scott Fitzgerald

  This example code is in the public domain.
*/

#include "Globals.h"
#include "HAL.h"

#include "SD_functions.h"
#include <iostream>
#include <sstream>
File rootDir;

#define INPUT_LENGTH 40

/* Functions

ls - list files
cd - change directory
pt - print tree

rm - remove (requires confirmation)
mkdir - make directory at current path
nano - list file contents (similar to cat or echo)

*/


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(HAL::SD_CS, OUTPUT);
  digitalWrite(HAL::SD_CS, HIGH);

  Serial.print("Initializing SD card...");

  HAL::initSensorHAL();
  
  if (!SD.begin(HAL::SD_CS, *HAL::HSPI_bus)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  rootDir = SD.open("/");

  // printDirectory(root, 0);

  // const char* dir_to_add = "/sup";


  filetree(rootDir, 0);

  Serial.println("done!");
}

void loop() {

  rootDir = SD.open(root);

  if (Serial.available() > 0) {
    // read the incoming byte:
    String input;
    String arg;

    input = Serial.readStringUntil(' ');
    arg = Serial.readString();

    if (input == "ls") {
      if (arg == ".")
        ls(root);
      else
        ls(root + "/" + arg);
    }

    if (input == "cd") {
      cd(root,arg);
    }

    if (input == "rm") {
      rm(arg);
    }

    if (input == "tree") {
      filetree(rootDir, 0);
    }

    if (input == "nano") {
      nano(arg);
    }
  }

  

}




