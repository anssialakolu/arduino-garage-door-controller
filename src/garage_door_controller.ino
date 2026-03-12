// Universal Garage door control system made with an Arduino Nano microcontroller. 
// Originally developed for a Wayne Dalton garage door with a ecostar electric lift mechanism
// The door movement is controlled by a single pushbutton and limited by two limit switches.
// The door can be stopped mid-movement with the button, and its movement direction can also be changed anytime.
// This code includes safety functions to protect the motor and handle error cases.

// Pins
const int button = 5; 
const int lowerLimit = 2;
const int upperLimit = 11;
const int relay1 = 8;
const int relay2 = 7;
const int currentSensor = A0;

// Constants
const int requiredPressTime = 500;
const int maxMoveTime = 18000;       // Change to match the movement time of your door
const int upperMoveExtra = 6000;     // Extra "puffer" because door moves slower when going up
const int pressed = LOW;
const int atBottom = LOW;
const int atTop = LOW;
const int maxCurrent = 5;            // Change to fit your door
const float sensitivity = 0.066;     // (0.066 for 30A sensor), (0.100 for 20A), (0.185 for 5A)
const int measurmentInterval = 500;  // Time difference between current readings
const int numReadings = 1000;        // Number of readings used to calibrate zero current voltage

// Variables
String lastDirection = "stopped";
String currentMovement = "stopped";
int previousState = HIGH;
int currentOverCounter = 0;
unsigned long pressMoment = 0;
unsigned long movementStartTime = 0;
unsigned long previousMillis = 0;
float zeroCurrentVoltage = 0.0;

void setup() {

  // Pin modes
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(lowerLimit, INPUT_PULLUP);
  pinMode(upperLimit, INPUT_PULLUP);

  // Relays initially off
  digitalWrite(relay1, HIGH); 
  digitalWrite(relay2, HIGH); 

  zeroCurrentVoltage = calibrateZeroCurrentVoltage();
}


// Calibrates the zero-current voltage to account for sensor offsets
float calibrateZeroCurrentVoltage() {

  long sum = 0;

  // Calculates the sum of 1000 measurments
  for (int i = 0; i < numReadings; i++) {
    sum += analogRead(currentSensor);
    delay(1);
  }
  
  float averageReading = sum / numReadings;
  float zeroVoltage = averageReading * (5.0 / 1023.0);
  return zeroVoltage;
}


void moveUp() {
  lastDirection = "up";
  currentMovement = "up";
  movementStartTime = millis();
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, HIGH);
}


void moveDown() {
  lastDirection = "down";
  currentMovement = "down";
  movementStartTime = millis();
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, LOW);  
}


void stop() {
  currentMovement = "stopped";
  movementStartTime = 0;
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
}


// Reads if the button has been pressed for the specified time and returns LOW if it has.
// This could be replaced by a simple digitalRead in the loop, but I decided to go this route because
// significant interference caused by fluorescent lights in the garage sometimes caused the door to open by itself.
int readButton() {

  int buttonStatus = digitalRead(button);

  // If the button is pressed and the state has changed.
  if (buttonStatus == pressed && buttonStatus != previousState) {
    pressMoment = millis(); 
    previousState = buttonStatus;
  }

  // If the button is released and the state has changed.
  else if (buttonStatus != pressed && buttonStatus != previousState) {
    previousState = buttonStatus; 
    pressMoment = 0;
  }

  // Returns LOW if the button has been pressed for the specified time.
  if (pressMoment > 0 && (millis() - pressMoment >= requiredPressTime) && buttonStatus == pressed) {
    pressMoment = 0;
    return pressed;
  } 
  return HIGH; 
}


void intermediatePosition() {
  // If the button is pressed while the door is moving, the door stops
  if (currentMovement != "stopped") {                                                         
    stop();
  }
  else {
    // If the door was stopped while moving up, it starts moving down when button is pressed
    if (lastDirection == "up") {                                             
      moveDown();
    }
    // If the door was stopped while moving down, it starts moving up when button is pressed
    // The door also starts moving up when the button is pressed if the system is powered on while the door is in an intermediate position.
    else if (lastDirection == "stopped" || lastDirection == "down") {
      moveUp();
    }
  }
}


// Prevents motor movement and possible damage in case of a malfunction.
// Stops the motor if one of the limit switches is stuck.
// Stops the motor if it has been running for more than the specified time. 
// Has different times for up and down movement because of speed difference.
void malfunctionProtection(){

  // Stops the door if both limit switches are pressed at the same time.
  if (digitalRead(lowerLimit) == atBottom && digitalRead(upperLimit) == atTop){
    stop();
  }
  // Limits the door's upward movement time.
  else if (currentMovement == "up" && (millis() - movementStartTime > maxMoveTime + upperMoveExtra)){
    stop();
  }
  // Limits the door's downward movement time.
  else if (currentMovement == "down" && (millis() - movementStartTime > maxMoveTime)){
    stop();
  }
}


// Protects the motor by measuring the current and stopping it if a set current is exceeded for a set time
void checkCurrent() {

  unsigned long currentMillis = millis();

  // Checks if the time since the last measurement exceeds the defined interval
  if (currentMillis - previousMillis >= measurmentInterval) {

    previousMillis = currentMillis;
    int analogValue = analogRead(currentSensor);
    
    // Converts the analog sensor reading to voltage
    float sensorVoltage = analogValue * (5.0 / 1023.0);
    
    // Converts voltage to current
    float current = (sensorVoltage - zeroCurrentVoltage) / sensitivity;

    // If the motor has been running for more than 2 seconds and the current exceeds the maximum allowed, updates counter
    if(millis() - movementStartTime > 2000 && abs(current) > maxCurrent){
      currentOverCounter += 1;
    }
    else{
      currentOverCounter = 0;  // Reset the counter if no overcurrent condition
    }
  }

  // Stops the motor if the overcurrent condition persists for two consecutive intervals
  if(currentOverCounter >= 2){
    stop();
  }
}


void loop()  {

  int buttonState = readButton();
  int lowerLimitState = digitalRead(lowerLimit);
  int upperLimitState = digitalRead(upperLimit);
  malfunctionProtection();
  checkCurrent();

  // The door stops when it is at the top
  if (currentMovement == "up" && upperLimitState == atTop){
    stop();
  }
  // The door stops when it is at the bottom
  else if (currentMovement == "down" && lowerLimitState == atBottom){                                                   
    stop();
  }

  // If the door is at the bottom when the button is pressed, it starts moving up
  if (buttonState == pressed && lowerLimitState == atBottom){
    moveUp();   
  }

  // If the door is at the top when the button is pressed, it starts moving down
  else if (buttonState == pressed && upperLimitState == atTop) {                                                           
    moveDown();
  }

  // If the button is pressed when the door is in an intermediate position
  else if (buttonState == pressed && upperLimitState != atTop && lowerLimitState != atBottom) {
    intermediatePosition();
  }
}
