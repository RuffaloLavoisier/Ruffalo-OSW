#ifndef CONFIG_DEFAULTS_H
#define CONFIG_DEFAULTS_H

/*
 * Do not edit this file. You can add a config.h (cp config.h.example config.h)
 * and change the values there.
 */
#if __has_include("config.h")
#include "config.h"
#endif

/*
 * Internals:
 * You can ignore these ;).
 */

#ifndef DISP_W
#define DISP_W 240
#endif
#ifndef DISP_H
#define DISP_H 240
#endif

// !! IMPORTANT: DISP_H must be divisible by DISP_CHUNK_H !!
#ifndef DISP_CHUNK_H
#define DISP_CHUNK_H 8
#endif

/*
 * Language:
 * Here you can select the language of the compiled os. By compiling the language directly
 * into the os you loose the ability to switch it later on, but the compiled binary is way
 * smaller (and faster)!
 */
#ifndef LOCALE
#include "locales/en-US.h"
#else
#include LOCALE
#endif

/*
 * OSW Config Values (see osw_config_keys.cpp for description). All these values can be configured at run time with the
 * config app.
 */

#ifndef DEVICE_NAME
#define DEVICE_NAME "MyOSW"
#endif

// BLE device name is derived form DEVICE_NAME, unless you want to change this
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME DEVICE_NAME
#endif

#ifndef WIFI_ON_BOOT
#define WIFI_ON_BOOT false
#endif

#ifndef NTP_ALWAYS_ON_WIFI
#define NTP_ALWAYS_ON_WIFI true
#endif

#ifndef WIFI_AUTO_AP
#define WIFI_AUTO_AP true
#endif

// PLEASE CONFIGURE THE VALUES BELOW TO YOUR PREFERENCES
#ifndef CONFIG_WIFI_SSID
#define CONFIG_WIFI_SSID "open-smartwatch"
#endif
#ifndef CONFIG_WIFI_PASS
#define CONFIG_WIFI_PASS "P4ssw0rd!"
#endif
#ifndef CONFIG_FALLBACK_1ST_WIFI_SSID
#define CONFIG_FALLBACK_1ST_WIFI_SSID ""
#endif
#ifndef CONFIG_FALLBACK_1ST_WIFI_PASS
#define CONFIG_FALLBACK_1ST_WIFI_PASS ""
#endif
#ifndef CONFIG_FALLBACK_2ND_WIFI_SSID
#define CONFIG_FALLBACK_2ND_WIFI_SSID ""
#endif
#ifndef CONFIG_FALLBACK_2ND_WIFI_PASS
#define CONFIG_FALLBACK_2ND_WIFI_PASS ""
#endif

#ifndef BLE_ON_BOOT
#define BLE_ON_BOOT false
#endif

#ifndef DISPLAY_BRIGHTNESS
// DISPLAY_MIN_BRIGHTNESS - 255
#define DISPLAY_BRIGHTNESS 128
#endif

#ifndef DISPLAY_MIN_BRIGHTNESS
// avoids turning off the display by accident. better leave as is ;)
#define DISPLAY_MIN_BRIGHTNESS 10
#endif

#ifndef DISPLAY_TIMEOUT
#define DISPLAY_TIMEOUT 10
#endif

#ifndef DISPLAY_OVERLAYS
#define DISPLAY_OVERLAYS true
#endif

#ifndef DISPLAY_OVERLAYS_FORCED
#define DISPLAY_OVERLAYS_FORCED false
#endif

#ifndef WAKE_FROM_RAISE
#define WAKE_FROM_RAISE false
#endif

#ifndef WAKE_FROM_RAISE_SENSITIVITY
#define WAKE_FROM_RAISE_SENSITIVITY 127
#endif

#ifndef WAKE_FROM_TAP
#define WAKE_FROM_TAP true
#endif

#ifndef WAKE_FROM_BTN1
#define WAKE_FROM_BTN1 false
#endif

#ifndef DO_LIGHT_SLEEP
#define DO_LIGHT_SLEEP false
#endif

#ifndef THEME_BACKROUND_COLOR
#define THEME_BACKROUND_COLOR rgb888(0, 0, 0)
#endif
#ifndef THEME_BACKROUND_DIMMED_COLOR
#define THEME_BACKROUND_DIMMED_COLOR rgb888(64, 64, 64)
#endif
#ifndef THEME_FOREGROUND_COLOR
#define THEME_FOREGROUND_COLOR rgb888(255, 255, 255)
#endif
#ifndef THEME_FOREGROUND_DIMMED_COLOR
#define THEME_FOREGROUND_DIMMED_COLOR rgb888(122, 122, 122)
#endif
#ifndef THEME_PRIMARY_COLOR
#define THEME_PRIMARY_COLOR rgb888(0, 209, 178)
#endif
#ifndef THEME_INFO_COLOR
#define THEME_INFO_COLOR rgb888(32, 156, 238)
#endif
#ifndef THEME_SUCCESS_COLOR
#define THEME_SUCCESS_COLOR rgb888(72, 199, 116)
#endif
#ifndef THEME_WARNING_COLOR
#define THEME_WARNING_COLOR rgb888(255, 221, 87)
#endif
#ifndef THEME_DANGER_COLOR
#define THEME_DANGER_COLOR rgb888(255, 56, 96)
#endif

// How long to wait after the first press to check for a double press (in ms)...
#ifndef CONFIG_APPV2_DOUBLE_PRESS_TIME
#define CONFIG_APPV2_DOUBLE_PRESS_TIME 500
#endif

// How long must a button be pressed to be considered a long press (in ms)...
#ifndef CONFIG_APPV2_LONG_PRESS_TIME
#define CONFIG_APPV2_LONG_PRESS_TIME 1000
#endif

// How long must a button be pressed to be considered a very long press (in ms)...
#ifndef CONFIG_APPV2_VERY_LONG_PRESS_TIME
#define CONFIG_APPV2_VERY_LONG_PRESS_TIME 3000
#endif

// The following settings are configurable later on using the web ui, you can still set the defaults here.

#ifndef CONFIG_DATE_FORMAT
// possibilities: "mm/dd/yyyy","dd.mm.yyyy" and "yy.mm/dd"
#define CONFIG_DATE_FORMAT "mm/dd/yyyy"
#endif

// Primary timezone POSIX string, you can translate "Europe/Berlin" (-> "CET-1CEST,M3.5.0,M10.5.0/3") to your timezone here: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#ifndef CONFIG_TIMEZONE_PRIMARY
#define CONFIG_TIMEZONE_PRIMARY ""
#endif

// Secondary timezone POSIX string, see primary timezone for more info
#ifndef CONFIG_TIMEZONE_SECONDARY
#define CONFIG_TIMEZONE_SECONDARY ""
#endif

// Set the initial value for the watchface to the index below
// This can be a small snippet to get the id of the app, or the app id itself
#ifndef CONFIG_DEFAULT_WATCHFACE_ID
#define CONFIG_DEFAULT_WATCHFACE_ID OswAppWatchface::APP_ID
#endif

#ifndef STEPS_PER_DAY
#define STEPS_PER_DAY 10000
#endif

#ifndef DIST_PER_DAY
#define DIST_PER_DAY 10000
#endif

#ifndef KCAL_PER_DAY
#define KCAL_PER_DAY 10000
#endif

#ifndef STEPS_HISTORY_CLEAR
#define STEPS_HISTORY_CLEAR false
#endif

/*
 * Apps:
 */
#ifndef GAME_SNAKE
#define GAME_SNAKE 0
#endif
#ifndef GAME_BRICK_BREAKER
#define GAME_BRICK_BREAKER 0
#endif
#ifndef TOOL_FLASHLIGHT
#define TOOL_FLASHLIGHT 1
#endif
#ifndef TOOL_CALCULATOR
#define TOOL_CALCULATOR 1
#endif
#ifndef TOOL_CLOCK
#define TOOL_CLOCK 1
#endif
#ifndef TOOL_STOPWATCH_BTN_TIMEOUT
#define TOOL_STOPWATCH_BTN_TIMEOUT 1800
#endif
#ifndef TOOL_WATERLEVEL
#define TOOL_WATERLEVEL 1
#endif
#ifndef TOOL_TIMER_BTN_TIMEOUT
#define TOOL_TIMER_BTN_TIMEOUT 1800
#endif

/*
 * Experimentals/Services:
 */

// Experimentals (1 = enable, 0 = disable):
#ifndef SERVICE_BLE_COMPANION
#define SERVICE_BLE_COMPANION 0
#endif

#ifndef OSW_SERVICE_NOTIFIER
#define OSW_SERVICE_NOTIFIER 1
#endif

#ifndef MINI_IOT_DEVICENAME
#define MINI_IOT_DEVICENAME "yourDeviceName"
#endif
#ifndef MINI_IOT_SERVER
#define MINI_IOT_SERVER "yourMiniIotServerIp:port"
#endif
#endif

#ifndef OPENWEATHERMAP_APIKEY
#define OPENWEATHERMAP_APIKEY ""
#endif
#ifndef OPENWEATHERMAP_CITY
#define OPENWEATHERMAP_CITY "turin"
#endif
#ifndef OPENWEATHERMAP_STATE_CODE
#define OPENWEATHERMAP_STATE_CODE "IT"
#endif

// USERTrust RSA Root xSigned using AAA CA
// this certificate is valid until 01/01/2029
// source https://support.sectigo.com/articles/Knowledge/Sectigo-Intermediate-Certificates
// https://comodoca.my.salesforce.com/sfc/p/#1N000002Ljih/a/3l000000VbAR/r5xd1JISSRVHeYpuB5yWee1XcPZeIVFHFx_iWXmT5M8
#ifndef OPENWEATHERMAP_CA_CERT
#define OPENWEATHERMAP_CA_CERT "-----BEGIN CERTIFICATE-----\n"                                      \
                               "MIIFgTCCBGmgAwIBAgIQOXJEOvkit1HX02wQ3TE1lTANBgkqhkiG9w0BAQwFADB7\n" \
                               "MQswCQYDVQQGEwJHQjEbMBkGA1UECAwSR3JlYXRlciBNYW5jaGVzdGVyMRAwDgYD\n" \
                               "VQQHDAdTYWxmb3JkMRowGAYDVQQKDBFDb21vZG8gQ0EgTGltaXRlZDEhMB8GA1UE\n" \
                               "AwwYQUFBIENlcnRpZmljYXRlIFNlcnZpY2VzMB4XDTE5MDMxMjAwMDAwMFoXDTI4\n" \
                               "MTIzMTIzNTk1OVowgYgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpOZXcgSmVyc2V5\n" \
                               "MRQwEgYDVQQHEwtKZXJzZXkgQ2l0eTEeMBwGA1UEChMVVGhlIFVTRVJUUlVTVCBO\n" \
                               "ZXR3b3JrMS4wLAYDVQQDEyVVU0VSVHJ1c3QgUlNBIENlcnRpZmljYXRpb24gQXV0\n" \
                               "aG9yaXR5MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAgBJlFzYOw9sI\n" \
                               "s9CsVw127c0n00ytUINh4qogTQktZAnczomfzD2p7PbPwdzx07HWezcoEStH2jnG\n" \
                               "vDoZtF+mvX2do2NCtnbyqTsrkfjib9DsFiCQCT7i6HTJGLSR1GJk23+jBvGIGGqQ\n" \
                               "Ijy8/hPwhxR79uQfjtTkUcYRZ0YIUcuGFFQ/vDP+fmyc/xadGL1RjjWmp2bIcmfb\n" \
                               "IWax1Jt4A8BQOujM8Ny8nkz+rwWWNR9XWrf/zvk9tyy29lTdyOcSOk2uTIq3XJq0\n" \
                               "tyA9yn8iNK5+O2hmAUTnAU5GU5szYPeUvlM3kHND8zLDU+/bqv50TmnHa4xgk97E\n" \
                               "xwzf4TKuzJM7UXiVZ4vuPVb+DNBpDxsP8yUmazNt925H+nND5X4OpWaxKXwyhGNV\n" \
                               "icQNwZNUMBkTrNN9N6frXTpsNVzbQdcS2qlJC9/YgIoJk2KOtWbPJYjNhLixP6Q5\n" \
                               "D9kCnusSTJV882sFqV4Wg8y4Z+LoE53MW4LTTLPtW//e5XOsIzstAL81VXQJSdhJ\n" \
                               "WBp/kjbmUZIO8yZ9HE0XvMnsQybQv0FfQKlERPSZ51eHnlAfV1SoPv10Yy+xUGUJ\n" \
                               "5lhCLkMaTLTwJUdZ+gQek9QmRkpQgbLevni3/GcV4clXhB4PY9bpYrrWX1Uu6lzG\n" \
                               "KAgEJTm4Diup8kyXHAc/DVL17e8vgg8CAwEAAaOB8jCB7zAfBgNVHSMEGDAWgBSg\n" \
                               "EQojPpbxB+zirynvgqV/0DCktDAdBgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rID\n" \
                               "ZsswDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQFMAMBAf8wEQYDVR0gBAowCDAG\n" \
                               "BgRVHSAAMEMGA1UdHwQ8MDowOKA2oDSGMmh0dHA6Ly9jcmwuY29tb2RvY2EuY29t\n" \
                               "L0FBQUNlcnRpZmljYXRlU2VydmljZXMuY3JsMDQGCCsGAQUFBwEBBCgwJjAkBggr\n" \
                               "BgEFBQcwAYYYaHR0cDovL29jc3AuY29tb2RvY2EuY29tMA0GCSqGSIb3DQEBDAUA\n" \
                               "A4IBAQAYh1HcdCE9nIrgJ7cz0C7M7PDmy14R3iJvm3WOnnL+5Nb+qh+cli3vA0p+\n" \
                               "rvSNb3I8QzvAP+u431yqqcau8vzY7qN7Q/aGNnwU4M309z/+3ri0ivCRlv79Q2R+\n" \
                               "/czSAaF9ffgZGclCKxO/WIu6pKJmBHaIkU4MiRTOok3JMrO66BQavHHxW/BBC5gA\n" \
                               "CiIDEOUMsfnNkjcZ7Tvx5Dq2+UUTJnWvu6rvP3t3O9LEApE9GQDTF1w52z97GA1F\n" \
                               "zZOFli9d31kWTz9RvdVFGD/tSo7oBmF0Ixa1DVBzJ0RHfxBdiSprhTEUxOipakyA\n" \
                               "vGp4z7h/jnZymQyd/teRCBaho1+V\n"                                     \
                               "-----END CERTIFICATE-----\n"
#endif