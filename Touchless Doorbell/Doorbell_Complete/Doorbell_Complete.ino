#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>

RTC_DS3231 rtc;
SoftwareSerial gsm(2, 3);

const int trigPin = 9;
const int echoPin = 10;
const int buzzerPin = 8;
const int threshold = 20;

void setup() {
  Serial.begin(9600);
  gsm.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  if (!rtc.begin()) {
    while (1);
  }

  
  rtc.adjust(DateTime(2026, 2, 16, 7, 40, 0));

  delay(3000);
}

void loop() {
  long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  if (distance < threshold && distance > 0) {
    digitalWrite(buzzerPin, HIGH);

    Serial.println("DOORBELL TRIGGERED!");
    Serial.println("Sending SMS...");

    sendSMS();

    Serial.println("SMS Sent!");

    delay(10000);  
    digitalWrite(buzzerPin, LOW);

    delay(10000);  
  }

  delay(100);
}

void sendSMS() {

  DateTime now = rtc.now();

  int hour24 = now.hour();
  String ampm;
  int hour12;

  if (hour24 >= 12) {
    ampm = "PM";
    if (hour24 == 12)
      hour12 = 12;
    else
      hour12 = hour24 - 12;
  } else {
    ampm = "AM";
    if (hour24 == 0)
      hour12 = 12;
    else
      hour12 = hour24;
  }

  String time = String(hour12) + ":" +
                String(now.minute()) + ":" +
                String(now.second()) + " " + ampm;

  String date = String(now.day()) + "/" +
                String(now.month()) + "/" +
                String(now.year());

  gsm.println("AT+CMGS=\"+918657454505\"");
  delay(1000);

  gsm.println("! DOORBELL ALERT !");
  gsm.println("Someone is at your door!");
  gsm.println("");
  gsm.print("Time: ");
  gsm.println(time);
  gsm.print("Date: ");
  gsm.println(date);

  delay(100);
  gsm.write(26);
  delay(5000);
}
