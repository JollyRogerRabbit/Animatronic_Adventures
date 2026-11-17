#include <Arduino.h>
#include <Servo.h>
#include <NewPing.h>

#define trigPin 9
#define echoPin 8
#define maxDistance 300

NewPing sonar(trigPin, echoPin, maxDistance);

unsigned long previousPing = 0;
const long pingInterval = 50; // 50ms delay between pings

unsigned long lastMoveTime = 0;
const unsigned long moveCooldown = 5000; // Wait at least 5 seconds between movements

Servo servo;
int currentAngle = 90; // Track current position for smooth sweeping
int check = 0;

void setup()
{
  Serial.begin(9600);
  servo.attach(2);
  servo.write(currentAngle); // Start facing center
}

// Helper function to move the servo smoothly instead of instantly snapping
void smoothMove(int targetAngle)
{
  if (currentAngle < targetAngle)
  {
    for (int angle = currentAngle; angle <= targetAngle; angle++)
    {
      servo.write(angle);
      delay(10); // Adjust this delay to control speed (higher = slower)
    }
  }
  else
  {
    for (int angle = currentAngle; angle >= targetAngle; angle--)
    {
      servo.write(angle);
      delay(15);
    }
  }
  currentAngle = targetAngle;
}

void loop()
{
  unsigned long currentMillis = millis();

  // Run sensor check at the specified interval
  if (currentMillis - previousPing >= pingInterval)
  {
    previousPing = currentMillis;

    unsigned int uS = sonar.ping_median(5);
    unsigned int distanceCM = sonar.convert_cm(uS);

    if (distanceCM == 0) {
      distanceCM = maxDistance;
    }

    Serial.print(distanceCM);
    Serial.println(" cm");

    // Motion logic with cooldown timer to prevent moving too often
    if (distanceCM < 100 && check == 0 && (currentMillis - lastMoveTime >= moveCooldown))
    {
      int randomBinary = random(0, 2);
      int randomDirection = random(0, 91) + randomBinary * 90; // Random angle (0-180)

      smoothMove(randomDirection); // Moves smoothly

      lastMoveTime = currentMillis; // Reset cooldown timer
      check = 1;
    }
    else if (distanceCM >= 100 && check == 1)
    {
      smoothMove(90); // Return smoothly to center
      check = 0;
      Serial.println("You are out of range");
    }
  }
}