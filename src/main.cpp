#include <Arduino.h>
#include "SoundboxLogic.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

//  ++++++++++++++++++++++++++++++++++++++++  Basic Credentials  ++++++++++++++++++++++++++++++

// WiFi credentials
const char *ssid = "V2029";
const char *password = "tanbir123";

// HiveMQ Cloud MQTT Broker
const char *mqtt_server = "3c07990e59c44be0afbc193bdbf9af31.s1.eu.hivemq.cloud"; // Example: "yourcluster.s1.eu.hivemq.cloud"
const int mqtt_port = 8883;                                                      // Secure TLS port

// MQTT credentials (HiveMQ Cloud)
const char *mqtt_user = "pradhantanbir";
const char *mqtt_password = "Tanbir123";
const char *mqtt_topic = "soundbox@tanbir@1001";

// Define LED pin
const int ledPin = 2; // Built-in LED on most ESP32 boards

// Root CA certificate (HiveMQ Cloud / Let's Encrypt ISRG Root X1)
const char *ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// Secure WiFi client & MQTT client
WiFiClientSecure espClient;
PubSubClient client(espClient);

// ++++++++++++++++++++++++++++++++++++   MQTT   +++++++++++++++

// Function to handle received MQTT messages
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String message = "";
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.print("Message: ");
  Serial.println(message);

  if (message.length() > 0)
  {
    int number = atoi(message.c_str()); // Convert string to integer
    std::vector<String> fileBuffer = generateFileBuffer(number);
    playAudioFiles(fileBuffer); // Play the files sequentially
  }
  else
  {
    Serial.println("Received empty message.");
  }
}

// Function to reconnect to MQTT broker
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT... ");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password))
    {
      Serial.println("Connected!");
      client.subscribe(mqtt_topic);
    }
    else
    {
      Serial.print("Failed (Error Code: ");
      Serial.print(client.state());
      Serial.println("). Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("==== ESP32 Soundbox WAV Playback Start ====");

  if (!SD.begin())
  {
    Serial.println("❌ SD init failed");
    while (1)
      ;
  }
  Serial.println("✅ SD initialized");

  out = new AudioOutputI2S();
  out->SetPinout(26, 22, 25); // BCLK, LRC, DIN
  out->SetGain(1.0);

  // Setup for web servers

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  espClient.setCACert(ca_cert); // Load CA certificate
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  reconnect();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}

