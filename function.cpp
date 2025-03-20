/*
 * This software is licensed under the Kik_Koh License v1.0
 * Unauthorized use in competitions or commercial applications is strictly prohibited.
 * See LICENSE for details.
 */
#include <Arduino.h>
#include "function.h"

char uid[9]; // rfid卡片的uid
char plate[8]; // 用来保存收到的消息
char re_plate[8]; // 用来保存收到的消息
char pay[4]; // 用来保存收到的消息

// 创建临时的 char 数组来存储拆分的日期时间部分
char dateStr[11];  // "YYYY-MM-DD" 的长度
char timeStr[9];   // "HH:MM:SS" 的长度
char numberStr[4]; // 假设数字部分最大是两位数

unsigned long preMillis = 0;  // 上次执行时间

bool readRFID() {
    if (mfrc522.PICC_IsNewCardPresent()) {
        if (mfrc522.PICC_ReadCardSerial()) {
            memset(uid, 0, sizeof(uid));  // 清空 uid 数组，防止残留数据
            // Read UID and display it in the Serial Monitor
            for (byte i = 0; i < mfrc522.uid.size; i++) {
                // 使用 sprintf 将每个字节转换为两位十六进制字符，并将其存储到 uid 数组中
                sprintf(uid + i * 2, "%02X", mfrc522.uid.uidByte[i]);  
            }
            
            Debug("RFID UID:  ");
            Debug(uid);  // Print UID to the serial monitor
            Debug("\r\n");
            
            mfrc522.PICC_HaltA();   // Halt the card
            mfrc522.PCD_StopCrypto1(); // End encryption
            return true;  // 成功讀取卡片
        }
    }
    return false;  // 沒有讀取到卡片
}

void callback(char* topic, uint8_t* payload, unsigned int length) {
    Serial.print(F("📩 收到訊息 - 主題: "));
    Serial.println(topic);

    char message[length + 1];  // +1 是为了存储 '\0' 结尾符
    Serial.print(F("📥 訊息內容: "));
    for (int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';  // 确保字符串结尾符
    Serial.println(message);

    // 根據不同主題進行處理
    if (strcmp(topic, "screen/in") == 0) {
        Serial.println(F("顯示車牌號碼"));  // 有變數
        strncpy(plate, message, sizeof(plate) - 1);  // 复制内容，避免溢出
        plate[sizeof(plate) - 1] = '\0';  // 确保字符串以 null 结尾
        displayImageAndText(plate, NULL, NULL, NULL, in);  // 显示车牌和图片
    } else if (strcmp(topic, "screen/invalid") == 0) {
        Serial.println(F("是無效操作")); // 無變數
        displayImageAndText(NULL, NULL, NULL, NULL, invalid);  // 显示无效操作的文字和图片
        if (!client.connected()){
          DEV_Delay_ms(2000);
        } else{
          preMillis = millis();
          while(myDelay(2000) == false);
        }
        if (plate[0] != '\0') {
            displayImageAndText(plate, NULL, NULL, NULL, in);  // 只显示图片
        } else if (re_plate[0] != '\0') {
            displayImageAndText(re_plate, NULL, NULL, NULL, reservation);  // 只显示图片
        } else {
            displayImageAndText(NULL, NULL, NULL, NULL, out);  // 只显示图片
        }
    } else if (strcmp(topic, "screen/reservation") == 0) {
        Serial.println(F("已預約")); // 無變數
        strncpy(re_plate, message, sizeof(re_plate) - 1);  // 复制内容，避免溢出
        plate[sizeof(re_plate) - 1] = '\0';  // 确保字符串以 null 结尾
        displayImageAndText(re_plate, NULL, NULL, NULL, reservation);  // 显示预约的文字和图片
    } else if (strcmp(topic, "screen/check") == 0) {
        Serial.println(F("顯示停車資訊")); // 有變數
        extractDateTime(message);
        displayImageAndText(plate, dateStr, timeStr, numberStr, check_2, 90,10);  // 显示停车信息（车牌号和图片）
    } else if (strcmp(topic, "screen/checkout") == 0) {
        Serial.println(F("請逼卡")); // 有變數
        strncpy(pay, message, sizeof(pay) - 1);  // 复制内容，避免溢出
        pay[sizeof(pay) - 1] = '\0';  // 确保字符串以 null 结尾
        displayImageAndText(plate, NULL, NULL, pay, checkout, 90, 10);  // 显示支付信息和图片
        // mfrc 522的東西
        inRFID = true;
    } else if (strcmp(topic, "screen/remain") == 0) {
        Serial.println(F("顯示還剩多少錢")); // 有變數
        displayImageAndText(pay, NULL, message, NULL, remain, 150, 60);  // 显示剩余金额和车牌信息
    } else if (strcmp(topic, "screen/out") == 0) {
        Serial.println(F("顯示停車資訊")); // 無變數
        memset(plate, 0, sizeof(plate));  // 清空 plate
        memset(re_plate, 0, sizeof(re_plate));  // 清空 re_plate
        memset(uid, 0, sizeof(uid));  // 清空 uid
        memset(pay, 0, sizeof(pay));  // 清空 pay
        displayImageAndText(NULL, NULL, NULL, NULL, out);  // 只显示图片，不显示文本
    }else if (strcmp(topic, "screen/clear") == 0) {
        Serial.println(F("清除為全白畫面")); // 無變數
        EPD_2IN66g_Clear(EPD_2IN66g_WHITE);  // 清除屏幕
    }
    else {
        return;
    }
}

bool myDelay(unsigned long delayTime) {
    unsigned long currentMillis = millis();
    
    // 如果延迟时间到了，返回
    if (currentMillis - preMillis >= delayTime) {
        preMillis = currentMillis; // 更新上次执行的时间
        return true;
    } else {
        // 在延迟期间继续执行其他任务
        client.loop(); // 调用 client.loop() 来处理 MQTT 或其他任务
        return false;
    }
}

void displayImageAndText(const char* text1, const char* text2, const char* text3, const char* text4, const uint8_t* image, int xPos, int yPos) {
    // DEV_Module_In();
    // EPD_2IN66g_Init();

    SWITCH_SPI_SS();
    
    // 计算并申请内存
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2IN66g_WIDTH % 4 == 0) ? (EPD_2IN66g_WIDTH / 4) : (EPD_2IN66g_WIDTH / 4 + 1)) * EPD_2IN66g_HEIGHT;
    
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Serial.print(F("Failed to apply for black memory...\r\n"));
        return;
    }
    // 初始化显示器
    Paint_NewImage(BlackImage, EPD_2IN66g_WIDTH, EPD_2IN66g_HEIGHT, 270, WHITE);
    Paint_SetScale(4);
    Paint_SelectImage(BlackImage);

    // 绘制图片，如果有提供图片数据
    if (image != NULL) {
        Paint_DrawBitMap(image);
    }

    // 绘制第一个文本
    if (text1 != NULL) {
        Paint_DrawString_EN(xPos, yPos, text1, &Font24, EPD_2IN66g_WHITE, EPD_2IN66g_BLACK);
        yPos += 70;  // 更新y坐标，显示第二行
    }

    // 绘制第二个文本
    if (text2 != NULL) {
        Paint_DrawString_EN(xPos, yPos, text2, &Font24, EPD_2IN66g_WHITE, EPD_2IN66g_BLACK);
        xPos += 40;  // 更新x坐标，显示第三行
        yPos += 25;  // 更新y坐标，显示第三行
    }

    // 绘制第三个文本
    if (text3 != NULL) {
        if (text2 != NULL) {
            Paint_DrawString_EN(xPos, yPos, text3, &Font24, EPD_2IN66g_WHITE, EPD_2IN66g_BLACK);
            xPos += 15;  // 更新x坐标，显示第四行
            yPos += 25;  // 更新y坐标，显示第四行
        } else {
            yPos -= 35;  // 更新y坐标，显示第三行
            Paint_DrawString_EN(xPos, yPos, text3, &Font24, EPD_2IN66g_WHITE, EPD_2IN66g_BLACK);
        }
    }

    // 绘制第四个文本
    if (text4 != NULL) {
        if (text2 != NULL) {
            Paint_DrawString_EN(xPos, yPos, text4, &Font24, EPD_2IN66g_WHITE, EPD_2IN66g_BLACK);
        } else {
            xPos += 55;  // 更新x坐标，显示第四行
            yPos -= 20;  // 更新y坐标，显示第四行
            Paint_DrawString_EN(xPos, yPos, text4, &Font24, EPD_2IN66g_WHITE, EPD_2IN66g_RED);
        }
    }

    // 显示内容
    EPD_2IN66g_Display(BlackImage);
    // while(myDelay(2000) == false); // 显示2秒钟
    // EPD_2IN66g_Sleep();
    free(BlackImage);
    BlackImage = NULL;
    // DEV_delay_ms(2000);
    // while(myDelay(2000) == false); // 至少等待2秒
    // Serial.print(F("close 5V, Module enters 0 power consumption ...\r\n"));
    // DEV_Module_Exit();
}


void extractDateTime(const char* message) {
    // 查找日期部分的起始位置
    const char* dateStart = strstr(message, "20");  // 假设日期是以 "20" 开头的
    if (dateStart == NULL) {
        Serial.println(F("No date found"));
        return;  // 如果没有找到日期，返回
    }

    // 查找日期部分的结束位置
    const char* dateEnd = strchr(dateStart, ' ');  // 日期后会有一个空格
    if (dateEnd == NULL) {
        Serial.println(F("No space found for date"));
        return;  // 如果没有找到空格，返回
    }

    size_t dateLength = dateEnd - dateStart;  // 计算日期部分的长度
    strncpy(dateStr, dateStart, dateLength);  // 拷贝日期部分到 dateStr
    dateStr[dateLength] = '\0';  // 确保以 null 结尾

    // 查找时间部分的结束位置（时间后会有一个空格）
    const char* timeStart = dateEnd + 1;  // 跳过日期部分的空格，指向时间部分
    const char* timeEnd = strchr(timeStart, ' ');  // 查找空格，时间结束于此
    if (timeEnd == NULL) {
        timeEnd = message + strlen(message);  // 如果没有找到空格，时间就是消息的结尾
    }

    size_t timeLength = timeEnd - timeStart;  // 计算时间部分的长度
    strncpy(timeStr, timeStart, timeLength);  // 拷贝时间部分到 timeStr
    timeStr[timeLength] = '\0';  // 确保以 null 结尾

    // 获取数字部分
    const char* numberStart = timeEnd + 1;  // 跳过空格，指向数字部分
    strncpy(numberStr, numberStart, sizeof(numberStr) - 1);  // 拷贝数字部分到 numberStr
    numberStr[sizeof(numberStr) - 1] = '\0';  // 确保以 null 结尾

    // 输出拆分后的结果
    Serial.print(F("Date: "));
    Serial.println(dateStr);  // 输出日期
    Serial.print(F("Time: "));
    Serial.println(timeStr);  // 输出时间
    Serial.print(F("Number: "));
    Serial.println(numberStr);  // 输出数字
}
