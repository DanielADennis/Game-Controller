#include <SPI.h>                //communication protocal for the OLED
#include <Wire.h>               //needed for the OLED
#include <Adafruit_GFX.h>       //graphics library for OLED
#include <Adafruit_SSD1306.h>   //OLED Display communication with graphics library
#include <Adafruit_NeoPixel.h>  //drive the LED strip
#include <Keyboard.h>           //HID keyboard

// LED's
#define PIN_LIGHTS 1
#define LED_NUM 10

// Keys
#define PIN_KEY_1 5  
#define PIN_KEY_2 4  
#define PIN_KEY_3 16  
#define PIN_KEY_4 14  
#define PIN_KEY_5 7
#define PIN_KEY_6 6  
#define PIN_KEY_7 9  
#define PIN_KEY_8 8  

// Joy Stick
#define PIN_JOY_KEY 15
#define PIN_JOY_1 19
#define PIN_JOY_2 20

// Oled
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Non-char Key
#define KEY_LEFT_SHIFT 0x81

//holds the condition of all 8 mechanical switches
struct KeyState
{
  bool key_1;
  bool key_2;
  bool key_3;
  bool key_4;
  bool key_5;
  bool key_6;
  bool key_7;
  bool key_8;
};

//holds the condition of the joystick
struct JoyState
{
  bool button;
  uint16_t pot_1;
  uint16_t pot_2;
};

///////////////////
// Configuration //
///////////////////

// Key
bool keyLatch = true;
char charKey1 = '1';
char charKey2 = '2';
char charKey3 = '3';
char charKey4 = '4';
char charKey5 = '5';
char charKey6 = '6';
char charKey7 = '7';
char charKey8 = '8';

// Joy
bool joyLatch = true;
bool outerKeyLatch = false;
bool potLatch[4] = {false, false, false, false};
uint16_t innerKeyValue = 80;  //initial keypress after the deadzone
uint16_t outerKeyValue = 600; //secondary keypress functionality
uint16_t pot_1_center = 510;  //roughly the center value of the potentiometer
uint16_t pot_2_center = 510;  //roughly the center value of the potentiometer
char charLeft = 'd';
char charRight = 'a';
char charUp = 's';
char charDown = 'w';
char charOuterKey = KEY_LEFT_SHIFT;
//char charOuterKey = 'R';
char joyButton = '9';

///////////////////
//      END      //
///////////////////

//   Display   //
bool displayActive = false;
uint16_t displayTimer = 200;
uint16_t displayCount = 0;
uint8_t activeMenu = 1;
uint8_t activePosition = 0;
uint8_t menu_1_len = 5;
String menu_1[] = {"Colors", "Brightness", "Menu 3", "Menu 4", "Menu 5"};
uint8_t menu_2_len = 7;
String menu_2[] = {"Red", "Green", "Blue", "Cyan", "Yellow", "Magenta", "White"};
uint8_t menu_3_len = 6;
String menu_3[] = {"0%", "20%", "40%", "60%", "80%", "100%"};
uint8_t menu_4_len = 4;
String menu_4[] = {"3 - 1", "3 - 2", "3 - 3", "3 - 4"};
uint8_t menu_5_len = 4;
String menu_5[] = {"4 - 1", "4 - 2", "4 - 3", "4 - 4"};
uint8_t menu_6_len = 4;
String menu_6[] = {"5 - 1", "5 - 2", "5 - 3", "5 - 4"};
// End Display //

//   LEDs   //
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, PIN_LIGHTS, NEO_GRB + NEO_KHZ800);
uint8_t r = 50;
uint8_t g = 50;
uint8_t b = 50;
uint8_t brightness = 1;
// End LEDs //



KeyState prevKeyState, curKeyState;
JoyState prevJoyState, curJoyState;

///////////////////////////////
// Set up initial conditions //
///////////////////////////////

void setup()
{
  Serial.begin(115200);
  
  // LED's
  strip.begin();

  // Keyboard
  Keyboard.begin();

  // Keys
  pinMode(PIN_KEY_1, INPUT_PULLUP);
  pinMode(PIN_KEY_2, INPUT_PULLUP);
  pinMode(PIN_KEY_3, INPUT_PULLUP);
  pinMode(PIN_KEY_4, INPUT_PULLUP);
  pinMode(PIN_KEY_5, INPUT_PULLUP);
  pinMode(PIN_KEY_6, INPUT_PULLUP);
  pinMode(PIN_KEY_7, INPUT_PULLUP);
  pinMode(PIN_KEY_8, INPUT_PULLUP);

  // Joy Stick
  pinMode(PIN_JOY_KEY, INPUT_PULLUP);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
  
  setLeds(r, g, b, brightness);
}
///////////////////////////////////
// End set up initial conditions //
///////////////////////////////////

///////////////
// Main Loop //
///////////////
void loop()
{
  // Read states and get changed values
  KeyState keyState = getKeyState(keyLatch);
  JoyState joyState = getJoyState(joyLatch);

  // Handle values
  handleKeyValues(keyState);
  handleJoyValues(joyState);

  // Update prev states
  updateStates();

  updateMenu(keyState);
  
  loopDelay(displayActive);
}
///////////////////
// End Main Loop //
///////////////////

// Delay depending on game mode or menu mode //
void loopDelay(bool displayActive)
{
  if (displayActive)
  {
    delay(50);
  }
  else
  {
   delay(10); 
  } 
}
// End Delay //

// Press keys depending on button state //
void handleKeyValues(KeyState state)
{
  if (state.key_1)
  {
    Keyboard.write(charKey1);
  }
  if (state.key_2)
  {
    Keyboard.write(charKey2);
  }
  if (state.key_3)
  {
    Keyboard.write(charKey3);
  }
  if (state.key_4)
  {
    Keyboard.write(charKey4);
  }
  if (state.key_5)
  {
    Keyboard.write(charKey5);
  }
  if (state.key_6)
  {
    Keyboard.write(charKey6);
  }
  if (state.key_7)
  {
    Keyboard.write(charKey7);
  }
  if (state.key_8)
  {
    Keyboard.write(charKey8);
  }
}
// End handleKeyValues //

// Press keys depending on Joystick state //
void handleJoyValues(JoyState state)
{
  if (state.button)
  {
    Keyboard.write(joyButton);
  }

  int16_t pot_1 = state.pot_1 - pot_1_center;
  int16_t pot_2 = state.pot_2 - pot_2_center;

  if (abs(pot_1) >= innerKeyValue)
  {
    if (pot_1 > 0)
    {
      if (!potLatch[0]) 
      {
        Keyboard.press(charLeft);
        potLatch[0] = true;
      }
    }
    else
    {
      if (!potLatch[1]) 
      {
        Keyboard.press(charRight);
        potLatch[1] = true;
      }
    }
  }
  else 
  {
    if (potLatch[0])
    {
      Keyboard.release(charLeft);
      potLatch[0] = false;
    }
    if (potLatch[1])
    {
      Keyboard.release(charRight);
      potLatch[1] = false;
    }
  }
  
  if (abs(pot_2) >= innerKeyValue)
  {
    if (pot_2 > 0)
    {
      if (!potLatch[2]) 
      {
        Keyboard.press(charDown);
        potLatch[2] = true;
      }
    }
    else
    {
      if (!potLatch[3]) 
      {
        Keyboard.press(charUp);
        potLatch[3] = true;
      }
    }
  }
  else
  {
    if (potLatch[2])
    {
      Keyboard.release(charDown);
      potLatch[2] = false;
    }
    if (potLatch[3])
    {
      Keyboard.release(charUp);
      potLatch[3] = false;
    }
  }
  
  if (abs(pot_1) >= outerKeyValue || abs(pot_2) >= outerKeyValue)
  {
    if (!outerKeyLatch) 
    {
      Keyboard.press(charOuterKey);
      outerKeyLatch = true;
    }
  }
  else if (outerKeyLatch)
  {
    Keyboard.release(charOuterKey);
    outerKeyLatch = false;
  }
}
// End handleKeyValues //

// set the previous states to be the current states
void updateStates()
{
  prevKeyState = curKeyState;
  prevJoyState = curJoyState;
}


KeyState getKeyState(bool updated)
{
  KeyState state;
  state.key_1 = digitalRead(PIN_KEY_1) == LOW;
  state.key_2 = digitalRead(PIN_KEY_2) == LOW;
  state.key_3 = digitalRead(PIN_KEY_3) == LOW;
  state.key_4 = digitalRead(PIN_KEY_4) == LOW;
  state.key_5 = digitalRead(PIN_KEY_5) == LOW;
  state.key_6 = digitalRead(PIN_KEY_6) == LOW;
  state.key_7 = digitalRead(PIN_KEY_7) == LOW;
  state.key_8 = digitalRead(PIN_KEY_8) == LOW;

  curKeyState = state;

  if (updated)
  {
    state = compareKeyState(prevKeyState, curKeyState);
  }

  return state;
}

// Only update if previous was 0 and current is 1
KeyState compareKeyState(KeyState prev, KeyState cur)
{
  KeyState state;
  state.key_1 = !prev.key_1 && cur.key_1;
  state.key_2 = !prev.key_2 && cur.key_2;
  state.key_3 = !prev.key_3 && cur.key_3;
  state.key_4 = !prev.key_4 && cur.key_4;
  state.key_5 = !prev.key_5 && cur.key_5;
  state.key_6 = !prev.key_6 && cur.key_6;
  state.key_7 = !prev.key_7 && cur.key_7;
  state.key_8 = !prev.key_8 && cur.key_8;

  return state;
}

JoyState getJoyState(bool updated)
{
  JoyState state;
  state.button = digitalRead(PIN_JOY_KEY) == LOW;
  state.pot_1 = analogRead(PIN_JOY_1);
  state.pot_2 = analogRead(PIN_JOY_2);

  curJoyState = state;

  if (updated)
  {
    state = compareJoyState(prevJoyState, curJoyState);
  }

  return state;
}

// Grab raw analog data from potentiometers.                       //
// On center click, only update if previous was 0 and current is 1 //
JoyState compareJoyState(JoyState prev, JoyState cur)
{
  JoyState state;
  state.button = !prev.button && cur.button;
  state.pot_1 = cur.pot_1;
  state.pot_2 = cur.pot_2;

  return state;
}

// Draw Menu
void drawMenu(String* menu, uint8_t stringLen, uint8_t activeLine) 
{
  display.clearDisplay();
  display.setCursor(0,0);

  for(int i = 0; i < stringLen; ++i)
  {
    display.setTextSize(1);
    if (i == activeLine)
    {
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    }
    else
    {
      display.setTextColor(SSD1306_WHITE);
    }
    
    display.println(menu[i]);
  }
  display.display();
}
// End Draw Menu

// State machine for menu
void handleMenuUpdate(bool button)
{
  if (activeMenu == 1)
  {
    if (button)
    {
      if (activePosition == 0)
      {
        activeMenu = 2;
        activePosition = 0;
      }
      else if (activePosition == 1)
      { 
        activeMenu = 3;
        activePosition = 0;
      }
      else if (activePosition == 2)
      {
        activeMenu = 4;
        activePosition = 0;
      }
      else if (activePosition == 3)
      {
        activeMenu = 5;
        activePosition = 0;
      }
      else if (activePosition == 4)
      {
        activeMenu = 6;
        activePosition = 0;
      }
    }
    else
    {
      displayCount = displayTimer;
    }
    
  }
  else if (activeMenu == 2)
  {
    if (button)
    {
      if (activePosition == 0)
      {
        r = 50;
        g = 0;
        b = 0; 
        activePosition = 0;
      }
      else if (activePosition == 1)
      { 
        r = 0;
        g = 50;
        b = 0; 
        activePosition = 0;
      }
      else if (activePosition == 2)
      {
        r = 0;
        g = 0;
        b = 50; 
        activePosition = 0;
      }
      else if (activePosition == 3)
      {
        r = 0;
        g = 50;
        b = 50; 
        activePosition = 0;
      }
      else if (activePosition == 4)
      {
        r = 50;
        g = 50;
        b = 00; 
        activePosition = 0;
      }
      else if (activePosition == 5)
      {
        r = 50;
        g = 0;
        b = 50; 
        activePosition = 0;
      }
      else if (activePosition == 6)
      {
        r = 50;
        g = 50;
        b = 50; 
        activePosition = 0;
      }
      setLeds(r, g, b, brightness);
    }
    else
    {
      activeMenu = 1;
    }
  }
  else if (activeMenu == 3)
  {
    if (button)
    {
      if (activePosition == 0)
      {
        brightness = 0;
        activePosition = 0;
      }
      else if (activePosition == 1)
      { 
        brightness = 1;
        activePosition = 0;
      }
      else if (activePosition == 2)
      {
        brightness = 2;
        activePosition = 0;
      }
      else if (activePosition == 3)
      {
        brightness = 3;
        activePosition = 0;
      }
      else if (activePosition == 4)
      {
        brightness = 4;
        activePosition = 0;
      }
      else if (activePosition == 5)
      {
        brightness = 5;
        activePosition = 0;
      }
      setLeds(r, g, b, brightness);
    }
    else
    {
      activeMenu = 1;
    }
    
  }
  else if (activeMenu == 4)
  {
    if (button)
    {
      if (activePosition == 0)
      {
        activeMenu = 1;
        activePosition = 0;
      }
      else if (activePosition == 1)
      { 
        activeMenu = 1;
        activePosition = 0;
      }
      else if (activePosition == 2)
      {
        activeMenu = 1;
        activePosition = 0;
      }
      else if (activePosition == 3)
      {
        activeMenu = 1;
        activePosition = 0;
      }
    }
    else
    {
      activeMenu = 1;
    }
  }
  else if (activeMenu == 5)
  {
    if (button)
    {
      if (activePosition == 0)
      {
        activeMenu = 1;
        activePosition = 0;
      }
      else if (activePosition == 1)
      { 
        activeMenu = 1;
        activePosition = 0;
      }
      else if (activePosition == 2)
      {
        activeMenu = 1;
        activePosition = 0;
      }
      else if (activePosition == 3)
      {
        activeMenu = 1;
        activePosition = 0;
      }
    }
    else
    {
      activeMenu = 1;
    }
    
  }
  else if (activeMenu == 6)
  {
    if (button)
    {
      if (activePosition == 0)
      {
        activeMenu = 1;
        activePosition = 0;
      }
      else if (activePosition == 1)
      { 
        activeMenu = 1;
        activePosition = 0;
      }
      else if (activePosition == 2)
      {
        activeMenu = 1;
        activePosition = 0;
      }
      else if (activePosition == 3)
      {
        activeMenu = 1;
        activePosition = 0;
      }
    }
    else
    {
      activeMenu = 1;
    }
  }
}
// End state machine for menu

//////////////////////////////////////
// Display and Update settings menu //
//////////////////////////////////////

void updateMenu(KeyState keyState)
{
  // Check if we should be in menu mode
  if (curKeyState.key_1 && curKeyState.key_2 && curKeyState.key_3 && curKeyState.key_4 && curKeyState.key_5 && curKeyState.key_6 && curKeyState.key_7 && curKeyState.key_8)
  {
    displayActive = true;
  }

  if (displayActive)
  {
    displayCount+=1;
    // reset timer if a key is pressed
    if (curKeyState.key_1 || curKeyState.key_2 || curKeyState.key_3 || curKeyState.key_4 || curKeyState.key_5 || curKeyState.key_6 || curKeyState.key_7 || curKeyState.key_8)
    {
      displayCount = 0;
    }
    
    if (displayCount > displayTimer)
    {
      activeMenu = 1;
      activePosition = 0;
      displayCount = 0;
      displayActive = false;
      display.clearDisplay();
      display.display();
    }
    else
    {
      if (keyState.key_8)
      {
        activePosition +=1;
      }
      else if (keyState.key_4)
      {
        activePosition -=1;
      }
      else if (keyState.key_3)
      {
        handleMenuUpdate(true);
      }
      else if (keyState.key_7)
      {
        handleMenuUpdate(false);
      }
      
      // Draw here
      if (activeMenu == 1)
      {
        if (activePosition > (menu_1_len - 1))
        {
          activePosition = 0;
        }
        drawMenu(menu_1, menu_1_len, activePosition);
      }
      else if (activeMenu == 2)
      {
        if (activePosition > (menu_2_len - 1))
        {
          activePosition = 0;
        }
        drawMenu(menu_2, menu_2_len, activePosition);
      }
      else if (activeMenu == 3)
      {
        if (activePosition > (menu_3_len - 1))
        {
          activePosition = 0;
        }
        drawMenu(menu_3, menu_3_len, activePosition);
      }
      else if (activeMenu == 4)
      {
        if (activePosition > (menu_4_len - 1))
        {
          activePosition = 0;
        }
        drawMenu(menu_4, menu_4_len, activePosition);
      }
      else if (activeMenu == 5)
      {
        if (activePosition > (menu_5_len - 1))
        {
          activePosition = 0;
        }
        drawMenu(menu_5, menu_5_len, activePosition);
      }
      else if (activeMenu == 6)
      {
        if (activePosition > (menu_6_len - 1))
        {
          activePosition = 0;
        }
        drawMenu(menu_6, menu_6_len, activePosition);
      }
    }
  }
}
//////////////////////////////////////////
// End display and Update settings menu //
//////////////////////////////////////////


/////////////////////////
// Set LED light strip //
/////////////////////////

void setLeds(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
  for(int i=0; i<LED_NUM; i++) 
  {
    strip.setPixelColor(i, strip.Color(r * brightness, g * brightness, b * brightness));
  }
  strip.show();
}
/////////////////////////////
// End set LED light strip //
/////////////////////////////
