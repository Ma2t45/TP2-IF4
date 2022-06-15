//Observation de l’ordonnancement à l’oscilloscope


#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
#define APP_CORE 0
#else
#define APP_CORE (portNUM_PROCESSORS - 1)
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

// Prototypes
void TaskBlink(void *pvParameters);

// the number of the LED pins
#define LED_RED       16   // RED LED corresponds to GPIO16
#define LED_GREEN     19   // GREEN LED corresponds to GPIO19
#define LED_BLUE      4    // BLUE LED corresponds to GPIO4

//Global
const uint8_t leds[] = {LED_RED, LED_GREEN, LED_BLUE};

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  xTaskCreatePinnedToCore(
    TaskBlink, //Function
    "Blink blue ",  //Name
    2048,          //Stack size
    (void*)&leds[2],       //Parameters
    5,             //Priority
    nullptr,       //Task handle
    APP_CORE);     //Core
  
  xTaskCreatePinnedToCore(
    TaskBlink, //Function
    "Blink rouge ",  //Name
    2048,          //Stack size
    (void*)&leds[0],       //Parameters
    5,             //Priority
    nullptr,       //Task handle
    APP_CORE);     //Core

  xTaskCreatePinnedToCore(
    TaskBlink, //Function
    "Blink vert ",  //Name
    2048,          //Stack size
    (void*)&leds[1],       //Parameters
    4,             //Priority
    nullptr,       //Task handle
    APP_CORE);     //Core
}
void loop() {
  // Tout est fait dans les tâches
  vTaskDelete(nullptr);
}

void TaskBlink(void *pvParameters) {

  uint8_t pin = *(uint8_t *)pvParameters; //A COMPLETER

  Serial.printf("Starting on : %hhd", pin);
  vTaskDelay(pdMS_TO_TICKS(2));

  pinMode(pin, OUTPUT);

  for (;;) {
    for (uint32_t i = 0; i < 40000; i++) {
      digitalWrite(pin, HIGH);
      digitalWrite(pin, LOW);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}