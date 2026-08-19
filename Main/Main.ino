#include <WiFiS3.h>
#include <ArduinoMqttClient.h>
#include <FastLED.h>

// --------------------
// WiFi
// --------------------
const char* ssid = "DIR-X1530-B318";
const char* password = "Sommer01";

// --------------------
// MQTT
// --------------------
const char* broker = "broker.hivemq.com";
const int port = 1883;

const char* LEDTopic = "arduino/LEDControl";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// --------------------
// LED Strip
// --------------------
#define DATA_PIN 6
#define NUM_LEDS 90

CRGB leds[NUM_LEDS];


void setup()
{
  Serial.begin(9600);

  // --------------------
  // Start LED strip
  // --------------------
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  FastLED.clear();
  FastLED.show();


  // --------------------
  // Connect to WiFi
  // --------------------
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected to WiFi!");


  // --------------------
  // Connect to MQTT
  // --------------------
  Serial.print("Connecting to MQTT broker: ");
  Serial.println(broker);

  while (!mqttClient.connect(broker, port))
  {
    Serial.print(".");
    delay(2000);
  }

  Serial.println();
  Serial.println("Connected to MQTT broker!");


  // --------------------
  // Subscribe to LED topic
  // --------------------
  mqttClient.subscribe(LEDTopic);

  Serial.print("Subscribed to: ");
  Serial.println(LEDTopic);
}


void loop()
{
  // --------------------
  // Keep MQTT connection alive
  // --------------------
  mqttClient.poll();


  // --------------------
  // Check for incoming MQTT message
  // --------------------
  int messageSize = mqttClient.parseMessage();

  if (messageSize)
  {
    Serial.print("Received message on: ");
    Serial.println(mqttClient.messageTopic());


    // Read MQTT message
    String message = "";

    while (mqttClient.available())
    {
      message += (char)mqttClient.read();
    }


    Serial.print("Message: ");
    Serial.println(message);


    // --------------------
    // OFF
    // --------------------

    if (message == "OFF")
    {
      fill_solid(
        leds,
        NUM_LEDS,
        CRGB::Black
      );

      FastLED.show();

      Serial.println("LEDs -> OFF");
    }


    // --------------------
    // RGB COLOR
    // Example:
    // 255,0,0
    // --------------------

    else
    {
      int firstComma =
        message.indexOf(',');

      int secondComma =
        message.indexOf(
          ',',
          firstComma + 1
        );


      // Make sure the message
      // contains two commas
      if (
        firstComma > 0 &&
        secondComma > firstComma
      )
      {

        // --------------------
        // Extract RGB values
        // --------------------

        int r =
          message
            .substring(
              0,
              firstComma
            )
            .toInt();


        int g =
          message
            .substring(
              firstComma + 1,
              secondComma
            )
            .toInt();


        int b =
          message
            .substring(
              secondComma + 1
            )
            .toInt();


        // --------------------
        // Keep values 0-255
        // --------------------

        r = constrain(r, 0, 255);
        g = constrain(g, 0, 255);
        b = constrain(b, 0, 255);


        // --------------------
        // Set LED color
        // --------------------

        fill_solid(
          leds,
          NUM_LEDS,
          CRGB(r, g, b)
        );

        FastLED.show();


        // --------------------
        // Serial output
        // --------------------

        Serial.print("LEDs -> RGB(");
        Serial.print(r);
        Serial.print(",");
        Serial.print(g);
        Serial.print(",");
        Serial.print(b);
        Serial.println(")");
      }

      else
      {
        Serial.println(
          "Unknown LED command"
        );
      }
    }
  }
}