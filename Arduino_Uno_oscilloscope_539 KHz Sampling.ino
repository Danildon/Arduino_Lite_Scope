#include "ADCboost.h"
#include "Streaming.h"

// ------------------- Configuration -------------------
constexpr uint16_t SAMPLE_COUNT = 800;       // Number of ADC samples
constexpr uint32_t SERIAL_BAUD = 115200;    // Serial baud rate
constexpr uint32_t SAMPLE_INTERVAL_US = 1850; // ~539 kSps sampling interval

uint16_t adcBuffer[SAMPLE_COUNT];
volatile uint16_t writeIndex = 0;           // Circular buffer write index

// ------------------- Debug Configuration -------------------
#define DEBUG 0
#if DEBUG
  #define DEBUG_PRINT(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
#endif

// ------------------- Function Prototypes -------------------
void setupADC();
void acquireSample();
void transmitBuffer();
float adcToVoltage(uint16_t adcValue);

// ------------------- Arduino Setup -------------------
void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(A0, INPUT);
  setupADC();
}

// ------------------- Main Loop -------------------
void loop() {
  static uint32_t lastSampleTime = 0;
  uint32_t currentTime = micros();

  // Check if it's time to sample
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL_US) {
    lastSampleTime += SAMPLE_INTERVAL_US;
    acquireSample();
  }

  // Optionally stream buffer when full
  if (writeIndex >= SAMPLE_COUNT) {
    transmitBuffer();
    writeIndex = 0; // Reset buffer index
  }
}

// ------------------- Function Definitions -------------------

// ADC initialization
void setupADC() {
  ADC_init(0);  // Default VCC reference
  DEBUG_PRINT("ADC Initialized");
}

// Acquire a single ADC sample and store in circular buffer
void acquireSample() {
  if (writeIndex < SAMPLE_COUNT) {
    adcBuffer[writeIndex++] = ADC_read();
  }
}

// Transmit buffer content over Serial
void transmitBuffer() {
  Serial.println("start"); // Marker for MATLAB
  for (uint16_t i = 0; i < SAMPLE_COUNT; i++) {
    Serial.println(adcBuffer[i]);
  }
  DEBUG_PRINT("Buffer transmitted");
}

// Convert ADC value to voltage
float adcToVoltage(uint16_t adcValue) {
  return adcValue * (5.0f / 1023.0f);
}
