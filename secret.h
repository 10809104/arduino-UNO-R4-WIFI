// Both SSID and password must be 8 characters or longer
#define SECRET_SSID "" // WIFI名稱
#define SECRET_PASS "" // WIFI密碼

#define MQTT_SERVER "" // MQTT伺服器
#define MQTT_PORT // MQTT阜

#define MQTT_USER ""// MQTT使用者
#define MQTT_PASS ""// MQTT密碼

#ifndef CONFIG_H
#define CONFIG_H

// 🌍 HiveMQ TLS CA 憑證
const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
這裡放MQTT的CA根憑證for tls加密
-----END CERTIFICATE-----
)EOF";

#endif // CONFIG_H