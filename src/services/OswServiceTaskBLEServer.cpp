#ifdef OSW_FEATURE_BLE_SERVER
#include "./services/OswServiceTaskBLEServer.h"
#include "osw_hal.h"

#define BATTERY_SERVICE_UUID "0000180F-0000-1000-8000-00805f9b34fb"
#define BATTERY_LEVEL_CHARACTERISTIC_UUID "00002A19-0000-1000-8000-00805f9b34fb"

void OswServiceTaskBLEServer::setup() {
    OswServiceTask::setup();
    this->updateName();

    NimBLEDevice::setSecurityAuth(true, false, false); // support bonding, but no mitm-protection or secure pairing
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO); // we can display yes/no with a given key to the user

    // Create the BLE Server
    server = NimBLEDevice::getServer();
    if(server == nullptr) {
        OSW_LOG_D("No server found, creating a new one.");
        server = NimBLEDevice::createServer();
    }
    server->setCallbacks(this); // make sure, we are the servers authority

    {
        // Create the BLE Service
        serviceBat = server->createService(BATTERY_SERVICE_UUID);

        // Create a BLE Characteristic
        characteristicBat = serviceBat->createCharacteristic(
                                BATTERY_LEVEL_CHARACTERISTIC_UUID,
                                NIMBLE_PROPERTY::READ
                            );
        characteristicBat->setCallbacks(&battery);

        // Start the service
        serviceBat->start();
    }

    // Start advertising
    {
        BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
        pAdvertising->setScanResponse(false);
        /** Note, this could be left out as that is the default value */
        pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
        BLEDevice::startAdvertising();
    }
}

void OswServiceTaskBLEServer::loop() {

}

void OswServiceTaskBLEServer::stop() {
    OswServiceTask::stop();

    BLEDevice::stopAdvertising();
    serviceBat->removeCharacteristic(characteristicBat, true);
    server->removeService(serviceBat, true);
}

void OswServiceTaskBLEServer::updateName() {
    memset(this->name, 0, 8); // clear the name buffer
    strncpy(this->name, OswConfigAllKeys::hostname.get().c_str(), 8);
    NimBLEDevice::init(this->name);
}

void OswServiceTaskBLEServer::onConnect(BLEServer* pServer) {
    OSW_LOG_D("A client has connected!");
}

void OswServiceTaskBLEServer::onDisconnect(BLEServer* pServer) {
    OSW_LOG_D("A client has disconnected!");
}

uint32_t OswServiceTaskBLEServer::onPassKeyRequest() {
    // TODO connecting client asked for a pin to confirm
    OSW_LOG_I("Server PassKeyRequest: ", 123456);
    return 123456;
}

bool OswServiceTaskBLEServer::onConfirmPIN(uint32_t pass_key) {
    // TODO user must confirm if this is the correct key sent by the other device
    OSW_LOG_I("The passkey YES/NO number: ", pass_key);
    return true;
}


void OswServiceTaskBLEServer::BatteryCharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic) {
    // get the current battery level into the inner byte buffer
    if(OswHal::getInstance()->isCharging()) {
        byte = 255; // invalid value
    } else {
        this->byte = OswHal::getInstance()->getBatteryPercent();
    }
    pCharacteristic->setValue(&this->byte, 1);
}
#endif
