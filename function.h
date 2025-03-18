#include <stdint.h>
#include <PubSubClient.h>
#include "src/Examples/EPD_Test.h"
#include "src/e-Paper/EPD_2in66g.h"

#ifndef MQTT_CALLBACK_H
#define MQTT_CALLBACK_H

extern MFRC522 mfrc522;  // Create MFRC522 instance.
extern PubSubClient client; // PubSubClient 对象的外部声明

extern bool inRFID;         // 是否进入等待循环
extern unsigned long preMillis;  // 上次执行时间

bool readRFID();

void callback(char* topic, uint8_t* payload, unsigned int length);
bool myDelay(unsigned long delayTime);
void displayImageAndText(const char* text1 = NULL, const char* text2 = NULL, const char* text3 = NULL, const char* text4 = NULL, const uint8_t* image = NULL, int xPos = 121, int yPos = 50);
void extractDateTime(const char* message);

constexpr uint32_t LEDMATRIX_ANIMATION[][4] = {
  { 0x0, 0x0, 0x0, 66 },
  { 0x0, 0x800, 0x0, 66 },
  { 0x0, 0x801, 0x0, 40 },
  { 0x0, 0x803, 0x0, 40 },
  { 0x0, 0x807, 0x0, 40 },
  { 0x0, 0x8807, 0x0, 40 },
  { 0x0, 0x8008807, 0x0, 40 },
  { 0x80, 0x8008807, 0x0, 40 },
  { 0x40080, 0x8008807, 0x0, 30 },
  { 0x60080, 0x8008807, 0x0, 30 },
  { 0x70080, 0x8008807, 0x0, 30 },
  { 0x70088, 0x8008807, 0x0, 30 },
  { 0x70088, 0x8408807, 0x0, 30 },
  { 0x70088, 0x8408a07, 0x0, 30 },
  { 0x70088, 0x8408a07, 0x10000000, 30 },
  { 0x70088, 0x8408a07, 0x18000000, 30 },
  { 0x70088, 0x8408a07, 0x1c000000, 30 },
  { 0x70088, 0x8408a27, 0x1c000000, 40 },
  { 0x70088, 0x8428a27, 0x1c000000, 40 },
  { 0x70088, 0x28428a27, 0x1c000000, 40 },
  { 0x70488, 0x28428a27, 0x1c000000, 40 },
  { 0x70c88, 0x28428a27, 0x1c000000, 40 },
  { 0x71c88, 0x28428a27, 0x1c000000, 66 },
  { 0x71c8a, 0x28428a27, 0x1c000000, 240 },
  { 0x71d8a, 0xa842aa27, 0x1c000000, 66 },
  { 0x171c8a, 0x2a4a8a27, 0x1c000000, 400 },
};

#endif
