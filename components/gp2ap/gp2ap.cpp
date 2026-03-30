#include "gp2ap.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
namespace esphome {
namespace gp2ap {

static const char *const TAG = "gp2ap";

#define GP2AP_ERROR_CHECK(func) \
  if (!(func)) { \
    this->mark_failed(); \
    return; \
  }
#define GP2AP_WRITE_BYTE(reg, value) GP2AP_ERROR_CHECK(this->write_byte(reg, value));

void GP2AP::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GP2AP...");
  uint8_t id;
  if (!this->read_byte(0x3E, &id)) {  // ID register
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }

  if (id != 0x60) {  // GP2AP should have this ID
    this->error_code_ = WRONG_ID;
    this->mark_failed();
    return;
  }
  GP2AP_WRITE_BYTE(0x02, 0x11);
  // Active PS and ALS mode
  GP2AP_WRITE_BYTE(0x00, 0x80);
  // Set PS Interrupt with level INT
  //GP2AP_WRITE_BYTE(0x02, 0x10);
  GP2AP_WRITE_BYTE(0x02, 0x1E);
  // Set 16bit integration + range x256 
  GP2AP_WRITE_BYTE(0x03, 0x48);
  GP2AP_WRITE_BYTE(0x04, 0x01);
  // PRST 4cycle 12bit x2 range
  GP2AP_WRITE_BYTE(0x05, 0x69);
  // IS=150mA LED Pulse x16
  GP2AP_WRITE_BYTE(0x06, 0xCD);
  // IS=150mA LED Pulse x16
  GP2AP_WRITE_BYTE(0x07, 0xFB);
  // Set Threshold LOW
  GP2AP_WRITE_BYTE(0x08, 0xE8);
  GP2AP_WRITE_BYTE(0x09, 0x03);
  // Set Threshold HIGH
  GP2AP_WRITE_BYTE(0x0A, 0xDC);
  GP2AP_WRITE_BYTE(0x0B, 0x05);
  //GS Offsets
  GP2AP_WRITE_BYTE(0x0C, 0x00);
  GP2AP_WRITE_BYTE(0x0D, 0x00);
  GP2AP_WRITE_BYTE(0x0E, 0x00);
  GP2AP_WRITE_BYTE(0x0F, 0x00);
  GP2AP_WRITE_BYTE(0x10, 0x00);
  GP2AP_WRITE_BYTE(0x11, 0x00);
  GP2AP_WRITE_BYTE(0x12, 0x00);
  GP2AP_WRITE_BYTE(0x13, 0x00);
  //panel
  GP2AP_WRITE_BYTE(0x41, 0x00);

}


void GP2AP::dump_config() {
  ESP_LOGCONFIG(TAG, "GP2AP:");
  LOG_I2C_DEVICE(this);

  LOG_UPDATE_INTERVAL(this);

#ifdef USE_SENSOR
  LOG_SENSOR("  ", "IR channel", this->ir_sensor_);
  LOG_SENSOR("  ", "Clear channel", this->clear_sensor_);
  LOG_SENSOR("  ", "Proximity", this->proximity_sensor_);
#endif

  if (this->is_failed()) {
    switch (this->error_code_) {
      case COMMUNICATION_FAILED:
        ESP_LOGE(TAG, "Communication with GP2AP failed!");
        break;
      case WRONG_ID:
        ESP_LOGE(TAG, "GP2AP has invalid id!");
        break;
      default:
        ESP_LOGE(TAG, "Setting up GP2AP registers failed!");
        break;
    }
  }
}

#define GP2AP_WARNING_CHECK(func, warning) \
  if (!(func)) { \
    ESP_LOGW(TAG, warning); \
    this->status_set_warning(); \
    return; \
  }

void GP2AP::update() {

  GP2AP_WRITE_BYTE(0x00, 0x80);
  // Set PS Interrupt with level INT
  //GP2AP_WRITE_BYTE(0x02, 0x10);
  GP2AP_WRITE_BYTE(0x02, 0x1E);
  // Set 16bit integration + range x256 
  GP2AP_WRITE_BYTE(0x03, 0x48);
  GP2AP_WRITE_BYTE(0x04, 0x01);
  // PRST 4cycle 12bit x2 range
  GP2AP_WRITE_BYTE(0x05, 0x69);
  // IS=150mA LED Pulse x16
  GP2AP_WRITE_BYTE(0x06, 0xCD);
  // IS=150mA LED Pulse x16
  GP2AP_WRITE_BYTE(0x07, 0xFB);
  // Set Threshold LOW
  GP2AP_WRITE_BYTE(0x08, 0xE8);
  GP2AP_WRITE_BYTE(0x09, 0x03);
  // Set Threshold HIGH
  GP2AP_WRITE_BYTE(0x0A, 0xDC);
  GP2AP_WRITE_BYTE(0x0B, 0x05);
  //GS Offsets
  GP2AP_WRITE_BYTE(0x0C, 0x00);
  GP2AP_WRITE_BYTE(0x0D, 0x00);
  GP2AP_WRITE_BYTE(0x0E, 0x00);
  GP2AP_WRITE_BYTE(0x0F, 0x00);
  GP2AP_WRITE_BYTE(0x10, 0x00);
  GP2AP_WRITE_BYTE(0x11, 0x00);
  GP2AP_WRITE_BYTE(0x12, 0x00);
  GP2AP_WRITE_BYTE(0x13, 0x00);
  //panel
  GP2AP_WRITE_BYTE(0x41, 0x00);
  uint8_t prox_raw[2];
  GP2AP_WARNING_CHECK(this->read_bytes(0x34, prox_raw, 2), "Reading prox reg failed.");
  uint8_t clear_raw[2];
  GP2AP_WARNING_CHECK(this->read_bytes(0x36, clear_raw, 2), "Reading prox reg failed.");
  uint8_t ir_raw[2];
  GP2AP_WARNING_CHECK(this->read_bytes(0x38, ir_raw, 2), "Reading prox reg failed.");
  int16_t prox_dat, ir_dat, clear_dat;
  prox_dat = prox_raw[1];
  prox_dat <<= 8;
  prox_dat |= prox_raw[0];

  clear_dat = clear_raw[1];
  clear_dat <<= 8;
  clear_dat |= clear_raw[0];

  ir_dat = ir_raw[1];
  ir_dat <<= 8;
  ir_dat |= ir_raw[0];
  this->status_clear_warning();
  ESP_LOGD(TAG, "Got Clear=%d IR=%d PS=%d", clear_dat, ir_dat, prox_dat);
#ifdef USE_SENSOR
  if (this->clear_sensor_ != nullptr)
    this->clear_sensor_->publish_state(clear_dat);
  if (this->ir_sensor_ != nullptr)
    this->ir_sensor_->publish_state(ir_dat);
  if (this->proximity_sensor_ != nullptr)
    this->proximity_sensor_->publish_state(prox_dat);
#endif

}

//void GP2AP::loop() { this->read_gesture_data_(); }

float GP2AP::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace gp2ap
}  // namespace esphome
