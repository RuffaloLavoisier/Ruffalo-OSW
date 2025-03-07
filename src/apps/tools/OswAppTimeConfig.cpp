#include "./apps/tools/OswAppTimeConfig.h"

#include <config.h>
#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <osw_ui.h>
#include <services/OswServiceTasks.h>
#include <time.h>
#ifdef OSW_FEATURE_WIFI
#include <services/OswServiceTaskWiFi.h>
#endif

void OswAppTimeConfig::setup() {}

void OswAppTimeConfig::enterManualMode() {
    OswTime oswTime = { };
    OswDate oswDate = { };
    OswHal::getInstance()->getLocalTime(oswTime);
    OswHal::getInstance()->getLocalDate(oswDate);
    manualSettingTimestamp[0] = oswDate.year % 10;
    manualSettingTimestamp[1] = oswDate.month / 10;
    manualSettingTimestamp[2] = oswDate.month % 10;
    manualSettingTimestamp[3] = oswDate.day / 10;
    manualSettingTimestamp[4] = oswDate.day % 10;
    manualSettingTimestamp[5] = oswTime.hour / 10;
    manualSettingTimestamp[6] = oswTime.hour % 10;
    manualSettingTimestamp[7] = oswTime.minute / 10;
    manualSettingTimestamp[8] = oswTime.minute % 10;
    manualSettingTimestamp[9] = oswTime.second / 10;
    manualSettingTimestamp[10] = oswTime.second % 10;
    manualSettingScreen = true;
}

void OswAppTimeConfig::handleIncrementButton() {
    OswUI::getInstance()->setTextCursor(BUTTON_3);
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->print("+");
    if (manualSettingStep == 12) {  // SAVE
        if (hal->btnHasGoneDown(BUTTON_3)) {
            // Date
            int16_t yy = 2020 + manualSettingTimestamp[0];
            int8_t mm = manualSettingTimestamp[1] * 10 + manualSettingTimestamp[2] - 1;  // January = 0
            int8_t dd = manualSettingTimestamp[3] * 10 + manualSettingTimestamp[4];
            int8_t h = manualSettingTimestamp[5] * 10 + manualSettingTimestamp[6];
            int8_t m = manualSettingTimestamp[7] * 10 + manualSettingTimestamp[8];
            int8_t s = manualSettingTimestamp[9] * 10 + manualSettingTimestamp[10];
            struct tm date = {s, m, h, dd, mm, yy - 1900};
            time_t epoch = mktime(&date);

            hal->setUTCTime(epoch - hal->getTimezoneOffsetPrimary());
            manualSettingScreen = false;
        }
    } else if (manualSettingStep == 11) {  // CANCEL
        if (hal->btnHasGoneDown(BUTTON_3)) {
            manualSettingScreen = false;
        }
    } else {  // +1
        if (hal->btnHasGoneDown(BUTTON_3)) {
            manualSettingTimestamp[manualSettingStep]++;

            if (manualSettingStep == 1) {  // MONTHTEN
                if (manualSettingTimestamp[manualSettingStep] > 1) {
                    manualSettingTimestamp[manualSettingStep] = 0;
                }
            } else if (manualSettingStep == 3) {  // DAYTEN
                if (manualSettingTimestamp[manualSettingStep] > 3) {
                    manualSettingTimestamp[manualSettingStep] = 0;
                }
            } else if (manualSettingStep == 5) {  // HOURTEN
                if (manualSettingTimestamp[manualSettingStep] > 2) {
                    manualSettingTimestamp[manualSettingStep] = 0;
                }
            } else if (manualSettingStep == 7 || manualSettingStep == 9) {  // MINTEN or SECTEN
                if (manualSettingTimestamp[manualSettingStep] > 5) {
                    manualSettingTimestamp[manualSettingStep] = 0;
                }
            } else {
                if (manualSettingTimestamp[manualSettingStep] > 9) {  // other
                    manualSettingTimestamp[manualSettingStep] = 0;
                }
            }
        }
    }
}

void OswAppTimeConfig::handleDecrementButton() {
    OswHal* hal = OswHal::getInstance();
    OswUI::getInstance()->setTextCursor(BUTTON_2);
    hal->gfx()->print("-");

    // decrement should not saved - code has been removed versus incrementButton

    if (manualSettingStep == 11) {  // CANCEL
        if (hal->btnHasGoneDown(BUTTON_2)) {
            manualSettingScreen = false;
        }
    } else {  // -1
        if (hal->btnHasGoneDown(BUTTON_2)) {
            manualSettingTimestamp[manualSettingStep]--;

            if (manualSettingStep == 1) {  // MONTHTEN
                if (manualSettingTimestamp[manualSettingStep] < 0) {
                    manualSettingTimestamp[manualSettingStep] = 1;
                }
            } else if (manualSettingStep == 3) {  // DAYTEN
                if (manualSettingTimestamp[manualSettingStep] < 0) {
                    manualSettingTimestamp[manualSettingStep] = 3;
                }
            } else if (manualSettingStep == 5) {  // HOURTEN
                if (manualSettingTimestamp[manualSettingStep] < 0) {
                    manualSettingTimestamp[manualSettingStep] = 2;
                }
            } else if (manualSettingStep == 7 || manualSettingStep == 9) {  // MINTEN or SECTEN
                if (manualSettingTimestamp[manualSettingStep] < 0) {
                    manualSettingTimestamp[manualSettingStep] = 5;
                }
            } else {
                if (manualSettingTimestamp[manualSettingStep] < 0) {  // other
                    manualSettingTimestamp[manualSettingStep] = 9;
                }
            }
        }
    }
}

void OswAppTimeConfig::handleNextButton() {
    OswHal* hal = OswHal::getInstance();
    OswUI::getInstance()->setTextCursor(BUTTON_1);
    hal->gfx()->print(">");
    if (hal->btnHasGoneDown(BUTTON_1)) {
        manualSettingStep++;
        manualSettingStep = manualSettingStep > 12 ? 0 : manualSettingStep;
    }
}

void OswAppTimeConfig::loop() {
    OswHal* hal = OswHal::getInstance();
    const uint16_t colorActive = ui->getDangerColor();
    const uint16_t colorForeground = ui->getForegroundColor();
    const uint16_t colorBackground = ui->getBackgroundColor();

    hal->gfx()->setTextSize(2);
    if (!manualSettingScreen) {
        OswUI::getInstance()->setTextCursor(BUTTON_3);
#ifdef OSW_FEATURE_WIFI
        if (OswServiceAllTasks::wifi.isConnected()) {
            hal->gfx()->print(LANG_DISCONNECT);
        } else {
            if(OswServiceAllTasks::wifi.isEnabled())
                hal->gfx()->print("...");
            else {
                hal->gfx()->print(LANG_CONNECT);
                OswUI::getInstance()->setTextCursor(BUTTON_2);
                hal->gfx()->print(LANG_MANUALLY);
            }
        }

        if (hal->btnHasGoneDown(BUTTON_3)) {
            if (OswServiceAllTasks::wifi.isConnected()) {
                OswServiceAllTasks::wifi.disconnectWiFi();
                OswServiceAllTasks::wifi.disableWiFi();
            } else {
                OswServiceAllTasks::wifi.enableWiFi();
                OswServiceAllTasks::wifi.connectWiFi();
            }
        }

        if (OswServiceAllTasks::wifi.isConnected()) {
            OswUI::getInstance()->setTextCursor(BUTTON_2);
            hal->gfx()->print(LANG_TFW_UPDATE);
            if (hal->btnHasGoneDown(BUTTON_2)) {
                if (OswServiceAllTasks::wifi.isConnected()) {
                    OswServiceAllTasks::wifi.queueTimeUpdateViaNTP();
                }
            }
        } else {
            if (hal->btnHasGoneDown(BUTTON_2)) {
                enterManualMode();
            }
        }
#else
        OswUI::getInstance()->setTextCursor(BUTTON_2);
        hal->gfx()->print(LANG_MANUALLY);
        if (hal->btnHasGoneDown(BUTTON_2)) {
            enterManualMode();
        }
#endif

        hal->gfx()->setTextSize(4);
        hal->gfx()->setTextMiddleAligned();
        hal->gfx()->setTextLeftAligned();
        hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(4), 120);
        OswTime oswTime = { };
        hal->getLocalTime(oswTime);
        hal->gfx()->printDecimal(oswTime.hour, 2);
        hal->gfx()->print(":");
        hal->gfx()->printDecimal(oswTime.minute, 2);
        hal->gfx()->print(":");
        hal->gfx()->printDecimal(oswTime.second, 2);

    } else {
        handleIncrementButton();
        handleDecrementButton();
        handleNextButton();

        ui->resetTextColors();
        hal->gfx()->setTextSize(3);
        hal->gfx()->setTextMiddleAligned();
        hal->gfx()->setTextLeftAligned();

        // Date
        hal->gfx()->setTextCursor((DISP_W / 2) - hal->gfx()->getTextOfsetColumns(5), DISP_H * 3 / 8);
        hal->gfx()->print("202");
        for (int8_t i = 0; i < 5; i++) {
            hal->gfx()->setTextColor(i == manualSettingStep ? colorActive : colorForeground, colorBackground);
            hal->gfx()->print(manualSettingTimestamp[i]);
            if (i == 0 || i == 2) {
                ui->resetTextColors();
                hal->gfx()->print("-");
            }
        }

        // Time
        hal->gfx()->setTextCursor((DISP_W / 2) - hal->gfx()->getTextOfsetColumns(4), DISP_H / 2);
        for (int8_t i = 5; i < 11; i++) {
            hal->gfx()->setTextColor(i == manualSettingStep ? colorActive : colorForeground, colorBackground);
            hal->gfx()->print(manualSettingTimestamp[i]);
            if (i == 6 || i == 8) {
                ui->resetTextColors();
                hal->gfx()->print(":");
            }
        }

        hal->gfx()->setTextSize(2);

        // Cancel-Field
        hal->gfx()->setTextRightAligned();
        hal->gfx()->setTextCursor((DISP_W / 2) - hal->gfx()->getTextOfsetColumns(0.5f), DISP_H * 5 / 8);
        hal->gfx()->setTextColor(11 == manualSettingStep ? colorActive : colorForeground, colorBackground);
        hal->gfx()->print(LANG_CANCEL);

        // Done-Field
        hal->gfx()->setTextLeftAligned();
        hal->gfx()->setTextCursor((DISP_W / 2) + hal->gfx()->getTextOfsetColumns(0.5f), DISP_H * 5 / 8);
        hal->gfx()->setTextColor(12 == manualSettingStep ? colorActive : colorForeground, colorBackground);
        hal->gfx()->print(LANG_SAVE);
    }
}
void OswAppTimeConfig::stop() {}
