#include "services/OswServiceTasks.h"

#include "services/OswServiceTaskBLECompanion.h"
#include "services/OswServiceTaskBLEServer.h"
#include "services/OswServiceTaskExample.h"
#include "services/OswServiceTaskGPS.h"
#include "services/OswServiceTaskMemMonitor.h"
#include "services/OswServiceTaskNotifier.h"
#include "services/OswServiceTaskConsole.h"
#ifdef OSW_FEATURE_WIFI
#include "services/OswServiceTaskWiFi.h"
#include "services/OswServiceTaskWebserver.h"
#endif
#include "osw_util.h"

namespace OswServiceAllTasks {
#ifdef OSW_SERVICE_EXAMPLE
OswServiceTaskExample example;
#endif
// OswServiceTaskExample example;
#if SERVICE_BLE_COMPANION == 1
OswServiceTaskBLECompanion bleCompanion;
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

OswServiceTaskGPS gps;
#endif
#ifdef OSW_FEATURE_WIFI
OswServiceTaskWiFi wifi;
OswServiceTaskWebserver webserver;
#endif
#ifdef OSW_FEATURE_BLE_SERVER
OswServiceTaskBLEServer bleServer;
#endif
#if OSW_SERVICE_NOTIFIER == 1
OswServiceTaskNotifier notifier;
#endif
#ifndef OSW_EMULATOR
OswServiceTaskMemMonitor memory;
#endif
#ifdef OSW_SERVICE_CONSOLE
OswServiceTaskConsole console;
#endif
} // namespace OswServiceAllTasks

OswServiceTask* oswServiceTasks[] = {
#if SERVICE_BLE_COMPANION == 1
    & OswServiceAllTasks::bleCompanion,
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

    & OswServiceAllTasks::gps,
#endif
#ifdef OSW_SERVICE_EXAMPLE
    & OswServiceAllTasks::example,
#endif
#ifdef OSW_FEATURE_WIFI
    & OswServiceAllTasks::wifi, &OswServiceAllTasks::webserver,
#endif
#ifdef OSW_FEATURE_BLE_SERVER
    & OswServiceAllTasks::bleServer,
#endif
#if OSW_SERVICE_NOTIFIER == 1
    & OswServiceAllTasks::notifier,
#endif
#ifdef OSW_SERVICE_CONSOLE
    & OswServiceAllTasks::console,
#endif
#ifndef OSW_EMULATOR
#ifndef NDEBUG
    & OswServiceAllTasks::memory
#endif
#else
    nullptr // To prevent static array with size zero
#endif
};
const unsigned char oswServiceTasksCount = OswUtil::size(oswServiceTasks);
