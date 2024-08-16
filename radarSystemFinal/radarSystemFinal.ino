#include <Servo.h>
#include <math.h>

// Servo
Servo myservo;

// HC-SR04 ultrasonic sensor
const int trigPin = 9;
const int echoPin = 10;

// Laser
const int laserPin = 12;

long duration;
float distance;
int servoSetting;
bool servoIncreasing = true;

const float MAX_DETECTION_RANGE = 200.0;
const float LASER_ACTIVATION_MIN_RANGE = 1.0;
const float LASER_ACTIVATION_MAX_RANGE = 50.0;

// Ketinggian sensor dari tanah (dalam cm)
const float SENSOR_HEIGHT = 101.0;
// Sudut kemiringan sensor HC-SR04 (dalam derajat)
const float SENSOR_TILT = 45.0;
// Setengah sudut bukaan sensor HC-SR04 (dalam derajat)
const float HALF_SENSOR_ANGLE = 7.5;

void setup() {
  Serial.begin(115200);
  Serial.println("Radar System");

  myservo.attach(11);
  myservo.write(0);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, LOW);
}

bool laserActive = false;
unsigned long laserStartTime = 0;
bool autoMode = false;
bool servoStopped = false;

void loop() {
  getDistance();
  readSerialCommand();
  outputDistance();

  if (distance >= LASER_ACTIVATION_MIN_RANGE && distance <= LASER_ACTIVATION_MAX_RANGE && !laserActive) {
    activateLaser();
    servoStopped = true;
  } else if (laserActive && (millis() - laserStartTime >= 2000)) {
    deactivateLaser();
    servoStopped = false;
  }

  if (!laserActive && !servoStopped) {
    if (autoMode) {
      updateServoAuto();
    }
  }

  delay(50);
}

void activateLaser() {
  laserActive = true;
  laserStartTime = millis();
  digitalWrite(laserPin, HIGH);
  Serial.println("LASER_ACTIVATED");
}

void deactivateLaser() {
  laserActive = false;
  digitalWrite(laserPin, LOW);
  Serial.println("LASER_DEACTIVATED");
}

void getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  if (distance > MAX_DETECTION_RANGE) {
    distance = 0;
  }
}

void readSerialCommand() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "AUTO") {
      autoMode = true;
      laserActive = false;
      digitalWrite(laserPin, LOW);
    } else if (command == "MANUAL") {
      autoMode = false;
      laserActive = false;
      digitalWrite(laserPin, LOW);
    } else {
      int angle = command.toInt();
      if (angle >= 0 && angle <= 180 && !autoMode) {
        myservo.write(angle);
        servoSetting = angle;
      }
    }
  }
}

void outputDistance() {
  float heightFromGround = calculateHeight();
  Serial.print(servoSetting);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(",");
  Serial.println(heightFromGround);
}

float calculateHeight() {
  if (distance == 0) return 0;

  float sensorAngleRad = radians(SENSOR_TILT);
  float halfSensorAngleRad = radians(HALF_SENSOR_ANGLE);

  // Menghitung sudut efektif (sudut sensor + setengah sudut bukaan)
  float effectiveAngleRad = sensorAngleRad + halfSensorAngleRad;

  // Menghitung jarak horizontal dan vertikal
  float horizontalDistance = distance * cos(effectiveAngleRad);
  float verticalDistance = distance * sin(effectiveAngleRad);

  // Menghitung ketinggian relatif terhadap sensor
  float relativeHeight = verticalDistance;

  // Menghitung ketinggian total dari tanah
  float totalHeight = SENSOR_HEIGHT + relativeHeight;

  // Jika objek berada di bawah garis tengah sensor
  if (relativeHeight < 0) {
    totalHeight = SENSOR_HEIGHT - abs(relativeHeight);
  }

  return totalHeight;
}

void updateServoAuto() {
  if (servoIncreasing) {
    servoSetting += 1;
    if (servoSetting >= 180) {
      servoSetting = 180;
      servoIncreasing = false;
    }
  } else {
    servoSetting -= 1;
    if (servoSetting <= 0) {
      servoSetting = 0;
      servoIncreasing = true;
    }
  }
  myservo.write(servoSetting);
}