#ifdef OSW_FEATURE_BLE_SERVER
#include "./services/OswServiceTaskBLEServer.h"
#include "osw_hal.h"

#define BATTERY_SERVICE_UUID                         "0000180f-0000-1000-8000-00805f9b34fb"
#define BATTERY_LEVEL_CHARACTERISTIC_UUID            "00002A19-0000-1000-8000-00805f9b34fb"
#define BATTERY_LEVEL_STATUS_CHARACTERISTIC_UUID     "00002bed-0000-1000-8000-00805f9b34fb"
#define TIME_SERVICE_UUID                            "00001805-0000-1000-8000-00805f9b34fb"
#define CURRENT_TIME_CHARACTERISTIC_UUID             "00002a0c-0000-1000-8000-00805f9b34fb"
#define DEVICE_INFORMATION_SERVICE_UUID              "0000180a-0000-1000-8000-00805f9b34fb"
#define FIRMWARE_REVISION_CHARACTERISTIC_UUID        "00002a26-0000-1000-8000-00805f9b34fb"
#define HARDWARE_REVISION_CHARACTERISTIC_UUID        "00002a27-0000-1000-8000-00805f9b34fb"
#define SOFTWARE_REVISION_CHARACTERISTIC_UUID        "00002a28-0000-1000-8000-00805f9b34fb"

#define OSW_SERVICE_UUID                             "6ab9b834-3e0b-4770-a938-ebaa58d6b852"
#define TOAST_CHARACTERISTIC_UUID                    "9cadf570-4ec8-40b0-a8d3-cfdb4a90940b"
#define STEP_COUNT_AVERAGE_CHARACTERISTIC_UUID       "3d27dd91-b714-4fe4-8067-620ff7cf13c0"
#define STEP_COUNT_TOTAL_CHARACTERISTIC_UUID         "6567e61b-41df-4416-96fc-4e068628bd37"
#define STEP_COUNT_TOTAL_WEEK_CHARACTERISTIC_UUID    "0b97315f-883b-4e1e-a745-bc2dd14aedb4"
#define STEP_COUNT_TODAY_CHARACTERISTIC_UUID         "143a6279-67ce-43c2-8db2-082a9fbca140"
#define STEP_COUNT_DAY_HISTORY_CHARACTERISTIC_UUID   "6b078d24-79ae-4fff-bf3a-2b71dce2b2bb"

void OswServiceTaskBLEServer::setup() {
    OswServiceTask::setup();
    this->bootDone = false;
}

void OswServiceTaskBLEServer::loop() {
    if(!this->bootDone) {
        if(OswConfigAllKeys::bleBootEnabled.get()) {
            this->enable();
        } else {
            this->disable();
        }
        this->bootDone = true;
    }
    this->updateBLEConfig();
}

void OswServiceTaskBLEServer::stop() {
    OswServiceTask::stop();
    this->disable();
    this->updateBLEConfig();
}

void OswServiceTaskBLEServer::enable() {
    this->enabled = true;
}

void OswServiceTaskBLEServer::disable() {
    this->enabled = false;
}

bool OswServiceTaskBLEServer::isEnabled() {
    return this->enabled;
}

void OswServiceTaskBLEServer::updateBLEConfig() {
    if(this->enabled and this->server == nullptr) {
        NimBLEDevice::init(this->name);
        OSW_LOG_D("On");
        this->updateName();

        NimBLEDevice::setSecurityAuth(true, true, true); // support bonding, with mitm-protection and secure pairing
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY); // we can display yes/no with a given key to the user

        // Create the BLE Server
        this->server = NimBLEDevice::getServer();
        if(this->server == nullptr) {
            OSW_LOG_D("No server found, creating a new one.");
            this->server = NimBLEDevice::createServer();
        }
        this->server->setCallbacks(new ServerCallbacks(this)); // make sure, we are the servers authority

        {
            // Create the BLE Service: Battery
            this->serviceBat = this->server->createService(BATTERY_SERVICE_UUID);

            // Create a BLE Characteristic: "Battery Level"
            this->characteristicBat = serviceBat->createCharacteristic(
                                          BATTERY_LEVEL_CHARACTERISTIC_UUID,
                                          NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                                      );
            this->characteristicBat->setCallbacks(new BatteryLevelCharacteristicCallbacks());

            // Create a BLE Characteristic: "Battery Level Status"
            this->characteristicBatStat = serviceBat->createCharacteristic(
                                              BATTERY_LEVEL_STATUS_CHARACTERISTIC_UUID,
                                              NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                                          );
            this->characteristicBatStat->setCallbacks(new BatteryLevelStatusCharacteristicCallbacks());

            // Start the service
            this->serviceBat->start();
        }

        {
            // Create the BLE Service: Time
            this->serviceTime = this->server->createService(TIME_SERVICE_UUID);

            // Create a BLE Characteristic: "Current Time"
            this->characteristicCurTime = serviceTime->createCharacteristic(
                                              CURRENT_TIME_CHARACTERISTIC_UUID,
                                              NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                                          );
            this->characteristicCurTime->setCallbacks(new CurrentTimeCharacteristicCallbacks());

            // Start the service
            this->serviceTime->start();
        }

        {
            // Create the BLE Service: Device Information
            this->serviceDevice = this->server->createService(DEVICE_INFORMATION_SERVICE_UUID);

            // Create a BLE Characteristic: "Firmware Revision String"
            this->characteristicFirmRev = serviceDevice->createCharacteristic(
                                              FIRMWARE_REVISION_CHARACTERISTIC_UUID,
                                              NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                                          );
            this->characteristicFirmRev->setCallbacks(new FirmwareRevisionCharacteristicCallbacks());

            // Create a BLE Characteristic: "Hardware Revision String"
            this->characteristicHardRev = serviceDevice->createCharacteristic(
                                              HARDWARE_REVISION_CHARACTERISTIC_UUID,
                                              NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                                          );
            this->characteristicHardRev->setCallbacks(new HardwareRevisionCharacteristicCallbacks());

            // Create a BLE Characteristic: "Software Revision String"
            this->characteristicSoftRev = serviceDevice->createCharacteristic(
                                              SOFTWARE_REVISION_CHARACTERISTIC_UUID,
                                              NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                                          );
            this->characteristicSoftRev->setCallbacks(new SoftwareRevisionCharacteristicCallbacks());

            // Start the service
            this->serviceDevice->start();
        }

        {
            // Create the BLE Service: OSW
            this->serviceOsw = this->server->createService(OSW_SERVICE_UUID);

            // Create a BLE Characteristic: "Toast"
            this->characteristicToast = this->serviceOsw->createCharacteristic(
                                            TOAST_CHARACTERISTIC_UUID,
                                            NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN
                                        );
            this->characteristicToast->setCallbacks(new ToastCharacteristicCallbacks(this));

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
            this->characteristicStepCountToday = this->serviceOsw->createCharacteristic(
                    STEP_COUNT_TODAY_CHARACTERISTIC_UUID,
                    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                                                 );
            this->characteristicStepCountToday->setCallbacks(new StepsTodayCharacteristicCallbacks());

            this->characteristicStepCountTotal = this->serviceOsw->createCharacteristic(
                    STEP_COUNT_TOTAL_CHARACTERISTIC_UUID,
                    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                                                 );
            this->characteristicStepCountTotal->setCallbacks(new StepsTotalCharacteristicCallbacks());

            this->characteristicStepCountTotalWeek = this->serviceOsw->createCharacteristic(
                        STEP_COUNT_TOTAL_WEEK_CHARACTERISTIC_UUID,
                        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                    );
            this->characteristicStepCountTotalWeek->setCallbacks(new StepsTotalWeekCharacteristicCallbacks());
#ifdef OSW_FEATURE_STATS_STEPS
            this->characteristicStepCountAverage = this->serviceOsw->createCharacteristic(
                    STEP_COUNT_AVERAGE_CHARACTERISTIC_UUID,
                    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                                                   );
            this->characteristicStepCountAverage->setCallbacks(new StepsAverageCharacteristicCallbacks());

            this->characteristicStepCountHistory = this->serviceOsw->createCharacteristic(
                    STEP_COUNT_DAY_HISTORY_CHARACTERISTIC_UUID,
                    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN
                                                   );
            this->characteristicStepCountHistory->setCallbacks(new StepsDayHistoryCharacteristicCallbacks());
#endif
#endif
            // Start the service
            this->serviceOsw->start();
        }

        // Start advertising
        {
            BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
            pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
            pAdvertising->addServiceUUID(TIME_SERVICE_UUID);
            pAdvertising->addServiceUUID(DEVICE_INFORMATION_SERVICE_UUID);
            pAdvertising->addServiceUUID(OSW_SERVICE_UUID);
            pAdvertising->setScanResponse(false);
            /** Note, this could be left out as that is the default value */
            pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
            NimBLEDevice::startAdvertising();
        }
    } else if(!this->enabled and this->server != nullptr) {
        OSW_LOG_D("Off");
        BLEDevice::stopAdvertising();
        this->serviceBat->removeCharacteristic(this->characteristicBat, true);
        this->serviceBat->removeCharacteristic(this->characteristicBatStat, true);
        this->server->removeService(this->serviceBat, true);
        this->serviceTime->removeCharacteristic(this->characteristicCurTime, true);
        this->server->removeService(this->serviceTime, true);
        this->serviceDevice->removeCharacteristic(this->characteristicFirmRev, true);
        this->serviceDevice->removeCharacteristic(this->characteristicHardRev, true);
        this->serviceDevice->removeCharacteristic(this->characteristicSoftRev, true);
        this->server->removeService(this->serviceDevice, true);
        this->serviceOsw->removeCharacteristic(this->characteristicToast, true);
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
        this->serviceOsw->removeCharacteristic(this->characteristicStepCountToday, true);
        this->serviceOsw->removeCharacteristic(this->characteristicStepCountTotal, true);
        this->serviceOsw->removeCharacteristic(this->characteristicStepCountTotalWeek, true);
#ifdef OSW_FEATURE_STATS_STEPS
        this->serviceOsw->removeCharacteristic(this->characteristicStepCountAverage, true);
        this->serviceOsw->removeCharacteristic(this->characteristicStepCountHistory, true);
#endif
#endif
        this->server->removeService(this->serviceOsw, true);
        this->server = nullptr;
        NimBLEDevice::deinit(true);
    }
}

void OswServiceTaskBLEServer::updateName() {
    memset(this->name, 0, 8); // clear the name buffer
    strncpy(this->name, OswConfigAllKeys::hostname.get().c_str(), 8);
    NimBLEDevice::setDeviceName(this->name);
}

void OswServiceTaskBLEServer::ServerCallbacks::onConnect(BLEServer* pServer) {
    OSW_LOG_D("A client has connected (", pServer->getConnectedCount(), ")!");
    task->notify.showToast("BLE connected");
}

void OswServiceTaskBLEServer::ServerCallbacks::onDisconnect(BLEServer* pServer) {
    OSW_LOG_D("A client has disconnected (", pServer->getConnectedCount(), ")!");
    task->notify.showToast("BLE disconnected");
}

uint32_t OswServiceTaskBLEServer::ServerCallbacks::onPassKeyRequest() {
    // roll a new passkey
    long passKey = random(100000, 999999);
    OSW_LOG_I("Server got a passkey request: ", passKey);
    // ...and toast it to the user
    task->notify.showToast("Validate BLE pass-key:\n" + std::to_string(passKey));
    return passKey;
}

void OswServiceTaskBLEServer::BatteryLevelCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    // get the current battery level into the inner byte buffer
    if(OswHal::getInstance()->isCharging()) {
        byte = 0xFF; // invalid value
    } else {
        this->byte = OswHal::getInstance()->getBatteryPercent();
    }
    pCharacteristic->setValue(&this->byte, sizeof(this->byte));
}

void OswServiceTaskBLEServer::BatteryLevelStatusCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    // see https://www.bluetooth.com/specifications/specs/battery-service/
    // see https://www.bluetooth.com/specifications/specs/gatt-specification-supplement-8-2/
    bool isCharging = OswHal::getInstance()->isCharging();
    // flags
    if(isCharging) {
        this->bytes[0] = 0b00000000; // No additional information
    } else {
        this->bytes[0] = 0b00000010; // Battery Level Present
    }
    // power-state
    this->bytes[1] = 0b00000001; // Battery Present: Yes
    this->bytes[2] = 0b00000000;
    if(isCharging) {
        this->bytes[1] |= 0b00000010; // Wired External Power Source Connected: Yes
        this->bytes[1] |= 0b00100000; // Battery Charge State: Charging
    } else {
        this->bytes[1] |= 0b01000000; // Battery Charge State: Discharging: Active
        if(OswHal::getInstance()->getBatteryPercent() > 50) {
            this->bytes[1] |= 0b10000000; // Battery Charge Level: Good
        } else if(OswHal::getInstance()->getBatteryPercent() > 25) {
            // Battery Charge Level: Low
            this->bytes[1] |= 0b00000000;
            this->bytes[2] |= 0b00000001;
        } else {
            // Battery Charge Level: Critical
            this->bytes[1] |= 0b10000000;
            this->bytes[2] |= 0b00000001;
        }
    }
    // battery-level
    if(isCharging) {
        this->bytes[3] = 0xFF; // invalid value (should not be sent if charging)
    } else {
        this->bytes[3] = OswHal::getInstance()->getBatteryPercent();
    }
    pCharacteristic->setValue(this->bytes, isCharging ? (sizeof(this->bytes) - 1) : sizeof(this->bytes));
}

void OswServiceTaskBLEServer::CurrentTimeCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    time_t offset = 0;
    OswDate oswDate;
    OswHal::getInstance()->getDate(offset, oswDate);
    OswTime oswTime;
    OswHal::getInstance()->getTime(offset, oswTime);

    this->bytes[0] = (uint8_t) oswDate.year; // Exact Time 256 -> Day Date Time -> Date Time -> Year #1
    this->bytes[1] = (uint8_t) (oswDate.year >> 8); // Exact Time 256 -> Day Date Time -> Date Time -> Year #2
    this->bytes[2] = (uint8_t) oswDate.month; // Exact Time 256 -> Day Date Time -> Date Time -> Month
    this->bytes[3] = (uint8_t) oswDate.day; // Exact Time 256 -> Day Date Time -> Date Time -> Day
    this->bytes[4] = (uint8_t) oswTime.hour; // Exact Time 256 -> Day Date Time -> Date Time -> Hours
    this->bytes[5] = (uint8_t) oswTime.minute; // Exact Time 256 -> Day Date Time -> Date Time -> Minutes
    this->bytes[6] = (uint8_t) oswTime.second; // Exact Time 256 -> Day Date Time -> Date Time -> Seconds
    this->bytes[7] = (uint8_t) (oswDate.weekDay == 0 ? 7 : oswDate.weekDay); // Exact Time 256 -> Day Date Time -> Day of Week
    this->bytes[8] = 0b00000000; // Exact Time 256 -> Fractions256
    this->bytes[9] = 0b00000001; // Adjust Reason: External Reference Time Update
    pCharacteristic->setValue(this->bytes, sizeof(this->bytes));
}

void OswServiceTaskBLEServer::FirmwareRevisionCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    pCharacteristic->setValue((uint8_t*) this->value.c_str(), this->value.length());
}

void OswServiceTaskBLEServer::HardwareRevisionCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    this->value = String(PIO_ENV_NAME);
    pCharacteristic->setValue((uint8_t*) this->value.c_str(), this->value.length());
}
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
void OswServiceTaskBLEServer::StepsTodayCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    uint32_t stepsToday = OswHal::getInstance()->environment()->getStepsToday();

    this->bytes[0] = (uint8_t) stepsToday;
    this->bytes[1] = (uint8_t) (stepsToday >> 8);
    this->bytes[2] = (uint8_t) (stepsToday >> 16);
    this->bytes[3] = (uint8_t) (stepsToday >> 24);

    pCharacteristic->setValue(this->bytes, sizeof(this->bytes));
}

void OswServiceTaskBLEServer::StepsTotalCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    uint32_t stepsTotal = OswHal::getInstance()->environment()->getStepsTotal();

    this->bytes[0] = (uint8_t) stepsTotal;
    this->bytes[1] = (uint8_t) (stepsTotal >> 8);
    this->bytes[2] = (uint8_t) (stepsTotal >> 16);
    this->bytes[3] = (uint8_t) (stepsTotal >> 24);

    pCharacteristic->setValue(this->bytes, sizeof(this->bytes));
}

void OswServiceTaskBLEServer::StepsTotalWeekCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    uint32_t stepsTotalWeek = OswHal::getInstance()->environment()->getStepsTotalWeek();

    this->bytes[0] = (uint8_t) stepsTotalWeek;
    this->bytes[1] = (uint8_t) (stepsTotalWeek >> 8);
    this->bytes[2] = (uint8_t) (stepsTotalWeek >> 16);
    this->bytes[3] = (uint8_t) (stepsTotalWeek >> 24);

    pCharacteristic->setValue(this->bytes, sizeof(this->bytes));
}
#ifdef OSW_FEATURE_STATS_STEPS
void OswServiceTaskBLEServer::StepsAverageCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    uint32_t stepsAverage = OswHal::getInstance()->environment()->getStepsAverage();

    this->bytes[0] = (uint8_t) stepsAverage;
    this->bytes[1] = (uint8_t) (stepsAverage >> 8);
    this->bytes[2] = (uint8_t) (stepsAverage >> 16);
    this->bytes[3] = (uint8_t) (stepsAverage >> 24);

    pCharacteristic->setValue(this->bytes, sizeof(this->bytes));
}

void OswServiceTaskBLEServer::StepsDayHistoryCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {

    for (uint8_t indexOfWeek = 0; indexOfWeek < 7; indexOfWeek++) {
        uint32_t value = OswHal::getInstance()->environment()->getStepsOnDay(indexOfWeek, false);
        this->bytes[0 + indexOfWeek * 4] = (uint8_t) value;
        this->bytes[1 + indexOfWeek * 4] = (uint8_t) (value >> 8);
        this->bytes[2 + indexOfWeek * 4] = (uint8_t) (value >> 16);
        this->bytes[3 + indexOfWeek * 4] = (uint8_t) (value >> 24);
    }

    pCharacteristic->setValue(this->bytes, sizeof(this->bytes));
}
#endif
#endif
void OswServiceTaskBLEServer::SoftwareRevisionCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    this->value = String(GIT_COMMIT_HASH) + " (" + String(GIT_BRANCH_NAME) + ")";
    pCharacteristic->setValue((uint8_t*) this->value.c_str(), this->value.length());
}

void OswServiceTaskBLEServer::ToastCharacteristicCallbacks::onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    OSW_LOG_I("BLE Toast: ", value);
    task->notify.showToast(value);
}
#endif
