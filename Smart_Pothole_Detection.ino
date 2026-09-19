#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPSPlus.h>

// =====================================================
// SMART POTHOLE DETECTION SYSTEM
// ESP32-CAM + MPU6050 + NEO-6M GPS + Buzzer
// Wi-Fi + Telegram Alert
// =====================================================

// ---------------- Wi-Fi ----------------
// Add your credentials locally before uploading to ESP32.
// NEVER upload real credentials to a public GitHub repository.
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// ---------------- Telegram ----------------
#define BOT_TOKEN "YOUR_TELEGRAM_BOT_TOKEN"
#define CHAT_ID   "YOUR_TELEGRAM_CHAT_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// ---------------- Pin Definitions ----------------

// MPU6050
#define MPU_SDA 13
#define MPU_SCL 15

// NEO-6M GPS
// GPS TX -> ESP32-CAM GPIO14
#define GPS_RX 14

// Buzzer
#define BUZZER_PIN 4

// ---------------- Objects ----------------

Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
HardwareSerial GPSserial(1);

// ---------------- Detection Settings ----------------

// Starting road-shock threshold.
// Tune this value after real road testing.
const float SHOCK_THRESHOLD = 4.0;

// Prevent repeated Telegram alerts.
const unsigned long ALERT_COOLDOWN = 15000;

unsigned long lastAlertTime = 0;

// =====================================================
// SEND TELEGRAM ALERT
// =====================================================

void sendPotholeAlert(float shockValue) {

  String message = "⚠️ ROAD ANOMALY DETECTED!\n\n";

  message += "Shock Value: ";
  message += String(shockValue, 2);
  message += " m/s²\n";

  if (gps.location.isValid()) {

    double latitude = gps.location.lat();
    double longitude = gps.location.lng();

    message += "\n📍 GPS Location\n";

    message += "Latitude: ";
    message += String(latitude, 6);

    message += "\nLongitude: ";
    message += String(longitude, 6);

    message += "\n\nGoogle Maps:\n";
    message += "https://maps.google.com/?q=";
    message += String(latitude, 6);
    message += ",";
    message += String(longitude, 6);

  } else {

    message += "\n📍 GPS fix not available yet.";
  }

  Serial.println();
  Serial.println("Sending Telegram alert...");

  if (bot.sendMessage(CHAT_ID, message, "")) {
    Serial.println("Telegram alert sent successfully!");
  } else {
    Serial.println("Telegram alert failed!");
  }
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==============================");
  Serial.println(" SMART POTHOLE DETECTION SYSTEM");
  Serial.println("==============================");

  // ---------------- Buzzer ----------------

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // ---------------- MPU6050 ----------------

  Serial.println("Starting MPU6050...");

  Wire.begin(MPU_SDA, MPU_SCL);

  if (!mpu.begin()) {

    Serial.println("MPU6050 NOT FOUND!");
    Serial.println("Check SDA, SCL, power and GND.");

    // Do not permanently stop the ESP32.
    // This allows Wi-Fi/GPS debugging to continue.

  } else {

    Serial.println("MPU6050 Connected!");

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }

  // ---------------- GPS ----------------

  Serial.println("Starting GPS...");

  GPSserial.begin(
    9600,
    SERIAL_8N1,
    GPS_RX,
    -1
  );

  Serial.println("GPS Started.");

  // ---------------- Wi-Fi ----------------

  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(ssid, password);

  unsigned long wifiStart = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - wifiStart < 20000) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("Wi-Fi Connected!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Required for Telegram HTTPS connection.
    client.setInsecure();

    bot.sendMessage(
      CHAT_ID,
      "✅ Smart Pothole Detection System Started",
      ""
    );

  } else {

    Serial.println("Wi-Fi connection failed!");
  }

  Serial.println();
  Serial.println("System Ready.");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // ---------------- Read GPS ----------------

  while (GPSserial.available() > 0) {

    gps.encode(GPSserial.read());
  }

  // ---------------- Read MPU6050 ----------------

  sensors_event_t acceleration;
  sensors_event_t gyro;
  sensors_event_t temperature;

  if (mpu.getEvent(
        &acceleration,
        &gyro,
        &temperature)) {

    /*
       Gravity is approximately 9.81 m/s².

       We calculate the magnitude of the
       three-axis acceleration vector.
    */

    float totalAcceleration =
      sqrt(
        acceleration.acceleration.x *
        acceleration.acceleration.x +

        acceleration.acceleration.y *
        acceleration.acceleration.y +

        acceleration.acceleration.z *
        acceleration.acceleration.z
      );

    // Difference from normal gravity.
    float shockValue =
      abs(totalAcceleration - 9.81);

    Serial.print("Shock: ");
    Serial.print(shockValue, 2);
    Serial.print(" m/s²");

    if (gps.location.isValid()) {

      Serial.print(" | GPS: ");
      Serial.print(gps.location.lat(), 6);
      Serial.print(", ");
      Serial.print(gps.location.lng(), 6);

    } else {

      Serial.print(" | GPS waiting...");
    }

    Serial.println();

    // ---------------- Road Shock Detection ----------------

    if (
      shockValue > SHOCK_THRESHOLD &&
      millis() - lastAlertTime > ALERT_COOLDOWN
    ) {

      Serial.println();
      Serial.println("⚠ ROAD ANOMALY DETECTED");

      // Activate buzzer.
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);

      // Send Telegram alert if Wi-Fi is connected.
      if (WiFi.status() == WL_CONNECTED) {

        sendPotholeAlert(shockValue);

      } else {

        Serial.println(
          "Telegram skipped: Wi-Fi disconnected."
        );
      }

      lastAlertTime = millis();
    }
  }

  delay(200);
}
