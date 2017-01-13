/* Install any missing libraries using Arduino Library Manager */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>    
#include <WebSocketsServer.h>
#include <Hash.h>
#include <WiFiUdp.h>
#include "ws2812_i2s.h"

// Set this to the number of LEDs in your LED strip
#define NUM_LEDS 60
// Maximum number of packets to hold in the buffer. Don't change this.
#define BUFFER_LEN 1024
// Toggles FPS output (1 = print FPS over serial, 0 = disable output)
#define PRINT_FPS 1

// Wifi and socket settings
unsigned int localPort = 7777;
char packetBuffer[BUFFER_LEN];

static WS2812 ledstrip;
static Pixel_t pixels[NUM_LEDS];
WiFiUDP port;

void setup() {
    Serial.begin(115200);
    // Connect to wifi and print the IP address over serial
    WiFiManager wifiManager;
    // Uncomment this line to reset the stored WiFi settings
    // wifiManager.resetSettings();
    wifiManager.autoConnect("esp8266-audio-visualizer");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    port.begin(localPort);
    ledstrip.init(NUM_LEDS);
}

uint8_t N = 0;
#if PRINT_FPS
    uint16_t fpsCounter = 0;
    uint32_t secondTimer = 0;
#endif

void loop() {
    // Read data over socket
    int packetSize = port.parsePacket();
    // If packets have been received, interpret the command
    if (packetSize) {
        int len = port.read(packetBuffer, BUFFER_LEN);
        for(int i = 0; i < len; i+=4) {
            packetBuffer[len] = 0;
            N = packetBuffer[i];
            pixels[N].R = (uint8_t)packetBuffer[i+1];
            pixels[N].G = (uint8_t)packetBuffer[i+2];
            pixels[N].B = (uint8_t)packetBuffer[i+3];
        } 
        ledstrip.show(pixels);

        #if PRINT_FPS
            fpsCounter++;
        #endif
    }
    #if PRINT_FPS
        if (millis() - secondTimer >= 1000U) {
            secondTimer = millis();
            Serial.printf("FPS: %d\n", fpsCounter);
            fpsCounter = 0;
        }
    #endif
}
