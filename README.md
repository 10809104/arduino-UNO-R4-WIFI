# Arduino UNO R4 WIFI 使用MQTT收發、RFID、電子紙與矩陣燈範例

本專案整合 **RFID 讀卡機** 、 **電子紙顯示器** 與 **內建矩陣燈**，使用 **Arduino UNO R4 WiFi** 來實現 **WiFi 連線**、**MQTT 通訊**，並根據不同的 MQTT 訊息動態更新顯示內容。

---

## 📌 目錄
1. [功能介紹](#功能介紹)  
2. [硬體需求與接線](#硬體需求與接線)  
3. [安裝與設定](#安裝與設定)  
4. [系統運作流程](#系統運作流程)  
5. [必需的 Arduino 庫](#必需的-arduino-庫)  
6. [常見問題](#常見問題)  
7. [授權條款](#授權條款)  

---

## 🎯 功能介紹
- **WiFi 連線**：嘗試連線至 WiFi，並顯示矩陣燈連線動畫。
- **MQTT 通訊**：建立 MQTT 連線，並顯示 MQTT 矩陣燈連線動畫。
- **MQTT 訊息處理**：
  - 根據不同主題 (`screen/in`, `screen/checkout` 等) 顯示對應的內容。
- **RFID 讀卡驗證**：
  - **只有在收到 `checkout` 訊息後，刷 RFID 卡才會生效**，否則顯示「無效操作」。
- **電子紙顯示**：
  - 顯示車牌號碼、時間、金額等資訊。

---

## 🛠 硬體需求與接線

### 硬體需求
- **Arduino UNO R4 WiFi**（或相容開發板）
- **MFRC522 RFID 模組**
- **2.66 吋電子紙顯示器（EPD_2IN66g）**
- **杜邦線與電源供應**

### 電子紙顯示器與 Arduino UNO
| 電子紙 | Arduino UNO |
|--------|------------|
| VCC    | 5V         |
| GND    | GND        |
| DIN (MOSI) | D11     |
| CLK (SCK) | D13      |
| CS     | D10       |
| DC     | D9        |
| RST    | D8        |
| BUSY   | D7        |
| PWR    | D6        |

### RFID 模組與 Arduino UNO
| RFID | Arduino UNO |
|------|------------|
| SDA (SS/CS) | D5  |
| SCK  | D13  |
| MOSI | D11  |
| MISO | D12  |
| IRQ  | (未使用) |
| GND  | GND  |
| RST  | D4   |
| 3.3V | 3.3V |

---

## 🔧 安裝與設定

1. **下載專案**
   ```sh
   git clone https://github.com/10809104/arduino-UNO-R4-WIFI.git
   ```
2. **安裝 Arduino 必要庫**（見下方 [必需的 Arduino 庫](#必需的-arduino-庫)）
3. **依照上方硬體接線圖連接裝置**
4. **開啟 Arduino IDE 並上傳 `.ino` 檔案至 Arduino UNO R4 WiFi**

---

## 🚀 系統運作流程

1. **WiFi 連線**
   - 系統嘗試連線至 WiFi，期間顯示一段 **矩陣動畫**。
2. **MQTT 連線**
   - WiFi 連線成功後，開始連線至 MQTT 伺服器，並顯示另一段 **矩陣動畫**。
3. **MQTT 訊息處理**
   - `screen/in`：顯示車牌號碼。(ex:ABC1234)
   - `screen/invalid`：顯示「無效操作」。
   - `screen/reservation`：顯示預約資訊。(ex:ABC1234)
   - `screen/check`：顯示停車資訊（車牌、時間等）。(ex:2025-01-01 00:00:00 30)
   - `screen/checkout`：請求付款。(ex:30)
   - `screen/remain`：顯示剩餘金額。(ex:770)
   - `screen/out`：還原為初始畫面。
   - `screen/clear`：清除畫面為全白。
4. **RFID 讀取驗證**
   - **只有在收到 `checkout` 訊息後，刷 RFID 卡才會生效。**
   - **若未收到 `checkout` 就刷卡，則會顯示 `screen/invalid`（無效操作）。**

---

## 📌 必需的 Arduino 庫

請確保已安裝以下庫：
- **SPI**（內建）
- **MFRC522**（RFID 讀卡機驅動）
- **PubSubClient**（MQTT 通訊）
- **e-Paper Display Library**（電子紙驅動）

可在 **Arduino IDE → 庫管理員** 安裝這些庫。

📌 **官方電子紙 Wiki & 驅動程式**：  
🔗 [Waveshare 2.66 吋電子紙模組 (G) 官方文件](https://www.waveshare.com/wiki/2.66inch_e-Paper_Module_(G)_Manual#Working_With_Arduino)

---

## ❓ 常見問題

### 1️⃣ WiFi 連線失敗？
- 確保 **WiFi SSID 和密碼** 正確。
- 嘗試靠近路由器。
- 嘗試重新開機 **Arduino**。

### 2️⃣ MQTT 連線失敗？
- 確保 **MQTT 伺服器** 運行中。
- 檢查 **MQTT 配置** 是否正確。
- 嘗試重新開機 **Arduino**。

### 3️⃣ 電子紙沒有顯示內容？
- 檢查電子紙模組的接線。

### 4️⃣ RFID 刷卡無反應？
- 確保 **RFID 模組已連接並且驅動程式安裝完成**。
- 嘗試使用 **不同的 RFID 卡測試**。

### 5️⃣ 為什麼畫面更新速度很慢？
- 電子紙天生更新速度較慢，建議使用適當的延遲時間來避免閃爍問題。
- 若需快速更新，請考慮 部分刷新技術（但需確認驅動庫支援）。

---

## 📜 授權條款

本專案使用 **Kik_Koh License v1.0**，未經授權禁止用於競賽與商業用途。
詳細授權條款請見 LICENSE 文件。

---
<footer>
  <p>2025 © Arduino UNO R4 WIFI範例
</p>
</footer>
