#include <Wire.h>
#include <VL53L0X.h>

#define BT Serial1

/* ---------- TOF ---------- */

VL53L0X rightSensor;
VL53L0X centerSensor;
VL53L0X leftSensor;

#define XSHUT_RIGHT PB12
#define XSHUT_CENTER PB13
#define XSHUT_LEFT PB14

#define SDA_PIN PB9
#define SCL_PIN PB8

/* ---------- MOTOR ---------- */

#define AIN1 PA0
#define AIN2 PA1
#define PWMA PA7

#define BIN1 PA2
#define BIN2 PA3
#define PWMB PA6

#define STBY PA4

/* ---------- ENCODERS ---------- */

#define ENC_L_A PB4
#define ENC_L_B PB5
#define ENC_R_A PB6
#define ENC_R_B PB7

volatile long leftTicks = 0;
volatile long rightTicks = 0;

/* ---------- PARAMETERS ---------- */

int baseSpeed = 170;

int CENTER_STOP_DIST = 60;

int LEFT_WALL_DIST = 35;
int RIGHT_WALL_DIST = 60;

int LEFT_THRESHOLD = 80; 
int RIGHT_THRESHOLD = 80;

#define LEFT_TURN_TICKS 10
#define RIGHT_TURN_TICKS 10

/* ---------- ENCODER ISR ---------- */

void leftEncoderISR()
{
  if (digitalRead(ENC_L_B))
    leftTicks++;
  else
    leftTicks--;
}

void rightEncoderISR()
{
  if (digitalRead(ENC_R_B))
    rightTicks++;
  else
    rightTicks--;
}

/* ---------- MOTOR CONTROL ---------- */

void stopMotor()
{
  analogWrite(PWMA,0);
  analogWrite(PWMB,0);
}

void forward()
{
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);

  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,HIGH);
}
void backward()
{
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,HIGH);

  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
}

void turnLeftMotor()
{
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);

  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
}

void turnRightMotor()
{
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,HIGH);

  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,HIGH);
}

/* ---------- FORWARD WALL FOLLOW ---------- */

void forwardControl()
{
  forward();

  while(true)
  {
    int r = rightSensor.readRangeContinuousMillimeters();
    int c = centerSensor.readRangeContinuousMillimeters();
    int l = leftSensor.readRangeContinuousMillimeters();

    BT.print("R:");
    BT.print(r);
    BT.print(" C:");
    BT.print(c);
    BT.print(" L:");
    BT.println(l);

  bool leftOpen = l > LEFT_THRESHOLD;

if(leftOpen || c < CENTER_STOP_DIST)
{
    stopMotor();

    if(leftOpen)
    {
        forward();  
        analogWrite(PWMA,170);   
        analogWrite(PWMB,170);     // move forward slightly
        delay(300);      // small forward distance
        stopMotor();

        turnLeft90();
    }

    return;
}

    int leftSpeed = baseSpeed;
    int rightSpeed = baseSpeed;

    if(l < LEFT_WALL_DIST)
      rightSpeed += 55;

    if(r < RIGHT_WALL_DIST)
      leftSpeed += 55;

    leftSpeed = constrain(leftSpeed,120,255);
    rightSpeed = constrain(rightSpeed,120,255);

    analogWrite(PWMA,leftSpeed);
    analogWrite(PWMB,rightSpeed);

    delay(5);
  }
}

/* ---------- LEFT TURN (dl/dr METHOD) ---------- */


void turnLeft90()
{
  BT.println("Turn LEFT");

  leftTicks = 0;
  rightTicks = 0;

  long prevLeft = 0;
  long prevRight = 0;
  long turnTicks = 0;

  while(turnTicks < LEFT_TURN_TICKS)
  {
    turnLeftMotor();

    analogWrite(PWMA,170);
    analogWrite(PWMB,170);

    long dl = abs(leftTicks - prevLeft);
    long dr = abs(rightTicks - prevRight);

    turnTicks += dl + dr;

    prevLeft = leftTicks;
    prevRight = rightTicks;

    BT.print("TurnTicks:");
    BT.println(turnTicks);
  }

  stopMotor();
}
void backwardUntilOpen()
{
  BT.println("Backward searching path");

  backward();

  analogWrite(PWMA,160);
  analogWrite(PWMB,160);

  unsigned long startTime = millis();

  while(true)
  {
    int r = rightSensor.readRangeContinuousMillimeters();
    int c = centerSensor.readRangeContinuousMillimeters();
    int l = leftSensor.readRangeContinuousMillimeters();

    BT.print("Back R:");
    BT.print(r);
    BT.print(" C:");
    BT.print(c);
    BT.print(" L:");
    BT.println(l);

    bool leftOpen = l > LEFT_THRESHOLD;
    bool rightOpen = r > RIGHT_THRESHOLD;

    /* ---- If opening found ---- */

    if(leftOpen || rightOpen)
    {
      stopMotor();
      delay(200);

      if(leftOpen)
      {
        turnLeft90();
      }
      else if(rightOpen)
      {
        turnRight90();
      }

      return;
    }

    /* ---- Timeout condition ---- */

    if(millis() - startTime >   4000)
    {
      BT.println("Backward search timeout");
      stopMotor();
      return;
    }

    delay(5);
  }
}
/* ---------- RIGHT TURN (dl/dr METHOD) ---------- */

void turnRight90()
{
  BT.println("Turn RIGHT");

  leftTicks = 0;
  rightTicks = 0;

  long prevLeft = 0;
  long prevRight = 0;
  long turnTicks = 0;

  while(turnTicks < RIGHT_TURN_TICKS)
  {
    turnRightMotor();

    analogWrite(PWMA,170);
    analogWrite(PWMB,170);

    long dl = abs(leftTicks - prevLeft);
    long dr = abs(rightTicks - prevRight);

    turnTicks += dl + dr;

    prevLeft = leftTicks;
    prevRight = rightTicks;

    BT.print("TurnTicks:");
    BT.println(turnTicks);
  }

  stopMotor();
}
/* ---------- LSRB DECISION ---------- */

void mazeDecision()
{
  int r = rightSensor.readRangeContinuousMillimeters();
  int c = centerSensor.readRangeContinuousMillimeters();
  int l = leftSensor.readRangeContinuousMillimeters();

  bool leftOpen = l > LEFT_THRESHOLD;
  bool frontOpen = c > CENTER_STOP_DIST;
  bool rightOpen = r > RIGHT_THRESHOLD;


  if(leftOpen)
  {
    BT.println("Decision LEFT");
    turnLeft90();
  }
  else if(frontOpen)
  {
    BT.println("Decision STRAIGHT");
  }
  else if(rightOpen)
  {
    BT.println("Decision RIGHT");
    turnRight90();
  }
  else if(!frontOpen && !leftOpen && !rightOpen)
  {
    BT.println("Decision BACK");
    backwardUntilOpen();
  }
  
}

/* ---------- SETUP ---------- */

void setup()
{
  BT.begin(9600);

  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();

  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(PWMA,OUTPUT);

  pinMode(BIN1,OUTPUT);
  pinMode(BIN2,OUTPUT);
  pinMode(PWMB,OUTPUT);

  pinMode(STBY,OUTPUT);
  digitalWrite(STBY,HIGH);

  pinMode(ENC_L_A,INPUT_PULLUP);
  pinMode(ENC_L_B,INPUT_PULLUP);
  pinMode(ENC_R_A,INPUT_PULLUP);
  pinMode(ENC_R_B,INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_L_A),leftEncoderISR,CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_R_A),rightEncoderISR,CHANGE);

  pinMode(XSHUT_RIGHT,OUTPUT);
  pinMode(XSHUT_CENTER,OUTPUT);
  pinMode(XSHUT_LEFT,OUTPUT);

  digitalWrite(XSHUT_RIGHT,LOW);
  digitalWrite(XSHUT_CENTER,LOW);
  digitalWrite(XSHUT_LEFT,LOW);

  delay(200);

  digitalWrite(XSHUT_RIGHT,HIGH);
  delay(150);
  rightSensor.init();
  rightSensor.setAddress(0x30);

  digitalWrite(XSHUT_CENTER,HIGH);
  delay(150);
  centerSensor.init();
  centerSensor.setAddress(0x31);

  digitalWrite(XSHUT_LEFT,HIGH);
  delay(150);
  leftSensor.init();
  leftSensor.setAddress(0x32);

  rightSensor.startContinuous();
  centerSensor.startContinuous();
  leftSensor.startContinuous();

  BT.println("Micromouse Ready");
}

/* ---------- LOOP ---------- */

void loop()
{
  forwardControl();
  mazeDecision();
}
