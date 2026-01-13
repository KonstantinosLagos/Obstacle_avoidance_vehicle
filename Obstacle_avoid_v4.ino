#include <Servo.h>  // Include servo library

// #### 1. Define Pin ####
const int pinLeftFwd = 14;    // Left Motor - Forward (A0)
const int pinLeftBack = 15;   // Left Motor - Backwards (A1)
const int pinRightFwd = 16;   // Right Motor - Forward (A2)
const int pinRightBack = 17;  // Right Motor - Backwards (A3)

// #### Movment Speed Control (PWM) ####
const int speedPinL = 5;  // PWM pin for Left Motor Speed
const int speedPinR = 6;  // PWM pin for Right Motor Speed

const int inputPin = 9;   // Pin Echo
const int outputPin = 8;  // Pin Trig
const int servoPin = 11;  // Pin Servo

// #### 2. Variablres ####
Servo myservo;            // Define Servo object
int delay_time = 200;     // Define Nominal delay time

// Variables for distance buffers
int distanceFwd = 0;      
int distanceRight = 0;    
int distanceLeft = 0;     

int motorSpeed = 130; // Define Nominal Motor speed between (0-255)

// Variables for direction
const int GO_FORWARD = 8;
const int GO_RIGHT = 6;
const int GO_LEFT = 4;
const int GO_BACK = 2;
const int GO_STOP = 0;

int currentDirection = 0; // Initial direction is GO_STOP

// #### 3. Setup ####
void setup() {
  Serial.begin(9600);     // Establish serial communication
  
  // Define Pins ether as INPUTS or OUTPUTS
  pinMode(pinLeftFwd, OUTPUT);
  pinMode(pinLeftBack, OUTPUT);
  pinMode(pinRightFwd, OUTPUT);
  pinMode(pinRightBack, OUTPUT);

  pinMode(speedPinL, OUTPUT);
  pinMode(speedPinR, OUTPUT);
  
  pinMode(inputPin, INPUT);   
  pinMode(outputPin, OUTPUT); 
  
  myservo.attach(servoPin);
  myservo.write(90);    // Initial direction of Servo is FORWARD
}

// #### 4. Loop ####
void loop() {
  myservo.write(90);    // Initial direction of Servo is FORWARD
  delay(100);           // Delay time for Servo adapt
  
  currentDirection = detection();   // Make Decision for current direction
  
  if(currentDirection == GO_BACK) {   // GO back with a slight turn
     back(8);         
     turnL(2);        
     Serial.println("Reverse");
  }
  else if(currentDirection == GO_RIGHT) {   // GO back and turn Right
     back(1);         
     turnR(6);        
     Serial.println("Right Turn");
  }
  else if(currentDirection == GO_LEFT) {  // GO back and TURN left
     back(1);         
     turnL(6);        
     Serial.println("Left Turn");  
  }  
  else if(currentDirection == GO_FORWARD) { // Advance 
    advance(1);       
    Serial.println("Forward movment");
  }
  else if(currentDirection == GO_STOP) {   // Stop
    stopp(1);
  }
}

// #### 5.Decision Making ####
int detection() {      // Direction of movment desition
  distanceFwd = getDistance();  // Calculate the obstacle distance in forward direction 
  
  if(distanceFwd < 10) {       // Emergency Stop
      stopp(1);                
      return GO_BACK; 
  }
       
  if(distanceFwd < 25) {       // Obstacle detection
    stopp(1);                  
    
    myservo.write(177);        // Scan Left
    delay(delay_time);         
    distanceLeft = getDistance();   // Calculate the obstacle distance in Left direction
    
    myservo.write(5);          // Scan Right
    delay(2*delay_time);
    distanceRight = getDistance();  // Calculate the obstacle distance in Right direction
    
    myservo.write(90);    // Set the servo in the forward position
    delay(delay_time);

    // Making Decision depending the most clear path
    if(distanceLeft > distanceRight) {
       return GO_LEFT; 
    }
    else if(distanceLeft <= distanceRight) {
       return GO_RIGHT; 
    } 
    
    if (distanceLeft < 15 && distanceRight < 15) {
       return GO_BACK;     
    }          
  }
  
  return GO_FORWARD; // Continue Forward if there is no obstacle in front
}    

// #### 6. Distance Measurment ####
int getDistance() {
  digitalWrite(outputPin, LOW);   // Sending a Trigger Signal
  delayMicroseconds(2);
  digitalWrite(outputPin, HIGH);  
  delayMicroseconds(10);
  digitalWrite(outputPin, LOW);   
  
  float distance = pulseIn(inputPin, HIGH);   // Measure the time of the Echo Pulse and retrun the roundtrip TIME.
  distance = distance * 0.017;    // Calculate the distance by multipling with TIME return 
  //(Sound speed = 340m/sec = 0.034cm/μsec) Divide by 2 for the single route (0.034/2 = 0.017 cm/μsec)
  return (int)distance;           
}

// #### 7. Movment Functions ####

void setMotorSpeed(int speed) {   // Set the Initial speed selection
  analogWrite(speedPinL, speed);
  analogWrite(speedPinR, speed);
}

void advance(int duration) {    // Set the Motors for Forward motion
  setMotorSpeed(motorSpeed);  
  digitalWrite(pinRightFwd, HIGH);
  digitalWrite(pinRightBack, LOW);
  digitalWrite(pinLeftFwd, HIGH);
  digitalWrite(pinLeftBack, LOW);
  delay(duration * 100);      
}

void turnR(int duration) {    // Set the Motors for Right turn
  setMotorSpeed(motorSpeed);
  digitalWrite(pinRightFwd, HIGH);  
  digitalWrite(pinRightBack, LOW);
  digitalWrite(pinLeftFwd, LOW);   
  digitalWrite(pinLeftBack, HIGH);  
  delay(duration * 50);
}

void turnL(int duration) {    // Set the Motors for Left turn
  setMotorSpeed(motorSpeed);
  digitalWrite(pinRightFwd, LOW);   
  digitalWrite(pinRightBack, HIGH);   
  digitalWrite(pinLeftFwd, HIGH);   
  digitalWrite(pinLeftBack, LOW);
  delay(duration * 50);
}    

void stopp(int duration) {    // Set the Motors in full STOP
  setMotorSpeed(0);           
  digitalWrite(pinRightFwd, LOW);   
  digitalWrite(pinRightBack, LOW);
  digitalWrite(pinLeftFwd, LOW);
  digitalWrite(pinLeftBack, LOW);
  delay(duration * 100);
}

void back(int duration) {     // Set the Motors in reverse
  setMotorSpeed(motorSpeed);
  digitalWrite(pinRightFwd, LOW);  
  digitalWrite(pinRightBack, HIGH);  
  digitalWrite(pinLeftFwd, LOW);  
  digitalWrite(pinLeftBack, HIGH);
  delay(duration * 200);     
}
