/********** Obstacle Avoidance UGV
 * Purpose
 * The mC (Arduino) controls various sensors (HC-SR04, IR Infrared 
 * Obstacle Avoidance Sensor Module) and actuators (SG90 Servo, 
 * DC Motors), alongside an I2C 1602 LCD Blue Screen, in order to 
 * perform obstacle avoidance and output info for the operator.
 * Hardware
 * - HC-SR04: Trig to pin 8, Echo to pin 9.
 * - IR Sensor: OUT to pin 4 and pin 7.
 * - SG90 Servo: Signal to PWM pin 11.
 * - LCD 1602: SDA to pin A4, SCL to pin A5.
 * - Motor Driver: IN1, IN2, IN3, IN4, ENA, ENB to pins A3, A2, A1, A0, 5, 6
 * Software
 * Uses Arduino standard library calls (analogRead(), digitalWrite(), 
 * millis() etc.) and predefined objects (lcd, myservo). Uses the 
 * Wire.h and LiquidCrystal_I2C.h libraries for the display.
 * Reference
 * v5.1, K. Lagos, July 2026.
 **********/

#include <Wire.h> 
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

/*--- (Pins) ---*/ 
const int IR_LEFT = 4, IR_RIGHT = 7;
const int TRIG = 8, ECHO = 9, SERVO_PIN = 11;

/*--- (LCD) ---*/ 
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Motors (Analog 0-3 corespond to pins 14-17) */ 
const int pinLeftFwd = 14, pinLeftBack = 15;
const int pinRightFwd = 16, pinRightBack = 17;
const int speedPinL = 5, speedPinR = 6;

/*--- Control Variables ---*/ 
Servo myservo;
unsigned long moveStartTime = 0;
unsigned long moveDuration = 0;
unsigned long lastLCDUpdate = 0;
const int lcdInterval = 250;
bool isMoving = false;
int motorSpeed = 130; /* Set motor speed */

/*---SETUP---*/
void setup() {
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  pinMode(pinLeftFwd, OUTPUT); pinMode(pinLeftBack, OUTPUT);
  pinMode(pinRightFwd, OUTPUT); pinMode(pinRightBack, OUTPUT);
  pinMode(speedPinL, OUTPUT); 
  pinMode(speedPinR, OUTPUT);

  /* lcd screen initialize setup */
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print ("Robot Loading...");
  
  /* Serial connection established */
  Serial.begin(9600);

  /* Servo initialize setup */
  myservo.attach(SERVO_PIN);
  myservo.write(90); /* Initial position Forward (90 deg) */
  delay(500);
}

/*---MAIN LOOP---*/
void loop() {

  int distance = getDistance();
  /* Update check on LCD distance info depending on time interval (duty cycle) */
  if (millis() - lastLCDUpdate >= lcdInterval){
    updateLCD(distance);
    lastLCDUpdate = millis();
  }
  
  /*--- 1] Emergency IR Check---*/
  if (checkIRSafety()) {
    return; /* If there was an Emergency stop start the MAIN LOOP again. */
  }

  /*--- 2] Ultrassonic Check---
  *Checking for obstacle closer to distance = 20cm
  *Deciding the clearest path
  */
  if (distance < 20 && distance > 0) {
    handleObstacleFront();
  } 
  else {
    /*Clear Path*/
    if (!isMoving) {
      startAdvance(500); /* If the vehicle was steady it's start moving for a fixed duration of 500ms */
    }
  }
  checkMoveTimeout(); /* Making constant optional checks for safety reasons every 500ms */
}

/******** function ckeckIRSafety
* Purpose
* Calculates if there is an obstacle below the
* predifined limit of IR sensors.
* Results
* - Making an emergency stop.
* - Making an avoidance disengage motion.
********/
bool checkIRSafety() {
  bool left = (digitalRead(IR_LEFT) == LOW);
  bool right = (digitalRead(IR_RIGHT) == LOW);

  if (left || right) {
    /* Double checks the IR sensors due to high sensitivity */
    delay(10); 
    left = (digitalRead(IR_LEFT) == LOW);
    right = (digitalRead(IR_RIGHT) == LOW);
    if (left || right) {
      stopp(); /* Emergency Stop */
      isMoving = false;
      goBack(200); /* Moves backwards for disengage */
      if (left) goTurnR(300); else goTurnL(300); /* Moves away from the Obstacle*/
      return true; /* Update for the emergency stop */
    }
  }
  return false;
}

/******** function setMotorSpeed
* Purpose
* Setting the motors in motion with the predifined speed.
********/
void setMotorSpeed(int s) {
  analogWrite(speedPinL, s);
  analogWrite(speedPinR, s);
}

/******** function startAdvance
* Purpose
* Setting the motion forwards with the predifined speed and duration.
* Keeps tracking of the start up time. 
********/
void startAdvance(int duration) {
  setMotorSpeed(motorSpeed);
  digitalWrite(pinLeftFwd, HIGH);
  digitalWrite(pinRightFwd, HIGH);
  moveStartTime = millis();
  moveDuration = duration;
  isMoving = true;
}

/******** functions stopp(), goBack(), goTurnR(), goTurnL(),
* Purpose
* Setting the vehicle in motion depending on the function.
********/
void stopp() {
  digitalWrite(pinLeftFwd, LOW); digitalWrite(pinLeftBack, LOW);
  digitalWrite(pinRightFwd, LOW); digitalWrite(pinRightBack, LOW);
}

void goBack(int ms) {
  setMotorSpeed(motorSpeed);
  digitalWrite(pinLeftBack, HIGH); digitalWrite(pinRightBack, HIGH);
  delay(ms);
  stopp();
}

void goTurnR(int ms) {
  setMotorSpeed(motorSpeed);
  digitalWrite(pinLeftFwd, HIGH); digitalWrite(pinRightBack, HIGH);
  delay(ms);
  stopp();
}

void goTurnL(int ms) {
  setMotorSpeed(motorSpeed);
  digitalWrite(pinRightFwd, HIGH); digitalWrite(pinLeftBack, HIGH);
  delay(ms);
  stopp();
}

/******** function checkMoveTimeout
* Purpose
* Checking the movement duration. For safety reasons,
* if the duration exceeds the predifined input,
* the vehicle makes a stop for environmental update.
********/
void checkMoveTimeout() {
  /* Compares the predifined duration with the actual one.*/
  if (isMoving && (millis() - moveStartTime >= moveDuration)) {
    stopp();
    isMoving = false;
  }
}

/******** function getDistance
* Purpose
* Emits the Ultrasonic pulse and retunrs the distance from an obstacle.
********/
int getDistance() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 30000); /* Timeout at 30ms */
  int dist = duration * 0.034 / 2;
  /* If pulseIN gets timeout set a large obstacle distance */
  if (dist == 0) {
    return 400;
  } else {
    return dist;
  }
}

/******** function handleObstacleFront()
* Purpose
* Turning the servo - HC-SR04 sensor in Left and Right direction.
* Choosing the clearest patgh depending on the distance of the obstacle.
********/
void handleObstacleFront() {
  stopp();
  isMoving = false;
  
  /* Setting the direction to the servo - HC-SR04*/
  myservo.write(30); delay(400);
  int dRight = getDistance();
  myservo.write(150); delay(400);
  int dLeft = getDistance();
  myservo.write(90); delay(400);
  /* Setting the vehicle in motion through the clearest path*/
  if (dRight > dLeft) goTurnR(400); else goTurnL(400);
}

/******** function updateLCD
* Purpose
* Updating LCD data (distance, STATUS) for dispay
********/
void updateLCD(int dist) {
  lcd.clear(); /* Setting the screen clear from previous data */
  lcd.setCursor(0,0); /* Initialize cursor position in the first line */
  /* Printing Distance data */
  lcd.print("Dist:");
  lcd.print(dist);
  lcd.print("cm");
  lcd.setCursor(0,1); /* Initialize cursor position in the second line */
  /* Printing STATUS*/
  if (digitalRead(IR_LEFT) == LOW || digitalRead(IR_RIGHT) == LOW) {
    lcd.print("STATUS: IR ALERT!");
  } 
  else if(dist < 20) {
    lcd.print("STATUS: Scanning...");
  }
    else {
  lcd.print("STATUS: Moving...");
  }
}