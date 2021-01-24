#include <Arduino.h>
#include <Wire.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

#include "./apps/main/stopwatch.h"
#include "./apps/main/watchface.h"
#include "./apps/tools/ble_media_ctrl.h"
#include "./apps/tools/print_debug.h"
#include "./apps/tools/water_level.h"

#define BTN_1_APP_SWITCH_TIMEOUT 1000
#define BTN_1_SLEEP_TIMEOUT 3000

OswHal *hal = new OswHal();
OswAppBLEMEdiaCtrl *bleCtrl = new OswAppBLEMEdiaCtrl();

// HINT: NUM_APPS must match the number of apps below!
#define NUM_APPS 3
uint8_t appPtr = 0;
OswApp *mainApps[] = {
    new OswAppWatchface(),  //
    new OswAppStopWatch(),  //
    new OswAppWaterLevel()  //
};

#if defined(GPS_EDITION)
#include "esp_task_wdt.h"
TaskHandle_t Task1;
void backgroundLoop(void *pvParameters) {
  while (true) {
    hal->gpsParse();
    delay(1);
  }
}
#endif

void IRAM_ATTR isrStepDetect() { Serial.println("Step"); }

void setup() {
  Serial.begin(115200);

  hal->setupPower();
  hal->setupButtons();
  hal->setupSensors();
  hal->setupDisplay();
  hal->setBrightness(128);

#if defined(GPS_EDITION)
  hal->setupGps();
  hal->setupSD();

  xTaskCreatePinnedToCore(backgroundLoop, "backgroundLoop", 1000 /*stack*/, NULL /*input*/, 0 /*prio*/,
                          &Task1 /*handle*/, 0);
#endif

  mainApps[appPtr]->setup(hal);
}

bool printDebugInfo = false;
void loop() {
  static long lastFlush = 0;

  hal->checkButtons();
  hal->updateAccelerometer();

  // handle long click to sleep
  if (hal->btn1Down() >= BTN_1_SLEEP_TIMEOUT) {
    hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
    hal->flushCanvas();
    hal->deepSleep();
  }

  // handle medium click to switch
  if (hal->btn1Down() >= BTN_1_APP_SWITCH_TIMEOUT) {
    // switch app
    mainApps[appPtr]->stop(hal);
    appPtr++;
    appPtr %= NUM_APPS;
    mainApps[appPtr]->setup(hal);
  }

  mainApps[appPtr]->loop(hal);

  // limit to 30 fps and handle display flushing
  if (millis() - lastFlush > 1000 / 30 && hal->isRequestFlush()) {
    hal->flushCanvas();
    lastFlush = millis();
  }
}
