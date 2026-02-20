#include <Wire.h>
#include "Adafruit_TCS34725.h"


const int SERVO_CYCLE_US = 20000;   // 20 milliseconds (50 Hz cycle)
const int SERVO_MIN_US = 1500;       // ~0 degrees pulse width (Minimum angle)
const int SERVO_MAX_US = 2100;      // ~180 degrees pulse width (Maximum angle)
const int SERVO_NEUTRAL_US = 1500;  // Center/Neutral for reference (unused in actuateServo now)
bool neutral = false;
bool firstRead = true;
int queuedCode1 = 7;
int queuedCode2 = 7;
int now = millis();

const int RED_SERVO_PIN = 13;
const int GREEN_SERVO_PIN = 11;
const int BLUE_SERVO_PIN = 12;
const int YELLOW_SERVO_PIN = 8;
const int ORANGE_SERVO_PIN = 9;
const int BROWN_SERVO_PIN = 10;
const int NO_COLOR_DETECTED = 0; // Keeping this for clarity if used in logic


Adafruit_TCS34725 tcs = Adafruit_TCS34725(
    TCS34725_INTEGRATIONTIME_50MS,
    TCS34725_GAIN_4X
);

uint16_t r, g, b, c;

struct ColorRef {
    int code;
    float r, g, b;
};

ColorRef colors[] = {
    {1, 0.650, 0.177, 0.174}, // Red
    {2, 0.284, 0.482, 0.235}, // Green
    {3, 0.152, 0.363, 0.487}, // Blue
    {4, 0.479, 0.366, 0.160}, // Yellow
    {5, 0.663, 0.193, 0.145}, // Orange
    {6, 0.426, 0.311, 0.263}, // Brown
    {7, 0.363, 0.342, 0.295}  // Custom Color 7
};


// ===============================
int closestColor(float rn, float gn, float bn) {
    float bestDist = 9999.0;
    int bestCode = NO_COLOR_DETECTED; 
    for (size_t i = 0; i < sizeof(colors) / sizeof(colors[0]); ++i) {
        float dr = rn - colors[i].r;
        float dg = gn - colors[i].g;
        float db = bn - colors[i].b;
        float dist = dr*dr + dg*dg + db*db; 
        if (dist < bestDist) {
            bestDist = dist;
            bestCode = colors[i].code;
        }
    }
    return bestCode;
}

void sendServoPulse(int pin, int pulseWidthUs) {
  // 1. Start the Pulse (PWM HIGH signal)
  digitalWrite(pin, HIGH);
  delayMicroseconds(pulseWidthUs);

  // 2. End the Pulse (PWM LOW signal)
  digitalWrite(pin, LOW);
  
  // 3. Wait for the remainder of the 20ms cycle.
  int lowTimeUs = SERVO_CYCLE_US - pulseWidthUs;
  delayMicroseconds(lowTimeUs);
}

void actuateServo(int pin) { 
    // 20 pulses * 20ms/pulse = 400ms per movement (0.4s)
    if(pin == 7) { return; }

    const int MOVEMENT_PULSES = 20; 

    // 1. ACTUATE: Move to the MAXIMUM position (SERVO_MAX_US)
    for (int i = 0; i < MOVEMENT_PULSES; i++) {
        sendServoPulse(pin, SERVO_MAX_US); 
    }
    delay(1000);
      // 2. RETURN: Move back to the MINIMUM position (SERVO_MIN_US)
    for (int i = 0; i < MOVEMENT_PULSES; i++) {
        sendServoPulse(pin, SERVO_MIN_US); 
    }
}

void ColorSensor() {
    tcs.getRawData(&r, &g, &b, &c);
    delay(60); 

    float total = r + g + b;


    // NORMALIZATION AND ACTUATION LOGIC (Only runs after firstRead is false)
    float rn = r / total;
    float gn = g / total;
    float bn = b / total;

    int code = closestColor(rn, gn, bn);    
    
    Serial.print("Code: "); Serial.print(code);
    Serial.print(" | Ratios: Rn="); Serial.print(rn, 3);
    Serial.print(" Gn="); Serial.print(gn, 3);
    Serial.print(" Bn="); Serial.print(bn, 3);
    // ACTUATE THE CORRECT SERVO based on the single reading
    switch (code) { 
        case 1: Serial.println(" -> Red (Actuating 13)");      actuateServo(RED_SERVO_PIN);    break;
        case 2: Serial.println(" -> Green (Actuating 11)");    actuateServo(GREEN_SERVO_PIN);  break;
        case 3: Serial.println(" -> Blue (Actuating 12)");     actuateServo(BLUE_SERVO_PIN);   break;
        case 4: Serial.println(" -> Yellow (Actuating 8)");    actuateServo(YELLOW_SERVO_PIN); break;
        case 5: Serial.println(" -> Orange (Actuating 9)");    actuateServo(ORANGE_SERVO_PIN); break;
        case 6: Serial.println(" -> Brown (Actuating 10)");    actuateServo(BROWN_SERVO_PIN);  break;
        case 7: Serial.println(" -> Custom Color 7 (Ignored)");  queuedCode1 = 7;             break; 
        default: Serial.println(" -> No Match Found"); break;
    }
}
void setServoNeutral(int pin) {
  if(pin == 7) { return; }

    const int MOVEMENT_PULSES = 20; 
for (int i = 0; i < MOVEMENT_PULSES; i++) {
        sendServoPulse(pin, SERVO_MIN_US); 
    }
}
void setup() {
    Serial.begin(115200);
    
    // Array of all servo pins for easy iteration
    int servoPins[] = {RED_SERVO_PIN, GREEN_SERVO_PIN, BLUE_SERVO_PIN, 
                       YELLOW_SERVO_PIN, ORANGE_SERVO_PIN, BROWN_SERVO_PIN};
    int numServos = sizeof(servoPins) / sizeof(servoPins[0]);
    
    // Initialize all pins and immediately set them to NEUTRAL
    for (int i = 0; i < numServos; i++) {
        pinMode(servoPins[i], OUTPUT);
        // Stabilize the servo immediately
        setServoNeutral(servoPins[i]);
    }

    if (!tcs.begin()) {
        Serial.println("TCS34725 ERROR. Check wiring (SDA/SCL) and power.");
        while (1); 
    }
    Serial.println("TCS34725 initialized. Ready to sense.");
}

void loop() {    
  delay(50);
  ColorSensor();
}