//lora stuff
#include <SPI.h>
#include <LoRa.h>
#include "SSD1306.h"

SSD1306 display(0x3c, 4, 15);

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6

//neopixel
#include <Adafruit_NeoPixel.h>
#define LED_PIN 32
#define LED_COUNT 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, 32, NEO_GRB + NEO_KHZ800);
const int buttonPin = 17;// button
const int neo = 32; //
const int neoCount = 1; //number of pixels
int counter = 0; //button state counter
int buttonState = 0;

//Vibration motor
int motorPin = 25;

//level of speed in mph to trigger motor
int beginner = 10;
int intermediate = 15;
int expert = 25;
int mphVal = 0;
void setup() {
  //lora
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  // button
  pinMode(buttonPin, INPUT);

  //motor
  pinMode(motorPin, OUTPUT);

  //neopixel
  strip.begin();
  strip.show();
  strip.setBrightness(50);
  Serial.begin(115200);

  //lora initial
  while (!Serial); //if just the the basic function, must connect to a computer
  delay(100);
  Serial.println("Speed Receiver");
  display.drawString(5, 5, "Speed Receiver");
  display.display();
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    display.drawString(5, 25, "Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
  display.drawString(5, 25, "LoRa Initializing OK!");
  display.display();

}

void loop() {
// try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packets
    Serial.print("Received packet. ");
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(3, 0, "Received Speed ");
    display.display();
    // read packet
    while (LoRa.available()) {
    String data = LoRa.readString();
    Serial.print(data);
    display.drawString(20,22, data);
    display.display();
    }
    // print RSSI of packet
    Serial.print(" with RSSI ");
    Serial.println(LoRa.packetRssi());
    display.drawString(20, 45, "RSSI:  ");
    display.drawString(70, 45, (String)LoRa.packetRssi());
    display.display();
  }

//taking the packet and turning it into an int from string
 String data = LoRa.readString();
 mphVal = data.toInt();
  //float mphVal = data.toInt();

  //button
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    counter++;

    delay(150);
  }

  //button states with three colors
  //beginner
  else if (counter == 0) {
    Serial.println("pushed");
    strip.setPixelColor(0, 255, 0, 0);
    strip.show();
    Serial.println(0);
    //vibration motor will trigger if the speed recieved is less then or equal to 12mph
    if (mphVal >= 12.0) {
      digitalWrite(motorPin, HIGH);
      delay(100);
      digitalWrite(motorPin, LOW);
      delay(100);
    }
  }
  //intermediate
  else if (counter == 1) {
    strip.setPixelColor(0, 0, 255, 0);
    strip.show();
    Serial.println(1);
    //vibration motor will trigger if the speed recieved is less then or equal to 17mph
    if (mphVal >= 17.0) {
      digitalWrite(motorPin, HIGH);
      delay(100);
      digitalWrite(motorPin, LOW);
      delay(100);
    }
  }
  //expert
  else if (counter == 2) {
    strip.setPixelColor(0, 0, 0, 255);
    strip.show();
    Serial.print(2);
    if (mphVal >= 27.0) {
      digitalWrite(motorPin, HIGH);
      delay(100);
      digitalWrite(motorPin, LOW);
      delay(100);
    }
  }
  //reset settings
  else {
    counter = 0;
  }
}
