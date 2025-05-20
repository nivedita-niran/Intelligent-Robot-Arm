#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <Servo.h> // Library for Servo motor control

LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust the address if needed
Servo tap_servo;

int trigPin = A0;
int echoPin = A1;
int ir_sensor_pin = 3; // IR sensor connected to pin D3
int tap_servo_pin = 5;
long distance;
long duration;
int ir_state;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600); // For debugging
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ir_sensor_pin, INPUT);
  tap_servo.attach(tap_servo_pin);
  
  lcd.setCursor(0, 0);
  lcd.print("     ROBOT");
  lcd.setCursor(0, 1);
  lcd.print("      ARM");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Measure distance using the ultrasonic sensor
  ULTRASONIC();
  
  // Read IR sensor state and update the display if triggered
  checkIRSensorAndDisplay();
  
  // Display distance on LCD
  if (ir_state == HIGH) { // Only show distance if IR is not triggered
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DIST CM:");
    lcd.print(distance);
    lcd.setCursor(0, 1);
    lcd.print("DIST IN:");
    lcd.print(distance * 0.393701); // Convert cm to inches
  }
  
  // Servo control based on distance or IR sensor state
  if (distance <= 5 || ir_state == LOW) {
    tap_servo.write(90); // Move servo to 90 degrees
  } else {
    tap_servo.write(0); // Reset servo to 0 degrees
  }
  
  delay(500); // Update rate
}

void ULTRASONIC() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Calculate distance in cm

  // Handle out-of-range or no detection values
  if (duration == 0 || distance > 400) {
    distance = 400; // Set to a default max range value to indicate no valid object detected
  }

  Serial.print("Distance: ");
  Serial.println(distance); // For debugging
}

void checkIRSensorAndDisplay() {
  ir_state = digitalRead(ir_sensor_pin); // Read the state of the IR sensor
  
  if (ir_state == LOW) { // IR sensor triggered
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("State: CLOSED");
    lcd.setCursor(0, 1);
    lcd.print("MOVE TO OPEN");
    delay(500); // Display "CLOSED" for a short time
  }
}
