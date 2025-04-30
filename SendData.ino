#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <ESP32Time.h>
#include <Wire.h>

// WiFi credentials
const char* ssid = "";
const char* password = "";

// Firebase config
const char* apiKey = "AIzaSyCi0dg2iJAnfCY";
const char* projectId = "weather-cb";
const char* documentPath = "5726/a1";  // collection/document

// DHT config
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

ESP32Time rtc;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - startAttemptTime >= 60000) {
      Serial.println("Unable to connect to Wi-Fi. Retrying...");
      ESP.restart();  // Instead of deep sleep, restart and retry
    }
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo);
    Serial.println("Time synced with NTP");
  } else {
    Serial.println("Failed to sync time");
  }
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(30000);  // Retry in 30s
    return;
  }

  String humidity_quality;
  if (humidity <= 20) humidity_quality = "Very Dry";
  else if (humidity <= 35) humidity_quality = "Dry";
  else if (humidity <= 50) humidity_quality = "Normal";
  else if (humidity <= 65) humidity_quality = "Slightly Humid";
  else if (humidity <= 75) humidity_quality = "Humid";
  else if (humidity <= 85) humidity_quality = "Very Humid";
  else humidity_quality = "Saturated";

  DynamicJsonDocument doc(512);
  doc["fields"]["temprature"]["doubleValue"] = temperature;
  doc["fields"]["humidity"]["doubleValue"] = humidity;
  doc["fields"]["humidity_quality"]["stringValue"] = humidity_quality;
  doc["fields"]["time"]["stringValue"] = getFormattedTime();

  String jsonData;
  serializeJson(doc, jsonData);

  String url = "https://firestore.googleapis.com/v1/projects/";
  url += projectId;
  url += "/databases/(default)/documents/";
  url += documentPath;
  url += "?key=";
  url += apiKey;

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PATCH(jsonData);

  Serial.print("Sending data... ");
  Serial.println(jsonData);
  Serial.print("HTTP Response Code: ");
  Serial.println(httpCode);
  String payload = http.getString();
  Serial.println(payload);

  http.end();

  // Wait for 1 minute
  Serial.println("Waiting 1 minute...");
  delay(60000);
}

// Helper functions remain unchanged
String getFormattedTime() {
  int hour24 = rtc.getHour(true);
  int hour12 = hour24 % 12;
  if (hour12 == 0) hour12 = 12;
  String meridiem = (hour24 >= 12) ? "PM" : "AM";
  String dayOfWeek = getDayOfWeek(rtc.getDayofWeek());
  String month = getMonthString(rtc.getMonth());
  int day = rtc.getDay();
  String suffix = getDaySuffix(day);
  String minuteStr = (rtc.getMinute() < 10) ? "0" + String(rtc.getMinute()) : String(rtc.getMinute());

  return String(hour12) + ":" + minuteStr + " " + meridiem + "  " + dayOfWeek + ", " +
         String(day) + suffix + " " + month + " " + String(rtc.getYear());
}

String getDaySuffix(int day) {
  if (day >= 11 && day <= 13) return "th";
  switch (day % 10) {
    case 1: return "st";
    case 2: return "nd";
    case 3: return "rd";
    default: return "th";
  }
}

String getDayOfWeek(int dayOfWeek) {
  switch (dayOfWeek) {
    case 0: return "Sun";
    case 1: return "Mon";
    case 2: return "Tue";
    case 3: return "Wed";
    case 4: return "Thu";
    case 5: return "Fri";
    case 6: return "Sat";
    default: return "Invalid";
  }
}

String getMonthString(int month) {
  switch (month) {
    case 0: return "January";
    case 1: return "February";
    case 2: return "March";
    case 3: return "April";
    case 4: return "May";
    case 5: return "June";
    case 6: return "July";
    case 7: return "August";
    case 8: return "September";
    case 9: return "October";
    case 10: return "November";
    case 11: return "December";
    default: return "Invalid";
  }
}
