#include <stdint.h>
#include <PubSubClient.h>
#include "src/Examples/EPD_Test.h"
#include "src/e-Paper/EPD_2in66g.h"

#ifndef MQTT_CALLBACK_H
#define MQTT_CALLBACK_H

extern MFRC522 mfrc522;  // Create MFRC522 instance.
extern PubSubClient client; // PubSubClient 对象的外部声明

extern bool inLoop;         // 是否进入等待循环
extern unsigned long preMillis;  // 上次执行时间

bool readRFID();

void callback(char* topic, uint8_t* payload, unsigned int length);
bool myDelay(unsigned long delayTime);
void displayImageAndText(const char* text1 = NULL, const char* text2 = NULL, const char* text3 = NULL, const char* text4 = NULL, const uint8_t* image = NULL, int xPos = 121, int yPos = 50);
void extractDateTime(const char* message);

#endif
