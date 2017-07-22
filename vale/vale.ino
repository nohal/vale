// Pins to connect Trig and Echo from modules
int pTrig[4] = {4,6,8,10};
int pEcho[4] = {5,7,9,11};
int pPlay = 12;

// Variable init
// How many measurements to average
const int avgcnt = 5;
// How much does the value have to change to trigger the playback
const int change_trig = 15;

// How many cycles have to appear as off to actually stop the playback
const int turnoff_cnt = 3;

//----- Real programming down bellow
int turnoff_counter = 0;
int counter = 0;
long echo[4][avgcnt];
long echonorm[4];
long empty[4] = {0,0,0,0};
long lastgood[4] = {0,0,0,0};
bool ready2play = false;
bool play;
 
void setup() {
  Serial.begin(9600);
  pinMode(pPlay, OUTPUT);
  digitalWrite(pPlay, LOW);
  delay(5000);
  Serial.println("Start!");
  for (int i = 0; i<=3; i++) {
    pinMode(pTrig[i], OUTPUT);
    pinMode(pEcho[i], INPUT);
  }
  play = false;
}

void loop()
{
  Serial.print(".");
  // set output to gnd for 2 ms (just for sure)
  // then 5 ms Vin
  // and back to GND
  Serial.println("Measuring:");
  for (int i = 0; i<=3; i++) {
    digitalWrite(pTrig[i], LOW);
    delayMicroseconds(2);
    digitalWrite(pTrig[i], HIGH);
    delayMicroseconds(5);
    digitalWrite(pTrig[i], LOW);
    // pulseIn measures the pulse duration in microseconds (us)
    // distance = echo / 58.31;
    long x = pulseIn(pEcho[i], HIGH);
    if ( ! ready2play )
    {
      echo[i][counter] = x;
      lastgood[i] = x;
    } else {
      for (int j = 1; j<avgcnt; j++) {
        echo[i][j-1] = echo[i][j];
      }
      if (x > empty[i] / 10 && x < empty[i] * 5)
      {
        echo[i][avgcnt-1] = x;
      } else {
        Serial.print(i);
        Serial.print(" measured: ");
        Serial.println(x);
        if (x >= empty[i] * 5 )
        {
          echo[i][avgcnt-1] = 2 * empty[i];
        } else {
          echo[i][avgcnt-1] = empty[i] / 2;
        }
      }
    }
    Serial.print(i);
    Serial.print(": ");
    Serial.println(echo[i][avgcnt-1]);
  }
  counter++;
  if (ready2play || counter == avgcnt)
  {
    counter = 0;
    play = false;
    for (int i = 0; i<=3; i++) {
      echonorm[i] = 0;
      for (int j = 0; j<avgcnt; j++) {
        echonorm[i] += echo[i][j];
      }
      echonorm[i] = echonorm[i]/avgcnt;
      if (!ready2play) {
        empty[i] = echonorm[i];
        Serial.print("Empty[");
        Serial.print(i);
        Serial.print("]=");
        Serial.println(empty[i]);
      } else {
        if (abs(empty[i] - echonorm[i]) > empty[i]/100*change_trig) {
          Serial.print("Because: ");
          Serial.print(i);
          Serial.print(" is ");
          Serial.println(empty[i]);
          play = true;
        }
      }
    }
    ready2play = true;
  }
   if (play) {
     turnoff_counter = 0;
     digitalWrite(pPlay, HIGH);
     Serial.print("Playing: ");
     for (int i=0; i<=3; i++) {
       Serial.print(echonorm[i]);
       Serial.print("-");
       Serial.print(empty[i]);
       Serial.print(" (");
       Serial.print(abs(empty[i]-echonorm[i])*100/empty[i]);
       Serial.print("%) ");
     }
   } else {
     if (turnoff_counter > turnoff_cnt) 
     {
       turnoff_counter = 0;
       digitalWrite(pPlay, LOW);
       Serial.print("Not playing: ");
       for (int i=0; i<=3; i++) {
         Serial.print(echonorm[i]);
         Serial.print("-");
         Serial.print(empty[i]);
         Serial.print(" (");
         Serial.print(abs(empty[i]-echonorm[i])*100/empty[i]);
         Serial.print("%) ");
       }
     } else {
       Serial.print("Turnoff counter: ");
       Serial.println(turnoff_counter);
       turnoff_counter++;
     }
   }
   Serial.println("");
  // wait 0.5s
  delay(1000);
}
