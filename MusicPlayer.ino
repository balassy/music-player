// Standard Arduino libraries.
#include <SoftwareSerial.h>       // https://www.arduino.cc/en/Reference/softwareSerial

// Third-party libraries.
#include <DFRobotDFPlayerMini.h>  // DFRobotDFPlayerMini library by DFRobot (https://github.com/DFRobot/DFRobotDFPlayerMini), MP3 source: https://www.101soundboards.com/
#include <OneButton.h>            // OneButton by Matthias Hertel (https://github.com/mathertel/OneButton)

// Pin configuration.
#define BUTTON_PIN 2         // Arduino pin - button - Arduino GND
#define TO_PLAYER_PIN 11     // Arduino pin (5V) -> 1kΩ resistor -> DFPlayer RX pin (3.3V)
#define FROM_PLAYER_PIN 10   // Arduino pin (5V) <- DFPlayer TX pin (3.3V)

// DFPlayer connection.
SoftwareSerial playerSoftwareSerial(FROM_PLAYER_PIN, TO_PLAYER_PIN); // RX, TX
DFRobotDFPlayerMini player;

// Buttons.
OneButton btn = OneButton(BUTTON_PIN, true, true);  // Button is active LOW (connected to GND), internal pull-up resistor enabled.

void setup()
{
  setupSerial();
  setupPlayer();
  setupButton();
}

void setupSerial() {
  Serial.begin(115200);
  Serial.println(F("Starting..."));
}

void setupPlayer() {
  Serial.println(F("[Player] Initializing... (may take 3-5 seconds)"));
  
  playerSoftwareSerial.begin(9600);

  while (!player.begin(playerSoftwareSerial)) {  
    Serial.println(F("[Player] Unable to begin:"));
    Serial.println(F("[Player] 1.Please recheck the connection!"));
    Serial.println(F("[Player] 2.Please insert the SD card!"));
   // while(true){
   //   delay(0); // Code to compatible with ESP8266 watch dog.
   // }
  }
  Serial.println(F("[Player] Initialization completed successfully."));

  player.volume(20);  // Set volume value. From 0 to 30
  player.play(1);     // Play the first mp3, remove this line if you do not want a voice line on power-up  
}

void setupButton() {
  btn.attachClick(onButtonClicked);
  btn.attachDoubleClick(onButtonDoubleClicked);
}

void loop()
{
  // Update the button state.
  btn.tick();

  // Send debug information.
  printPlayerStatus();
}

void onButtonClicked() {
  Serial.println(F("[Button] Clicked"));
  player.pause();
}

void onButtonDoubleClicked() {
  Serial.println(F("[Button] Double clicked"));
  player.start();
}

void printPlayerStatus() {
  if (player.available()) {
    uint8_t type = player.readType();
    int value = player.read();

    switch (type) {
      case TimeOut:
        Serial.println(F("[Player] Time Out!"));
        break;
      case WrongStack:
        Serial.println(F("[Player] Stack Wrong!"));
        break;
      case DFPlayerCardInserted:
        Serial.println(F("[Player] Card Inserted!"));
        break;
      case DFPlayerCardRemoved:
        Serial.println(F("[Player] Card Removed!"));
        break;
      case DFPlayerCardOnline:
        Serial.println(F("[Player] Card Online!"));
        break;
      case DFPlayerPlayFinished:
        Serial.print(F("[Player] Number:"));
        Serial.print(value);
        Serial.println(F(" Play Finished!"));
        break;
      case DFPlayerError:
        Serial.print(F("[Player] DFPlayerError:"));
        switch (value) {
          case Busy:
            Serial.println(F("Card not found"));
            break;
          case Sleeping:
            Serial.println(F("Sleeping"));
            break;
          case SerialWrongStack:
            Serial.println(F("Get Wrong Stack"));
            break;
          case CheckSumNotMatch:
            Serial.println(F("Check Sum Not Match"));
            break;
          case FileIndexOut:
            Serial.println(F("File Index Out of Bound"));
            break;
          case FileMismatch:
            Serial.println(F("Cannot Find File"));
            break;
          case Advertise:
            Serial.println(F("In Advertise"));
            break;
          default:
            break;
        }
        break;
      default:
        break;    
    }  
  }
}
