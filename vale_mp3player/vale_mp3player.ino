#include <SPI.h>
#include <SdFat.h>
#include <SFEMP3Shield.h>

int pTrig = 5;

bool play;

SdFat sd;
SFEMP3Shield MP3player;
 
void setup() {
  Serial.begin(9600);
  sd.begin(SD_SEL, SPI_HALF_SPEED);
  MP3player.begin();
  MP3player.setVolume(uint8_t(0));
  delay(5000);
  pinMode(pTrig, INPUT_PULLUP);
  play = false;
}

void loop()
{
  /*
  if (digitalRead(pTrig) == HIGH)
  {
    play = true;
  } else {
    play = false;
  }
  */
  int x = digitalRead(pTrig);
  if (x == HIGH)
  {
    Serial.println(x);
    play = true;
  } else {
    Serial.println(x);
    play = false;
  }
  if (play) {
    if (!MP3player.isPlaying()) {
      MP3player.playTrack(1);
    }
  } else {
    if (MP3player.isPlaying()) {
      MP3player.stopTrack();
    }
  }
  delay(1000);
}
