#include <Arduino.h>
#include <SoftwareSerial.h>
#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

#define in1 4        // IN1 of L293N attached to pin 4 arduino
#define in2 5        // IN2 of L293N attached to pin 5 arduino
#define in3 6        // IN3 of L293N attached to pin 6 arduino
#define in4 7        // IN4 of L293N attached to pin 7 arduino
#define SENSOR_PIN 3 // Motion sensor attached to pin 3 arduino
#define BUZZER_PIN 2 // buzzer attached to pin 2 arduino
#define BUTTON_PIN 8 // button attached to pin 8 arduino

#define PIN_IN 10
#define PIN_OUT 11

#define OFF 0
#define ON 1

byte alarmState = OFF;
SoftwareSerial EspSerial(PIN_IN, PIN_OUT);
byte motionDetected = OFF;
byte buttonState = LOW;
const char *mario = "mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
unsigned long countdownTime = 0; // in minutes
bool armed = false;

const byte numChars = 10;
char receivedChars[numChars];
char tempChars[numChars]; // temporary array for use when parsing
char webrequest[numChars] = {0};

boolean newData = false;

void alarmOn();
void alarmOff();
void checkButton();
void doAlarmstate();
void checkSerial();
void checkCountdownTime();
void recvWithStartEndMarkers();
void parseData();

////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT); //BUZZER_PIN
  pinMode(13, OUTPUT);         //led indicator when singing a note
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(PIN_IN, INPUT);
  pinMode(PIN_OUT, OUTPUT);
  EspSerial.begin(115200);
}
unsigned long now = 0;

void loop()
{
  checkSerial();
  checkButton();
  if (armed)
  {
    checkCountdownTime();
  }
  doAlarmstate();
  if (millis() > now + 2000)
  {
    now = millis();
    // do something every 2 seconds
  }
}

void checkSerial()
{
  recvWithStartEndMarkers();
  if (newData == true)
  {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    newData = false;
    Serial.println("Done: [" + String(tempChars) + "]");
  }
}

void checkCountdownTime()
{
  static unsigned long lastTick = millis();
  unsigned long currentMillis = millis();

  if (currentMillis - lastTick >= 1000 /* 60000 */)
  {
    lastTick += 1000;
    countdownTime--;
    // Serial.print("countdownTime: ");
    // Serial.println(countdownTime);
  }

  if (countdownTime == 0)
  {
    // Serial.println("Countdown Finished");
    armed = false;
    alarmState = ON;
  }
}

void alarmOn()
{
  if (!anyrtttl::nonblocking::isPlaying())
  {
    anyrtttl::nonblocking::begin(BUZZER_PIN, mario); // start to play music
  }
  else
  {
    anyrtttl::nonblocking::play();
  }

  motionDetected = digitalRead(SENSOR_PIN); // read SENSOR_PIN value
}

void alarmOff()
{
  if (alarmState == ON || anyrtttl::nonblocking::isPlaying())
  {
    Serial.println("ALARM TURNED OFF !!");
    anyrtttl::nonblocking::stop();
  }
}

void checkButton()
{
  buttonState = digitalRead(BUTTON_PIN);
  // delay(500);
  // Serial.print("buttonState: ");
  // Serial.println(buttonState);
  if (buttonState == HIGH)
  {
    Serial.println("-Button Pressed-");
    alarmState = OFF;
  }
}

void doAlarmstate()
{
  // Serial.print("AlarmState: ");
  // Serial.println(alarmState);
  switch (alarmState)
  {
  case ON:
    // Serial.println("do Alarm_ON");
    alarmOn();
    break;
  case OFF:
    // Serial.println("do Alarm_OFF");
    alarmOff();
    break;
  }
}

void recvWithStartEndMarkers()
{
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (EspSerial.available() > 0 && newData == false)
  {
    rc = EspSerial.read();
    Serial.print(rc);

    if (recvInProgress == true)
    {
      if (rc != endMarker)
      {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars)
        {
          ndx = numChars - 1;
        }
      }
      else
      {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker)
    {
      recvInProgress = true;
    }
  }
}

void parseData()
{
  char *strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ","); // get the first part - the string
  strcpy(webrequest, strtokIndx);   // copy it to webrequest

  switch (webrequest[0])
  {
  case 'A':
    // website requests current Alarm configuration
    {
      String armedState = armed ? "1" : "0";
      String data = "<" + armedState + "," + countdownTime + ">";
      EspSerial.println(data);
    }
    break;
  case 'B':
    // website sets Brand new alarm
    {
      strtokIndx = strtok(NULL, ",");   // this continues where the previous call left off
      countdownTime = atol(strtokIndx); // convert this part to unsigned long
      armed = true;
    }
    break;
  case 'C':
    // website Cancels alarm (disarm)
    {
      countdownTime = 0;
      armed = false;
    }
    break;
  default:
    // invalid command in webrequest, do nothing
    break;
  }
}