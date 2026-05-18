#include <Servo.h>

// Define Pins
#define IR_LEFT A1
#define IR_RIGHT A0
#define TRIG_PIN 11
#define ECHO_PIN 12
#define SERVO_PIN 3
#define MOTOR_LEFT1 7
#define MOTOR_LEFT2 8
#define MOTOR_RIGHT1 9
#define MOTOR_RIGHT2 10
#define LED_PARKING 13
#define ENA 5  // Enable pin for left motor
#define ENB 6  // Enable pin for right motor

Servo servoMotor;  // Servo for obstacle detection

void setup() {
    pinMode(IR_LEFT, INPUT);
    pinMode(IR_RIGHT, INPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LED_PARKING, OUTPUT);
    
    pinMode(MOTOR_LEFT1, OUTPUT);
    pinMode(MOTOR_LEFT2, OUTPUT);
    pinMode(MOTOR_RIGHT1, OUTPUT);
    pinMode(MOTOR_RIGHT2, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    servoMotor.attach(SERVO_PIN);
    servoMotor.write(90); // Keep servo straight initially

    analogWrite(ENA, 90);  // Set initial speed (0-255)
    analogWrite(ENB, 90);

    Serial.begin(9600);
}

long getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    
    if (duration == 0) {
        return 999;  // Return a large value if no valid reading
    }

    return duration * 0.034 / 2;  // Convert to cm
}

void setSpeed(int speed) { 
    speed = constrain(speed, 0, 255);  // Ensure speed stays within 0-255
    analogWrite(ENA, speed);
    analogWrite(ENB, speed);
}

void moveForward(int speed = 100) {  
    setSpeed(speed);
    digitalWrite(MOTOR_LEFT1, LOW);
    digitalWrite(MOTOR_LEFT2, HIGH);
    digitalWrite(MOTOR_RIGHT1, HIGH);
    digitalWrite(MOTOR_RIGHT2, LOW);
}

void moveBackward(int speed = 100) {
    setSpeed(speed);
    digitalWrite(MOTOR_LEFT1, LOW);
    digitalWrite(MOTOR_LEFT2, HIGH);
    digitalWrite(MOTOR_RIGHT1, LOW);
    digitalWrite(MOTOR_RIGHT2, HIGH);
}

void turnLeft(int speed = 100) {
    setSpeed(speed);
    digitalWrite(MOTOR_LEFT1, LOW);
    digitalWrite(MOTOR_LEFT2, HIGH);
    digitalWrite(MOTOR_RIGHT1, HIGH);
    digitalWrite(MOTOR_RIGHT2, LOW);
    delay(300);
    stopMotors();
}

void turnRight(int speed = 100) {
    setSpeed(speed);
    digitalWrite(MOTOR_LEFT1, HIGH);
    digitalWrite(MOTOR_LEFT2, LOW);
    digitalWrite(MOTOR_RIGHT1, LOW);
    digitalWrite(MOTOR_RIGHT2, HIGH);
    delay(300);
    stopMotors();
}

void stopMotors() {
    setSpeed(0);  // Stop motors completely
    digitalWrite(MOTOR_LEFT1, LOW);
    digitalWrite(MOTOR_LEFT2, LOW);
    digitalWrite(MOTOR_RIGHT1, LOW);
    digitalWrite(MOTOR_RIGHT2, LOW);
}

void loop() {
    long distance = getDistance();
    int leftSensor = digitalRead(IR_LEFT);
    int rightSensor = digitalRead(IR_RIGHT);

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" cm | Left IR: ");
    Serial.print(leftSensor);
    Serial.print(" | Right IR: ");
    Serial.println(rightSensor);

    // Obstacle Avoidance
    if (distance < 30 && distance > 2) {  
        stopMotors();
        delay(500);
        
        servoMotor.write(0); // Look left
        delay(700); // Allow time for servo to stabilize
        long leftDist = getDistance();
        
        servoMotor.write(180); // Look right
        delay(700); // Allow time for servo to stabilize
        long rightDist = getDistance();
        
        servoMotor.write(90); // Reset to center
        delay(500);

        if (leftDist > rightDist) {
            turnLeft(90);
        } else {
            turnRight(90);
        }
    }  
    // Parking Slot Detection
    else if (leftSensor == 1 && rightSensor == 1) {  
        stopMotors();
        digitalWrite(LED_PARKING, HIGH);
        Serial.println("Parking Complete!");
        while(1);
    } 
    else if((leftSensor==0 && rightSensor==0)||(leftSensor==1 && rightSensor==0)||(leftSensor==0 && rightSensor==1)){
      moveForward(90);
    }
    // Line Following Logic
    /*else if (leftSensor == 0 && rightSensor == 1) {  
        turnLeft(80);
    } 
    else if (leftSensor == 1 && rightSensor == 0) {  
        turnRight(80);
    } 
    else if (leftSensor == 0 && rightSensor == 0) {  
        moveForward(90);  // Adjust speed dynamically
    }*/
}