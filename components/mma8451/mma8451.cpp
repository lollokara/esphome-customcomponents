#include "mma8451.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mma8451 {

static const char *const TAG = "mma8451";

const uint8_t MMA8451_REGISTER_WHO_AM_I = 0x0D;
const uint8_t MMA8451_REG_OUT_X_MSB = 0x01; //!< Read-only device output register
const uint8_t MMA8451_REG_WHOAMI = 0x0D;    //!< WHO_AM_I device ID register
const uint8_t MMA8451_REG_XYZ_DATA_CFG = 0x0E; //!< XYZ_DATA_CFG register, sets dynamic range and high-pass filter for
       //!< output data
const uint8_t MMA8451_REG_PL_STATUS = 0x10; //!< PL_STATUS portrait/landscape status register
const uint8_t MMA8451_REG_PL_CFG = 0x11; //!< Portrait/landscape configuration register
const uint8_t MMA8451_REG_CTRL_REG1 = 0x2A; //!< CTRL_REG1 system control 1 register
const uint8_t MMA8451_REG_CTRL_REG2 = 0x2B; //!< CTRL_REG2 system control 2 register
const uint8_t MMA8451_REG_CTRL_REG4 = 0x2D; //!< CTRL_REG4 system control 4 register
const uint8_t MMA8451_REG_CTRL_REG5 = 0x2E; //!< CTRL_REG5 system control 5 register

///@{
void MMA8451Component::setup() {
  ESP_LOGV(TAG, "Setting up MMA8451...");
  uint8_t who_am_i;
  if (!this->read_byte(MMA8451_REGISTER_WHO_AM_I, &who_am_i) ||
      (who_am_i != 0x2A)) {
    ESP_LOGV(TAG, "Unable to Find MMA8451");
    //this->mark_failed();
    //return;
  }
  write_byte(MMA8451_REG_CTRL_REG2, 0x40); //reset device
  who_am_i = 0;
  ESP_LOGV(TAG, "  Resetting MMA8551...");
  // Setup power management
  while (read_byte(MMA8451_REG_CTRL_REG2, &who_am_i) && who_am_i == 0x40);
    // enable 4G range
  if (!this->write_byte(MMA8451_REG_XYZ_DATA_CFG, MMA8451_RANGE_2_G)) {
    //this->mark_failed();
    //return;
  }
    // High res
  if (!this->write_byte(MMA8451_REG_CTRL_REG2, 0x02)) {
    //this->mark_failed();
    //return;
  }
    // DRDY on INT1
  if (!this->write_byte(MMA8451_REG_CTRL_REG4, 0x01)) {
    //this->mark_failed();
    //return;
  }
  if (!this->write_byte(MMA8451_REG_CTRL_REG5, 0x01)) {
    //this->mark_failed();
    //return;
  }
    // Turn on orientation config
  if (!this->write_byte(MMA8451_REG_PL_CFG, 0x40)) {
    //this->mark_failed();
    //return;
  }
  // Activate at max rate, low noise mode
  if (!this->write_byte(MMA8451_REG_CTRL_REG1, 0x05)) {
    //this->mark_failed();
    //return;
  }
  

}
void MMA8451Component::dump_config() {
  ESP_LOGCONFIG(TAG, "MMA8451:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with MMA8451 failed!");
  }
  uint8_t who_am_i;
  if (!this->read_byte(MMA8451_REGISTER_WHO_AM_I, &who_am_i) ||
      (who_am_i != 0x2A)) {
    ESP_LOGE(TAG, "Unable to Find MMA8451");
    ESP_LOGD(TAG, "Got WHOAMI= %d", who_am_i);
  }
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Acceleration X", this->accel_x_sensor_);
  LOG_SENSOR("  ", "Acceleration Y", this->accel_y_sensor_);
  LOG_SENSOR("  ", "Acceleration Z", this->accel_z_sensor_);
}

void MMA8451Component::update() {
  ESP_LOGV(TAG, "    Updating MMA8451...");
  uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  write_byte(MMA8451_REG_OUT_X_MSB, 0xFF);
  if (!this->read_bytes(MMA8451_REG_OUT_X_MSB, buffer, 7)) {
    this->status_set_warning();
    return;
  }
  int16_t x, y, z;
  uint16_t divider = 2048;
  x = buffer[1];
  x <<= 8;
  x |= buffer[2];
  x >>= 4;
  y = buffer[3];
  y <<= 8;
  y |= buffer[4];
  y >>= 4;
  z = buffer[5];
  z <<= 8;
  z |= buffer[6];
  z >>= 4;

  float accel_x = (float)x / divider;
  float accel_y = (float)y / divider;
  float accel_z = (float)z / divider;
/*
  ESP_LOGD(TAG,
           "Got buff={1=%d,2=%d,3=%d,4=%d,5=%d,6=%d,7=%d,8=%d}",
           buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
  ESP_LOGD(TAG,
           "Got accel={x=%.3f m/s², y=%.3f m/s², z=%.3f m/s²}, ",
           accel_x, accel_y, accel_z);
*/
  if (this->accel_x_sensor_ != nullptr)
    this->accel_x_sensor_->publish_state(accel_x);
  if (this->accel_y_sensor_ != nullptr)
    this->accel_y_sensor_->publish_state(accel_y);
  if (this->accel_z_sensor_ != nullptr)
    this->accel_z_sensor_->publish_state(accel_z);

  this->status_clear_warning();
}
float MMA8451Component::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace MMA8451
}  // namespace esphome
