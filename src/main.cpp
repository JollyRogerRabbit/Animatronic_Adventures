#include <Arduino.h>
#include <Servo.h>
#include <NewPing.h>

#define trigPin 9
#define echoPin 8
#define maxDistance 300

NewPing sonar(trigPin, echoPin, maxDistance);

unsigned long previousPing = 0;
const long pingInterval = 100; // 50ms delay between pings

Servo servo;
int check = 0;

void setup()
{
    Serial.begin(115200);
    servo.attach(2);
}

void loop()
{
    unsigned long currentMillis = millis();

    // Run the sensor check at the specified interval (non-blocking)
    if (currentMillis - previousPing >= pingInterval)
    {
        previousPing = currentMillis;

        // Get distance using NewPing's median filter
        unsigned int uS = sonar.ping_median(5);
        unsigned int distanceCM = sonar.convert_cm(uS);

        // If out of range or no echo received, NewPing returns 0.
        // Treat 0 as max distance so it doesn't trigger false positives.
        if (distanceCM == 0)
        {
            distanceCM = maxDistance;
        }

        Serial.print(distanceCM);
        Serial.println(" cm");

        // Motion logic
        if (distanceCM < 300 && check == 0)
        {
            int randomBinary = random(0, 2);
            int randomDirection = random(0, 91) + randomBinary * 90; // Random direction (e.g., 0-90 or 90-180)

            servo.write(randomDirection);
            check = 1;
        }
        else if (distanceCM >= 300)
        {
            servo.write(90);
            check = 0;
            Serial.println("You are out of range");
        }
    }
}