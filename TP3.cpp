#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
#define APP_CORE 0
#else
#define APP_CORE (portNUM_PROCESSORS - 1)
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN  13
#endif

// the number of the LED pins
#define LED_RED      16     // RED LED corresponds to GPIO16
#define LED_GREEN    19     // GREEN LED corresponds to GPIO19
#define LED_BLUE     4      // BLUE LED corresponds to GPIO4

// Buttons
#define  BP_1_PIN    14
#define  BP_2_PIN    15
QueueHandle_t queue;


static void DebounceTask(void *pvParameters) {
  uint32_t level, state = 0, last = 1;
  uint32_t mask = 0x7FFFFFFF;
  bool event;

  for (;;) {
    level = digitalRead(BP_1_PIN);
    state = (state << 1) | level;
    if (((state & mask) == mask) || ((state & mask) == 0)) {
      if (level != last) {
        event = !level;  // true when pressed
        if (xQueueSendToBack(queue,
                            (void *)&state,
                            portMAX_DELAY)) {       //send event to Queue and only modify last level if the event could be sent
          last = level;
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(2));
  }
}

static void LedTask(void *pvParameters) {
  bool event, led = false;

  digitalWrite(LED_BUILTIN, led);

  for (;;) {
     xQueueReceive(queue,
                  (void *)&event,
                  portMAX_DELAY);//Read from queue
    if (event) {
      led ^= true;
      digitalWrite(LED_BUILTIN, led);
    } else {
      digitalWrite(LED_BLUE, HIGH);
      for(int i=0;i<10;i++){
        digitalWrite(LED_BLUE, HIGH);
        vTaskDelay(pdMS_TO_TICKS(50));
        digitalWrite(LED_BLUE, LOW);
        vTaskDelay(pdMS_TO_TICKS(200));
      } //Flash 10 pulses on Blue LED
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BP_1_PIN, INPUT);

  //Creation de la file
  queue = xQueueCreate(10, sizeof(bool));

  xTaskCreatePinnedToCore(DebounceTask,  // Function
                          "Debounce",    // Name
                          8192,          // Stack size
                          nullptr,       // Parameters
                          2,             // Priority
                          nullptr,       // Task handle
                          APP_CORE);     // Core

  xTaskCreatePinnedToCore(LedTask,       // Function
                          "Toggle Led",  // Name
                          8192,          // Stack size
                          nullptr,       // Parameters
                          1,             // Priority
                          nullptr,       // Task handle
                          APP_CORE);     // Core
}

void loop() {
  // Tout est fait dans les tâches
  vTaskDelete(nullptr);
}