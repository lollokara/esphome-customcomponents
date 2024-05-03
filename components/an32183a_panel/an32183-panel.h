#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

#include "an32183a-driver.h"


namespace esphome {
namespace an32183 {

class Panel : public display::DisplayBuffer {
 public:
  int p_nrst;

  void set_pins(int p_nrst) {
    this->p_nrst = p_nrst;
  }

  void setBrightness(int brightness);

  float get_setup_priority() const override { return setup_priority::PROCESSOR; }

  void data(uint8_t value);

  void initialize();
  void dump_config() override;
  void HOT display();

  void update() override;

  void fill(Color color) override;

  void setup() override {
    this->initialize();
  }
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_BINARY; }
  
 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;

  void init_reset_();

  size_t get_buffer_length_();

  void start_command_();
  void end_command_();
  void start_data_();
  void end_data_();

  int get_width_internal() override;
  int get_height_internal() override;

  AN32183A *panel;
};

}  // namespace pcd8544
}  // namespace esphome
