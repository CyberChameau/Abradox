#include <avr/eeprom.h>

// Buttons
#define BTN_1 5
#define BTN_2 6
#define BTN_3 7

// LEDs
#define LED_G_1 4
#define LED_G_2 3
#define LED_Y_1 2
#define LED_Y_2 1
#define LED_RED 0

// Sound
#define SOUND 9

// Danger level (0 - low, 1 - medium, 2 - high)
int level = 0;

// Next blinking
int blink_dalay = 0;
// LED state (HIGH/LOW)
uint8_t led_state = false;

// SundDelay [min, max]
int sound_delay = 0;


/* Danger levels */
void low() {
    digitalWrite(LED_G_1, HIGH);
    digitalWrite(LED_G_2, HIGH);
}

void medium() {
    low();
    digitalWrite(LED_Y_1, HIGH);
    digitalWrite(LED_Y_2, HIGH);
}

void high() {
    medium();
    digitalWrite(LED_RED, HIGH);
}
/* ---------- */

// Loading animation
void loading() {
    int LEDs[5] = {LED_G_1, LED_G_2, LED_Y_1, LED_Y_2, LED_RED};
    
    for (int i = 0; i < 5; i++)
        digitalWrite(LEDs[i], HIGH);

    delay(1000);

    for (int i = 0; i < 5; i++)
        digitalWrite(LEDs[i], LOW);

    for (int i = 0; i < 5; i++) {
        digitalWrite(LEDs[i], HIGH);
        delay(300);
        digitalWrite(LEDs[i], LOW);
    }
    
    if (level == 0)
        low();
    else if (level == 1)
        medium();
    else
        high();
}


// Configurating
void setup() {
    // Buttons
    pinMode(BTN_1, INPUT_PULLUP);
    pinMode(BTN_2, INPUT_PULLUP);
    pinMode(BTN_3, INPUT_PULLUP);

    // LEDs
    pinMode(LED_G_1, OUTPUT);
    pinMode(LED_G_2, OUTPUT);
    pinMode(LED_Y_1, OUTPUT);
    pinMode(LED_Y_2, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    // Sound
    pinMode(SOUND, OUTPUT);

    // Random
    randomSeed(analogRead(0));

    level = eeprom_read_byte(0);
    loading();
}

// Sound
void soundAlert() {
    switch (level) {
    case 0:
        tone(SOUND, 300, 100);
        sound_delay = millis() + random(500, 3000);
        break;
    case 1:
        tone(SOUND, 500, 100);
        sound_delay = millis() + random(500, 1000);
        break;
    case 2:
        tone(SOUND, random(700, 1000));
        sound_delay = millis() + 100;
        break;
    }
}

// LEDs
int blink() {
    int LEDs[5];
    int count = 1;
    
    if (level != 2)
        LEDs[0] = (level == 0 ? LED_G_2 : LED_Y_2);
    else {
        LEDs[0] = LED_G_1;
        LEDs[1] = LED_G_2;
        LEDs[2] = LED_Y_1;
        LEDs[3] = LED_Y_2;
        LEDs[4] = LED_RED;
        
        count = 5;
    }
    
    for (int i = 0; i < count; i++)
        digitalWrite(LEDs[i], led_state);

    if (led_state == HIGH)
        led_state = LOW;
    else 
        led_state = HIGH;
    
    if (level == 2)
        return 300;
    return 2000;
}

// Buttons
void checkButtons() {
    if (digitalRead(BTN_1) == LOW) {
        eeprom_write_byte(0, 0);
        delay(500);
    } else if (digitalRead(BTN_2) == LOW) {
        eeprom_write_byte(0, 1);
        delay(500);
    } else if (digitalRead(BTN_3) == LOW) {
        eeprom_write_byte(0, 2);
        delay(500);
    }
}

void loop() {
    if (millis() >= blink_dalay)
        blink_dalay = millis() + random(100, blink());
    
    if (millis() >= sound_delay)
        soundAlert();

    checkButtons();
}
