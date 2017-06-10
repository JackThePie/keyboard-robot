/* Code used in Bachelor "Robot playing keboard".
Program is receiving data from PC program (which is parsing music notes)
and steer servos and dc motor driver so robot can play.
by Jacek Bera and Marcin Pilch */


#include <Servo.h>

// Fingers IDs
enum ID { FINGER_1 = 0, FINGER_2, FINGER_3, FINGER_4, FINGER_5, CARRIAGE, BEATRATE, CALIBRATE, REST, STOP, START, PAUSE };
enum FINGER_CONFIG { UP = 1, WHITE, BLACK };

const byte ack = 123;
boolean permission = true;

// Class finger with possible positions
class Finger {
public:
  byte auxUp;
  byte auxWhite;
  byte auxBlack;
  byte mainUp;
  byte mainWhite;
  byte mainBlack;
  Servo auxiliary;
  Servo main;
  
  void Move(byte conf);
  void Move(byte conf, int time);
};

Finger palec[5];

const byte start = 90;

// pin - stepper motor rotation direction
const int dir = 22;
// pin - providing steps for the motor
const int tick = 23;  
// pin - limit switch           
const int limit = 24;       

byte buff[4];
int posi;
byte beatrate = 120;

// Function to move carriage
void moveCarriage(int posCurrent)
{
  static int posOld = 0;  
  int dPos = posCurrent - posOld;
  
  if (dPos < 0)
    digitalWrite(dir, LOW);
  else
    digitalWrite(dir, HIGH);
    
  for (int i = 0; i < abs(dPos); i++) {
    digitalWrite(tick, LOW);  
    delayMicroseconds(230);
    digitalWrite(tick, HIGH);  
    delayMicroseconds(230);
  }
  posOld = posCurrent;
  if (permission)
     Serial.write(ack);
}

// Calibration (recommended every track)
void calibrate() {
  while (digitalRead(limit) == HIGH) {
    digitalWrite(dir, LOW);
    digitalWrite(tick, LOW);  
    delayMicroseconds(500);
    digitalWrite(tick, HIGH);  
    delayMicroseconds(500);
  }
  digitalWrite(dir, HIGH);
  delay(200);
  for (int i = 0; i < 900; i++) {
      digitalWrite(tick, LOW);  
      delayMicroseconds(400);
      digitalWrite(tick, HIGH);  
      delayMicroseconds(400);
  }
}

// Setting up the connection and servos positions
void setup()
{
    Serial.begin(19200);
    
    for (byte i = 0; i < 5; i++) {
      palec[i].auxiliary.attach(2*i+2);
      palec[i].main.attach(2*i+3);
    }

    palec[0].auxWhite = 35;
    palec[0].mainWhite = 50;
    palec[0].auxBlack = 90;
    palec[0].mainBlack = 40;

    palec[1].auxWhite = 70;
    palec[1].mainWhite = 50;
  	palec[1].auxBlack = start; //Ten palec nie uderza w czarne klawisze
  	palec[1].mainBlack = start; //Ten palec nie uderza w czarne klawisze
    
    palec[2].auxWhite = 145;
    palec[2].mainWhite = 135;
    palec[2].auxBlack = 100;
    palec[2].mainBlack = 140;
    
	  palec[3].auxWhite = 50;
    palec[3].mainWhite = 70;
    palec[3].auxBlack = 80;
    palec[3].mainBlack = 60;
    
    palec[4].auxWhite = 145;
    palec[4].mainWhite = 125;
    palec[4].auxBlack = 100;
    palec[4].mainBlack = 140;
    
    for (byte i = 0; i < 5; i++) {
      palec[i].auxUp = start;
      palec[i].mainUp = start;
      palec[i].Move(UP);
    }
    
    pinMode(tick, OUTPUT);
    pinMode(dir, OUTPUT);
    pinMode(limit, INPUT);
}

// Main loop
void loop()
{
// Receiving data from PC
    while (Serial.available() < 4);
    Serial.readBytes((char *)buff, buffSize);

	  switch(buff[0])
    {
        case FINGER_1:
        case FINGER_2:
        case FINGER_3:
        case FINGER_4:
        case FINGER_5:
          if (buff[2] == 0)
            palec[buff[0]].Move(buff[1]);
          else
            palec[buff[0]].Move(buff[1], ((int)buff[2] << 8) | buff[3]);
          break;
        case CARRIAGE:
          posi = ((int)buff[1] << 8) | buff[2];
          moveCarriage(posi);
          break;
        case CALIBRATE:
          calibrate();
          break;
        case BEATRATE:
          beatrate = buff[1];
          break;
        case REST:
          delay(((int)buff[2] << 8) | buff[3]);
          if (permission)
             Serial.write(ack);
          break;
        case START:
          permission = true;
          if (permission)
             Serial.write(ack);
          break;
        case STOP:
        case PAUSE:
          buff[0] = 79;
          Serial.write(121);
          permission = false;
          break;
    }
}

// Function for moving carriage in manual mode
void Finger::Move(byte conf)
{
  switch (conf)
  {
    case UP:
      main.write(mainUp);
      auxiliary.write(auxUp);
      break;
    case WHITE:
      main.write(mainWhite);
      auxiliary.write(auxWhite);
      break;
    case BLACK:
      main.write(mainBlack);
      auxiliary.write(auxBlack);
      break;
  }
}

// Function for moving carriage in automatic mode
void Finger::Move(byte conf, int time)
{
  switch (conf)
  {
    case WHITE:
      main.write(mainWhite);
      auxiliary.write(auxWhite);
      break;
    case BLACK:
      main.write(mainBlack);
      auxiliary.write(auxBlack);
      break;
   }
   
   delay(time);
   main.write(mainUp);
   auxiliary.write(auxUp);
   if (permission)
     Serial.write(ack);
}
