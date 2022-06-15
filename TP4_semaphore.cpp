#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
#define APP_CORE 0
#else
#define APP_CORE (portNUM_PROCESSORS - 1)
#endif

// Task function prototype
void waitingTask(void *pvParameters);

#define PUSH_BUTTON_S1 14

SemaphoreHandle_t syncSema;

void IRAM_ATTR toggleLED() {
  static BaseType_t xLastTick = 0;
  static BaseType_t xHigherPriorityTaskWoken;
  BaseType_t xCurrentTick;
  xHigherPriorityTaskWoken = pdFALSE;
  //Déclaration de variable à compléter

  xCurrentTick = xTaskGetTickCountFromISR();
  if (pdTICKS_TO_MS(xCurrentTick - xLastTick) > 200) {
    xSemaphoreGiveFromISR(syncSema,&xHigherPriorityTaskWoken);
  }
  xLastTick = xCurrentTick;
  if(xHigherPriorityTaskWoken){
    portYIELD_FROM_ISR();
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PUSH_BUTTON_S1, INPUT);
  
  syncSema = xSemaphoreCreateBinary();

  xTaskCreatePinnedToCore(
    waitingTask,
    "Waiting task",
    8192,
    nullptr,
    5,
    nullptr,
    APP_CORE
  );

  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_S1), toggleLED, FALLING);       //A compléter
}

void loop() { 
    vTaskDelete(nullptr); 
}

void waitingTask(void *pvParameters) {

  for (;;) {
    xSemaphoreTake(syncSema,portMAX_DELAY);
    Serial.println("Task did wake up");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}
