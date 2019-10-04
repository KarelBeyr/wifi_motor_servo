#include <WiFi.h>
#include <Arduino.h>
#include "debugUtils.h"
#include "wifimodule.h"
#include "thingspeak.h"
#include "pass.h"

//http://tomeko.net/online_tools/cpp_text_escape.php?lang=en
const char *webPages[] = {
"",
"<!doctype html>\n"
"<html lang=\"en\">\n"
"  <head>\n"
"    <meta charset=\"utf-8\">\n"
"    <title>Motor PWM control</title>\n"
"  </head>\n"
"  <body>\n"
"    <form action=\"/PWM1_0\" method=\"post\"> <input type=\"submit\" value=\"0%\"></form>\n"
"    <form action=\"/PWM1_33\" method=\"post\"> <input type=\"submit\" value=\"33%\"></form>\n"
"    <form action=\"/PWM1_66\" method=\"post\"> <input type=\"submit\" value=\"66%\"></form>\n"
"    <form action=\"/PWM1_100\" method=\"post\"> <input type=\"submit\" value=\"100%\"></form>\n"

"    <form action=\"/PWM2_0\" method=\"post\"> <input type=\"submit\" value=\"0%\"></form>\n"
"    <form action=\"/PWM2_33\" method=\"post\"> <input type=\"submit\" value=\"33%\"></form>\n"
"    <form action=\"/PWM2_66\" method=\"post\"> <input type=\"submit\" value=\"66%\"></form>\n"
"    <form action=\"/PWM2_100\" method=\"post\"> <input type=\"submit\" value=\"100%\"></form>\n"
"    <br />\n"
"    <p>Built at: \n"
__DATE__
"    @ \n"
__TIME__
"    </p>\n"
"  </body>\n"
"</html>",
"Watering finished",
};

WiFiServer httpServer(80);

void setupWifiServer()
{
    DEBUG_PRINTLN("");
    DEBUG_PRINT("Connecting to ");
    DEBUG_PRINTLN(WifiSsid);

    WiFi.begin(WifiSsid, WifiPassword);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        DEBUG_PRINT(".");
    }

    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("WiFi connected.");
    DEBUG_PRINTLN("IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());

    httpServer.begin();
    IPAddress ip = WiFi.localIP();
}

void setPwm(int pwmDuty, int channel)
{
    DEBUG_PRINT("Setting PWM ");
    DEBUG_PRINT(pwmDuty);
    DEBUG_PRINT(" ");
    DEBUG_PRINTLN(channel);
    ledcWrite(channel, pwmDuty);
}

void maybeServeClient(byte ch0, byte ch1)
{
    bool res = false;
    WiFiClient client = httpServer.available(); // listen for incoming clients
    if (client)
    { // if you get a client,
        DEBUG_PRINTLN("New Client.");
        String currentLine = ""; // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                if (c == '\n')
                { // if the byte is a newline character
                    DEBUG_PRINTLN(currentLine);
                    // if (currentLine.startsWith("GET "))
                    if (currentLine.startsWith("POST /PWM1_0")) setPwm(0, ch0);
                    if (currentLine.startsWith("POST /PWM1_33")) setPwm(300, ch0);
                    if (currentLine.startsWith("POST /PWM1_66")) setPwm(600, ch0);
                    if (currentLine.startsWith("POST /PWM1_100")) setPwm(1023, ch0);

                    if (currentLine.startsWith("POST /PWM2_0")) setPwm(0, ch1);
                    if (currentLine.startsWith("POST /PWM2_33")) setPwm(300, ch1);
                    if (currentLine.startsWith("POST /PWM2_66")) setPwm(600, ch1);
                    if (currentLine.startsWith("POST /PWM2_100")) setPwm(1023, ch1);

                    if (currentLine.startsWith("GET /reboot")) esp_restart_noos();
                    if (currentLine.startsWith("GET /bluescreen"))
                    {
                        while(true) {
                          //lockup to test watchdog
                        }
                    }
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        client.println("HTTP/1.1 200 OK");        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        client.println("Content-type:text/html"); // and a content-type so the client knows what's coming, then a blank line:
                        client.println();
                        client.print(webPages[1]); // the content of the HTTP response follows the header:
                        client.println();                 // The HTTP response ends with another blank line:
                        break;                            // break out of the while loop:
                    }
                    else
                    { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            }
        }
        client.stop(); // close the connection:
        DEBUG_PRINTLN("Client Disconnected.");
    }
}
