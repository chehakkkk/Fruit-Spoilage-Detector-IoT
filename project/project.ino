#include <IRremote.h>

// Define pins
const int RECV_PIN = 7;    // IR sensor output pin connected to D7
const int LED_PIN = 12;    // LED connected to D12

IRrecv irrecv(RECV_PIN);   // Create IR receiver object
decode_results results;    // Variable to store decoded results

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  irrecv.enableIRIn();     // Start the IR receiver
  Serial.println("IR Receiver is ready...");
}

void loop() {
  // Check if any IR signal is received
  if (irrecv.decode(&results)) {
    Serial.print("Received code: ");
    Serial.println(results.value, HEX); // Print in HEX format

    switch (results.value) {
      case 0x10:  // Replace with your remote's button HEX code
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
        break;

      case 0x11:  // Replace with another button HEX code
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
        break;

      default:
        Serial.println("Unknown button pressed");
        break;
    }

    irrecv.resume(); // Receive the next value
  }
}