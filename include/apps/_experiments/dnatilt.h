#ifndef OSW_APP_DNA_TILT_H
#define OSW_APP_DNA_TILT_H

#include <osw_hal.h>

#include <OswAppV1.h>

class OswAppDNATilt : public OswApp {
  public:
    OswAppDNATilt(void) {};
    virtual void setup() override;
    virtual void loop() override;
    ~OswAppDNATilt() {};

  private:
};

#endif
