#include <FastLED.h>
#define LED_PIN         3
#define LED_COUNT       44
#define SNAKE_LENGTH    4
#define NUM_SNAKES      10 // Maximum number of snakes
#define BRIGHTNESS      50
#define LED_TYPE        WS2812
#define COLOR_ORDER     GRB
#define BUTTON_PIN      5 // Define the pin connected to the button

CRGB leds[LED_COUNT];
CRGBPalette16 currentPalette; // Palette for color spectrum
bool isOn = true; // Initial state is ON
bool buttonState = false; // Initialize button state
bool lastButtonState = false; // Initialize last button state
unsigned long lastDebounceTime = 0; // Initialize debounce time
unsigned long debounceDelay = 50; // Set debounce delay time in milliseconds
int ff =0;
// Enumerate modes
enum Mode {
  MOVE_SNAKES,
  RUNNER,
  RAINBOW,
  TOGGLE_LEDS,
  FILL_GREEN,
  FILL_BLUE,
  NUM_MODES // Number of modes
};

Mode currentMode = FILL_BLUE; // Initial mode is MOVE_SNAKES

struct Snake {
  int position;
  int direction;
  bool isActive;
};

Snake allSnakes[6]; // Maximum 3 snakes at a time
unsigned long lastSnakeTime2 = 0; // Time when the last snake was added
unsigned long snakeInterval2 = 300; // Interval between adding snakes (in milliseconds)

void setup() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, LED_COUNT);
    FastLED.setBrightness(BRIGHTNESS);
    pinMode(BUTTON_PIN, INPUT); // Set the button pin as input with internal pull-up resistor
    
    fillFromMiddle(CRGB::White); // Fills from the middle, turning on LEDs one by one towards both ends
    delay(100); 
    fillFromMiddle2(CRGB::Black);
    delay(100); 
    fillFromMiddle(CRGB::White); // Fills from the middle, turning on LEDs one by one towards both ends
    delay(10); 
    blinkThreeTimes(CRGB::White);
    delay(300); 
}

void loop() {
  // Read the button state
  int reading = digitalRead(BUTTON_PIN);
  if( reading==0 && ff==0){
    ff=1;
  }

  // Check if the button state has changed
      if (reading == HIGH && ff==1) { // Check if the button is pressed
        // Increment the current mode and wrap around if necessary
        ff=0;
        currentMode = static_cast<Mode>((currentMode + 1) % NUM_MODES);
  }
  
  // Call the appropriate mode function based on the current mode
  switch (currentMode) {
    case MOVE_SNAKES:
      moveSnakes();
      break;
    case RUNNER:
      runner();
      break;
    case RAINBOW:
      rainbow();
      break;
    case TOGGLE_LEDS:
      toggleLeds();
      break;
    case FILL_GREEN:
      fillFromMiddle(CRGB::Green);
     // delay(100);
      break;
    case FILL_BLUE:
      fillFromMiddle(CRGB::Blue);
      //delay(100);
      break;
    default:
      break;
  }
 // delay(100);
}

void fillFromMiddle(CRGB color) {
    int middleLED = LED_COUNT / 2;
    int startIndex = middleLED;
    int endIndex = middleLED;
    
    while (startIndex >= 0 && endIndex < LED_COUNT) {
        leds[startIndex] = color;
        leds[endIndex] = color;
        startIndex--;
        endIndex++;
        FastLED.show();
        delay(30); // Adjust speed of filling effect
    }
}

void fillFromMiddle2(CRGB color) {
    int startIndex = 0;
    int endIndex = LED_COUNT - 1;
    
    while (startIndex <= endIndex) {
        leds[startIndex] = color;
        leds[endIndex] = color;
        startIndex++;
        endIndex--;
        FastLED.show();
        delay(30); // Adjust speed of filling effect
    }
}

void blinkThreeTimes(CRGB color) {
    for (int i = 0; i < 3; i++) {
        // Turn on all LEDs
        fill_all(color);
        FastLED.show();
        delay(300); // Adjust duration of LED being ON

        // Turn off all LEDs
        fill_all(CRGB::Black);
        
        delay(300); // Adjust duration of LED being OFF
    }
}

void fill_all(CRGB color) {
    for (int i = -1; i < LED_COUNT; i++) {
        leds[i] = color;
    }
    FastLED.show();
}

void moveSnakes() {
    struct Snake {
    int position;
    int direction;
    bool active;
  };

  Snake snakes[NUM_SNAKES]; // Maximum number of snakes at a time
  unsigned long lastSnakeTime = 0; // Time when the last snake was added
  unsigned long snakeInterval = 300; // Interval between adding snakes (in milliseconds)
  int loopCounter = 0; // Counter to track the number of loops completed
// (continued from previous code)

  int colorChangeInterval = 3; // Color change interval in loops

  // Initialize the current palette with a color spectrum
  currentPalette = RainbowColors_p;

  unsigned long currentTime = millis();

  // Add a new snake if the interval has passed and we haven't reached the maximum number of snakes
  if (currentTime - lastSnakeTime >= snakeInterval) {
    for (int i = 0; i < NUM_SNAKES; i++) {
      if (!snakes[i].active) {
        // Initialize new snake
        snakes[i].position = LED_COUNT / 2;
        snakes[i].direction = (i % 2 == 0) ? 1 : -1; // Alternate direction for each snake
        snakes[i].active = true;
        lastSnakeTime = currentTime;
        break;
      }
    }
  }

  // Move the snakes
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = CRGB(0, 0, 0); // Clear all LEDs
  }
  for (int i = 0; i < NUM_SNAKES; i++) {
    if (snakes[i].active) {
      snakes[i].position += snakes[i].direction;

      // Deactivate the snake if it reaches the end
      if (snakes[i].position >= LED_COUNT - SNAKE_LENGTH || snakes[i].position < 0) {
        snakes[i].active = false;
      } else {
        // Update the LEDs for the snake
        for (int j = 0; j < SNAKE_LENGTH; j++) {
          int pos = snakes[i].position + (snakes[i].direction * j);
          if (pos >= 0 && pos < LED_COUNT) {
            leds[pos] = ColorFromPalette(currentPalette, map(pos, 0, LED_COUNT - 1, 0, 255)); // Use palette colors for snake LEDs
          }
        }
      }
    }
  }

  FastLED.show();

  // Increment the loop counter and change the color palette if the color change interval is reached
  loopCounter++;
  if (loopCounter >= colorChangeInterval) {
    loopCounter = 0; // Reset the loop counter
    // Change color palette
    static uint8_t startIndex = 0;
    startIndex += 8; // Increase to change color faster
    for (int i = 0; i < 16; i++) {
      currentPalette[i] = ColorFromPalette(RainbowColors_p, startIndex + i * 16);
    }
  }

  delay(30); // Adjust the speed of the animation by changing the delay time
}
void runner(){
  unsigned long currentTime = millis();

  // Add a new snake if the interval has passed and we have fewer than 3 active snakes
  if (currentTime - lastSnakeTime2 >= snakeInterval2) {
    for (int i = 0; i < 3; i++) {
      if (!allSnakes[i].isActive) {
        // Initialize new snake
        allSnakes[i].position = 0;
        allSnakes[i].direction = 1; // Initial direction: right
        allSnakes[i].isActive = true;
        lastSnakeTime2 = currentTime;
        break;
      }
    }
  }

  // Move the snakes
  for (int i = 0; i < 3; i++) {
    if (allSnakes[i].isActive) {
      allSnakes[i].position += allSnakes[i].direction;

      // Deactivate the snake if it reaches the end
      if (allSnakes[i].position >= LED_COUNT - SNAKE_LENGTH) {
        allSnakes[i].isActive = false;
      }
    }
  }

  // Update the LEDs
  for (int i = 0; i < LED_COUNT; i++) {
    int brightness = 0;
    for (int j = 0; j < 6; j++) {
      if (allSnakes[j].isActive && i >= allSnakes[j].position && i < allSnakes[j].position + SNAKE_LENGTH) {
        brightness = 255; // Turn on the snake LEDs
        break; // Exit the loop if the LED is part of any active snake
      }
    }
    leds[i] = CRGB(brightness, brightness, brightness);
  }

  FastLED.show();
  delay(15); // Adjust the speed of the animation by changing the delay time
}
void rainbow() {
  static uint8_t hue = 0;
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = CHSV(hue + (i * 255 / LED_COUNT), 255, 255);
  }
  hue++;
    FastLED.show();
  delay(10); 
}
void toggleLeds() {
  for (int i = 0; i < LED_COUNT; i++) {
    if (i % 2 == 0) {
      leds[i] = isOn ? CRGB::Purple : CRGB::Black; // Toggle even-indexed LEDs
    } else {
      leds[i] = isOn ? CRGB::Black : CRGB::Purple; // Toggle odd-indexed LEDs
    }
  }
  isOn = !isOn; // Toggle the state for the next loop
  FastLED.show();
  delay(200); }
