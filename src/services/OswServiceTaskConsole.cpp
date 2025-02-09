#include <OswSerial.h>
#include "./services/OswServiceTaskConsole.h"
#include "osw_hal.h"
#include <services/OswServiceTasks.h>
#include <services/OswServiceTaskBLEServer.h>
#include <services/NotifierClient.h>

void OswServiceTaskConsole::setup() {
    OswServiceTask::setup();
    OSW_LOG_I("Console is now enabled.");
#ifdef NDEBUG
    this-> m_locked = true; // in release mode the console is locked by default
#endif
    this->newPrompt();
}

void OswServiceTaskConsole::loop() {
    while(true) {
        OswSerial* serial = OswSerial::getInstance();
        char c;
        if(!serial->getc((uint8_t&) c)) break;
        switch (c) {
        case 10: // LF
        case 13:  // CR
            this->runPrompt();
            this->newPrompt();
            break;
        case 8: // backspace
        case 127: // delete
        case 27: // escape
            if(this->m_inputBuffer.length() > 0) {
                this->m_inputBuffer.pop_back();
            }
            serial->print(c);
            break;
        case 9: // tab
            serial->println(); // finish the prompt line
            this->showHelp();
            this->showPrompt();
            break;
        default:
            if(32 <= c and c <= 126) { // printable characters
                this->m_inputBuffer += c;
                serial->print(c); // echo the entered character back
            } else {
                serial->print((char) 0x07); // bell
                OSW_LOG_D("Unprocessable character (",(int) c, "): ", c);
            }
        }
    }
}

void OswServiceTaskConsole::newPrompt() {
    this->m_inputBuffer.clear();
    if(this->m_locked) return;
    this->showPrompt();
}

void OswServiceTaskConsole::showPrompt() {
    OswSerial* serial = OswSerial::getInstance();
    switch(this->m_mode) {
    case Mode::Generic:
        serial->print("OSW > ");
        break;
    case Mode::Configuration:
        serial->print("OSW (configure) > ");
        break;
#ifdef OSW_FEATURE_BLE_SERVER
    case Mode::Ble:
        serial->print("OSW (BLE) > ");
        break;
#endif
    default:
        assert(false && "Unknown console mode?!");
    }
    if(!this->m_inputBuffer.empty()) {
        serial->print(this->m_inputBuffer.c_str());
    }
}

void OswServiceTaskConsole::runPrompt() {
    OswSerial* serial = OswSerial::getInstance();
    serial->println(); // finish the prompt line
    if(this->m_inputBuffer.empty()) return;
    if(this->m_locked) {
        serial->println("Console is locked! Enter \"unlock\" to unlock.");
        if(this->m_inputBuffer == "unlock") {
            this->m_locked = false;
        }
        return;
    }
    // command convention: show only what was asked (preferably machine readable), only on error be verbose
    OswHal::getInstance()->noteUserInteraction(); // console input also counts as user interaction
    bool processed = true;
    switch (this->m_mode) {
    case Mode::Generic:
#ifdef OSW_FEATURE_BLE_SERVER
        if (this->m_inputBuffer == "ble") {
            this->m_mode = Mode::Ble;
        } else if (this->m_inputBuffer == "configure") {
#else
        if (this->m_inputBuffer == "configure") {
#endif
            this->m_mode = Mode::Configuration;
        } else if (this->m_inputBuffer == "help") {
            this->showHelp();
#if defined(OSW_FEATURE_WIFI) || defined(OSW_FEATURE_BLE_SERVER)
        } else if (this->m_inputBuffer == "hostname") {
            serial->println(OswConfigAllKeys::hostname.get());
#endif
        } else if (this->m_inputBuffer == "lock") {
            this->m_locked = true;
#ifndef OSW_EMULATOR
        } else if (this->m_inputBuffer == "reboot") {
            // this does not work in the emulator as it is running under an own thread, of which the shutdown-exception is not captured - populating here and crashing
            ESP.restart();
#endif
#if OSW_SERVICE_NOTIFIER == 1
        } else if (this->m_inputBuffer.find("toast ") == 0 and this->m_inputBuffer.length() > 6) {
            auto arg = this->m_inputBuffer.substr(6);
            std::string toast;
            // iterate over the string and replace "\n" with the newline character
            for (auto i = 0; i < arg.length(); i++) {
                if (i + 1 < arg.length() and arg[i] == '\\' and arg[i + 1] == 'n') {
                    toast += '\n';
                    i++;
                } else {
                    toast += arg[i];
                }
            }
            NotifierClient notify("osw.console");
            notify.showToast(toast);
#endif
        } else if (this->m_inputBuffer == "time") {
            serial->println(OswHal::getInstance()->getUTCTime());
        } else if (this->m_inputBuffer == "wipe") {
            OswConfig::getInstance()->reset(true);
        }  else {
            processed = false;
        }
        break;
    case Mode::Configuration:
        if (this->m_inputBuffer == "clear") {
            OswConfig::getInstance()->reset(false);
        } else if (this->m_inputBuffer == "exit") {
            this->m_mode = Mode::Generic;
        } else if (this->m_inputBuffer.find("get ") == 0 and this->m_inputBuffer.length() > 4) {
            auto key = this->m_inputBuffer.substr(4);
            for (auto i = 0; i < oswConfigKeysCount; i++) {
                if(oswConfigKeys[i]->id == key) {
                    serial->println(oswConfigKeys[i]->toString());
                    break;
                }
            }
        } else if (this->m_inputBuffer.find("info ") == 0 and this->m_inputBuffer.length() > 5) {
            auto key = this->m_inputBuffer.substr(5);
            serial->println(OswConfig::getInstance()->getFieldJson(key.c_str()));
        } else if (this->m_inputBuffer == "help") {
            this->showHelp();
        } else if (this->m_inputBuffer == "list") {
            for (auto i = 0; i < oswConfigKeysCount; i++) {
                serial->println(oswConfigKeys[i]->id);
            }
        } else if (this->m_inputBuffer.find("reset ") == 0 and this->m_inputBuffer.length() > 7) {
            auto key = this->m_inputBuffer.substr(7);
            OswConfig::getInstance()->enableWrite();
            OswConfig::getInstance()->resetField(key.c_str());
            OswConfig::getInstance()->disableWrite();
        } else if (this->m_inputBuffer.find("set ") == 0 and this->m_inputBuffer.length() > 5) {
            auto key = this->m_inputBuffer.substr(4); // "<id> <value>"
            auto space = key.find(" ", 0);
            if (space == std::string::npos) {
                serial->println("Invalid command.");
                return;
            }
            auto value = key.substr(space + 1); // "<value>"
            key = key.substr(0, space); // "<id>"
            OswConfig::getInstance()->enableWrite();
            OswConfig::getInstance()->setField(key.c_str(), value.c_str());
            OswConfig::getInstance()->disableWrite();
        } else {
            processed = false;
        }
        break;
#ifdef OSW_FEATURE_BLE_SERVER
    case Mode::Ble:
        if (this->m_inputBuffer == "disable") {
            OswServiceAllTasks::bleServer.disable();
        } else if (this->m_inputBuffer == "enable") {
            OswServiceAllTasks::bleServer.enable();
        } else if (this->m_inputBuffer == "exit") {
            this->m_mode = Mode::Generic;
        } else if (this->m_inputBuffer == "help") {
            this->showHelp();
        } else if (this->m_inputBuffer == "status") {
            serial->println(OswServiceAllTasks::bleServer.isEnabled() ? "enabled" : "disabled");
        } else {
            processed = false;
        }
        break;
#endif
    default:
        assert(false && "Unknown console mode?!");
    }
    // show help if the command was not processed
    if (!processed) {
        serial->println("Unknown command.");
        serial->println();
        this->showHelp();
        this->m_lockCounter++;
    }
    // in case of garbage input, lock the console after 16 attempts
    if (this->m_lockCounter > 16) {
        this->m_locked = true;
        this->m_lockCounter = 0;
    }
}

void OswServiceTaskConsole::showHelp() {
    OswSerial* serial = OswSerial::getInstance();
    serial->println("Available commands:");
    // let's try to be civil and show the commands in alphabetical order
    switch (this->m_mode) {
    case Mode::Generic:
        serial->println("  ble         - enter BLE console-mode");
        serial->println("  configure   - enter configuration console-mode");
        serial->println("  help        - show this help");
#if defined(OSW_FEATURE_WIFI) || defined(OSW_FEATURE_BLE_SERVER)
        serial->println("  hostname    - show the device hostname");
#endif
        serial->println("  lock        - lock the console");
#ifndef OSW_EMULATOR
        serial->println("  reboot      - warm-start the device forcefully");
#endif
        serial->println("  time        - show current UTC time");
#if OSW_SERVICE_NOTIFIER == 1
        serial->println("  toast <msg> - send a toast notification (use \"\\n\" for new line)");
#endif
        serial->println("  wipe        - format NVS partition and reset configuration");
        break;
    case Mode::Configuration:
        serial->println("  clear             - reset all keys to default values");
        serial->println("  exit              - leave configuration mode");
        serial->println("  get <id>          - get a value for a key");
        serial->println("  help              - show this help");
        serial->println("  info <id>         - show more information about a key");
        serial->println("  list              - show all keys");
        serial->println("  reset <id>        - reset a key to default value");
        serial->println("  set <id> <value>  - set a value for a key (value is everything until the end of the line)");
        break;
#ifdef OSW_FEATURE_BLE_SERVER
    case Mode::Ble:
        Serial.println("  disable - disable BLE server");
        Serial.println("  enable  - enable BLE server");
        Serial.println("  exit    - leave BLE console-mode");
        Serial.println("  help    - show this help");
        Serial.println("  status  - show BLE server status");
        break;
#endif
    default:
        assert(false && "Unknown console mode?!");
    }
}

void OswServiceTaskConsole::stop() {
    OSW_LOG_I("Console is now disabled.");
    OswServiceTask::stop();
}
