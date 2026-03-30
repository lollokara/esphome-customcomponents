#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

namespace esphome {
namespace gp2ap {

class GP2AP : public PollingComponent, public i2c::I2CDevice {
#ifdef USE_SENSOR
  SUB_SENSOR(ir)
  SUB_SENSOR(clear)
  SUB_SENSOR(proximity)
#endif

 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

 protected:
  uint8_t led_drive_;

  enum ErrorCode {
    NONE = 0,
    COMMUNICATION_FAILED,
    WRONG_ID,
  } error_code_{NONE};
};

}  // namespace gp2ap
}  // namespace esphome
