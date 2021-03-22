//lora
#include <SPI.h>
#include <LoRa.h>
#include "SSD1306.h"
#include <Arduino.h>

SSD1306  display(0x3c, 4, 15);

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 

int hallSens = 37 ;
int led =  17;
int state = 0;
float hallThresh = 10000.0;
float Pi = 3.14159;

//values for hall sens
float hallCount = 0;
float start = micros();
bool isOn = false;

void setup() {
  //board and lcd
  pinMode(25, OUTPUT); //Send success, LED will bright 1 second
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH);

  // sensor and led
  pinMode(led, OUTPUT);
  pinMode(hallSens, INPUT);
  Serial.begin(115200);

  //lcd
  while (!Serial); //If just the the basic function, must connect to a computer
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(5, 5, "LoRa Sender");
  display.display();
  //lora
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
  display.drawString(5, 20, "LoRa Initializing OK!");
  display.display();
  delay(10);
  //option 1
  //  Serial.println("LoRa Sender Starting...");
  //
  //  LoRa.setPins(18, 14, 26); //LoRa.setPins(CS, RESET, IRQ); Lora
  //  if (!LoRa.begin(868E6)) { //set frequency to 433, 868 or 915 MHz using 433E6, 868E6, or 915E6
  //    Serial.println("could not find a valid LoRa transceiver, check pins used and wiring");
  //    delay(100);

  //option 2
  //Serial.println("LoRa Sender starting...");
  //  LoRa.setPins(18, 14, 26); // LoRa.setPins(CS, RESET, IRQ);
  //  if (!LoRa.begin(868E6)) { // Set frequency to 433, 868 or 915MHz
  //    Serial.println("Could not find a valid LoRa transceiver, check pins used and wiring!");
  //    delay(1000);
  //  }
  //  bool status = bme.begin();
  //  if (!status) {
  //    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  //    delay(1000);
  //  }
  //  Serial.println("Sending data packet...");
  //  Send_and_Display_Sensor_Data();
  //  start_sleep();
}

void loop() {

  //  state = digitalRead(hallSens);
  //
  //  if (state == LOW) {
  //    digitalWrite(led, HIGH);
  //    //Serial.println("0");
  ////    ++hallCount;
  //    hallCount++;
  //    isOn = true;
  //  }
  //  else {
  //    digitalWrite(led, LOW);
  //    //Serial.println("1");
  //    isOn = false;
  //    //    ++hallCount;
  //    //    hallCount++;
  //  }
  //  if (hallCount > hallThresh) { //if hallCount becomes larger than the threshold it will revert to zero
  //    float hallCount = 0;
  //  }

  while (true) {
    if (digitalRead(hallSens) == 0) {
      if (state == false) {
        isOn = true;
        hallCount++;
        digitalWrite(led, HIGH);
      }
    } else {
      isOn = false;
      digitalWrite(led, LOW);
      
    }

    if (hallCount >= hallThresh) {
      break;
    }
  }

  //prints time and rpm
  float endTime = micros();
  float timePassed = ((endTime - start) / 1000000.0);
  float rpmVal = (hallCount / timePassed) * 60.0;
  float mphVal = rpmVal * 3.14159 * 5 / 30960.0;                            //((rpmVal * wheelDia * Pi) * (60 / 63360));
  //Serial.print(timePassed);
  //Serial.print(" sec");
  //Serial.print(rpmVal);
  //Serial.print(" rpm");
  Serial.println(mphVal);
  //Serial.print(" mph");

  //lora
  //lcd and serial port
  Serial.print("Sending Speed: ");
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(3, 5, "Sending Speed ");
  display.drawString(50, 30, String(mphVal));
  display.display();

  //sending packet
  LoRa.beginPacket();
  LoRa.print("MPH: " + String(mphVal));
  LoRa.endPacket();

  //LoRa.sleep(); //puts lora receiver to sleep probably dont need
  digitalWrite(25, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(10);                       // wait for a second
  digitalWrite(25, LOW);    // turn the LED off by making the voltage LOW
  delay(10);                       // wait for a second
  //delay(300);


}
