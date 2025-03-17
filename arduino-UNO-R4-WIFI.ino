/*
 * 目的接收MQTT的訊息顯示對應的東西
 *  默認起始畫面 nothing
 *  無效操作 接收screen/invalid
 *  被預約 
 *  有車進來了 
 *  查詢停車資訊 接收screen/check
 *  應繳費金額 下一步 接收screen/checkout
 *  繳費完成資訊 接收screen/checkout
 * 可以使用rfid
 *  等待收到其他訊號或刷卡
 *    刷卡 傳送-數值
*/
#include <WiFiS3.h>
#include <WiFiSSLClient.h>
// <PubSubClient.h>可以用<ArduinoMqttClient.h>取代，以使用更多高級功能
#include <PubSubClient.h>

#include <SPI.h>
#include <MFRC522.h>

#include "secret.h"
#include "function.h"

#include "Arduino_LED_Matrix.h"   //Include the LED_Matrix library

// 先設定有的沒的
const char ssid[] = SECRET_SSID;        // your network SSID (name)
const char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP)

const char mqtt_server[] = MQTT_SERVER; // MQTT 伺服器
const int mqtt_port = MQTT_PORT;    // MQTT 連接埠
const char mqtt_user[] = MQTT_USER;    // MQTT 使用者名稱
const char mqtt_password[] = MQTT_PASS; // MQTT 使用者密碼

const char sub_topic[]  = "screen/#";
const char pub_topic[] = "screen/rfid";

extern char uid[9];
extern char pay[4];
extern MFRC522 mfrc522;  // Create MFRC522 instance.

bool inLoop = false;         // 是否进入等待循环
char pub[15];

// 📡 使用 WiFiSSLClient 來支援 TLS
WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);

// Create an instance of the ArduinoLEDMatrix class
ArduinoLEDMatrix matrix;

// 📶 連接 WiFi
void setup_wifi() {
    Serial.print(F("🔌 連接 WiFi "));
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(F("."));
    }
    Serial.println(F("\n✅ WiFi 已連接！"));
}

// 🔄 重新連線 MQTT
void reconnect_mqtt() {
    // you can also load frames at runtime, without stopping the refresh
    matrix.loadSequence(LEDMATRIX_ANIMATION);
    matrix.play(true);
    while (!client.connected()) {
        Serial.print(F("🔗 連接 MQTT..."));
        if (client.connect("ArduinoUNO", mqtt_user, mqtt_password)) {
            Serial.println(F(" ✅ 連接成功！"));
            
            // 📌 訂閱主題
            client.subscribe(sub_topic);
            client.subscribe("screen/invalid");
            client.subscribe("screen/check");
            client.subscribe("screen/checkout");
            
            Serial.print(F("📡 已訂閱多個topic "));
            Serial.println(sub_topic);
        } else {
            Serial.print(F(" ❌ 連線失敗，錯誤代碼: "));
            Serial.print(client.state());
            Serial.println(F("\r\n5 秒後重試..."));
            delay(5000);
        }
    }
}

void setup() {
    DEV_Module_Init();

    matrix.begin();
    
    // 📡 連接 WiFi
    setup_wifi();

    // 設定 TLS 憑證
    wifiClient.setCACert(root_ca);
    // 設定 MQTT 伺服器 & 訂閱回呼函數
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    
    reconnect_mqtt();

    matrix.clear(); // Clear the matrix when connected

    // 顯示歐美圖示
    Serial.print(F("e-Paper Init...\r\n"));
    EPD_2IN66g_Init();

    displayImageAndText(NULL, NULL, NULL, NULL, out);
}

void loop() {
    // 確保 MQTT 連線正常
    if (!client.connected()) {
        reconnect_mqtt();
    }
    else {
        matrix.clear(); // Clear the matrix when connected
    }
    client.loop();
    if (inLoop) {
        if (readRFID() == true) {
            memset(pub, 0, sizeof(pub));  // 清空 pub
            strcpy(pub, uid);  // 先将 uid 复制到 pub
            strcat(pub, " ");   // 拼接一个空格
            strcat(pub, pay);   // 拼接 pay
            client.publish(pub_topic, pub);
            // mfrc 522的東西
            inLoop = false;
        }
    }
}
