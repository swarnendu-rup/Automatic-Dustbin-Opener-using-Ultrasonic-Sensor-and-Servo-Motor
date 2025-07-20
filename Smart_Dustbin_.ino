#include <LiquidCrystal.h>
#include <Servo.h>

// Initialize LCD pins (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Initialize Servo
Servo servoMotor;

// Ultrasonic sensor pins (lid operation)
const int trigPin = 8;
const int echoPin = 9;

// Ultrasonic sensor pins (bin fullness detection)
const int fullTrigPin = 6;
const int fullEchoPin = 7;

// PIR Motion Sensor
const int pirPin = 13;

// Buzzer and LED pins
const int buzzerPin = A0;
const int greenLEDPin = A1; // Ready
const int redLEDPin = A2;   // Full
const int blueLEDPin = A3;  // Opening

// Variables for ultrasonic sensor
long duration, fullDuration;
int distance, fullDistance;

// Timers for power-saving mode
unsigned long lastActivity = 0;
const unsigned long timeout = 20000; // 20 seconds of inactivity

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Hello! <_>");
  lcd.setCursor(0, 1);
  lcd.print("I, Smart Dustbin");

  // Set up pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(fullTrigPin, OUTPUT);
  pinMode(fullEchoPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);

  // Attach servo motor
  servoMotor.attach(10);
  servoMotor.write(0); // Initial position

  // Set initial LED states
  digitalWrite(greenLEDPin, HIGH); // Ready
  digitalWrite(redLEDPin, LOW);
  digitalWrite(blueLEDPin, LOW);

  delay(2000); // Display initial message for 2 seconds
}

void loop() {
  // Check motion detection
  if (digitalRead(pirPin) == HIGH) {
    lastActivity = millis(); // Reset inactivity timer
    digitalWrite(greenLEDPin, HIGH); // Indicate readiness

    // Measure fullness
    fullDistance = measureDistance(fullTrigPin, fullEchoPin);
    if (fullDistance < 5) { // Bin is full
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bin Full!");
      lcd.setCursor(0, 1);
      lcd.print("Empty Me!");
      digitalWrite(redLEDPin, HIGH); // Indicate full bin
      digitalWrite(greenLEDPin, LOW);
      digitalWrite(blueLEDPin, LOW);
      tone(buzzerPin, 1000, 500); // Buzz for half a second
      delay(2000); // Wait before rechecking
      return; // Prevent further operation
    }

    // Measure distance to open lid
    distance = measureDistance(trigPin, echoPin);
    if (distance < 10) {
      openLid();
    }
  }

  // Power-saving mode
  if (millis() - lastActivity > timeout) {
    digitalWrite(greenLEDPin, LOW);
  }
}

// Function to measure distance
int measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}

// Function to open lid
void openLid() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Throw It!");
  digitalWrite(blueLEDPin, HIGH); // Indicate lid opening
  servoMotor.write(90); // Open lid
  delay(5000); // Wait 5 seconds
  servoMotor.write(0); // Close lid
  digitalWrite(blueLEDPin, LOW); // Lid closed

  // Display "It's Tasty"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("It's Tasty ^_^");
  lcd.setCursor(0, 1);
  lcd.print("Give me more!");
  delay(10000); // Wait 10 seconds

  // Return to default message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello! <_>");
  lcd.setCursor(0, 1);
  lcd.print("I, Smart Dustbin");
}
