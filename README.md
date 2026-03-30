# esphome-customcomponents
personal repo for custom esphome components :)
Currently supporting:
- Frekvens panels with multipanel support for stacking one or more IKEA Frekvens (compatible with esphome 2024.4.2)
- gp2ap Gesture sensor (present on Nuimo)
- an32183a Panasonic LED matrix manager (present on Nuimo)
- mma8451 NXP 3Axis mems accelerometer (present on Nuimo)
- ld2410 Radar (obsolete i shall say but i'll keep this version since if aint broken don't fix it.

# Example Frekvens config entry:
```yaml
  display:
  - platform: frekvens_panel
    id: IKEABOX
    latch_pin: 12
    clock_pin: 04
    data_pin: 05
    panel_num: 3
    rotation: 0
    update_interval: 0.025s
    lambda: |- //insert code here
  light:
  - platform: monochromatic
    name: 'Brightness'
    output: matrix_brightness
    restore_mode: RESTORE_DEFAULT_ON
    id: light_1
  output:
  - platform: ledc
    id: matrix_brightness
    pin:
      number: GPIO14
      inverted: True
```

# Example gp2ap and MMA8451 config entry:
```yaml
sensor:
  - platform: mma8451
    address: 0x1C
    accel_x:
      name: "MMA8451 Accel X"
      internal: true
      id: x_acc
    accel_y:
      name: "MMA8451 Accel Y"
      internal: true
      id: y_acc
    accel_z:
      name: "MMA8451 Accel Z"
      internal: true

  - platform: gp2ap
    type: CLEAR
    name: "GP2AP Clear Channel"
    id: light_sens
  - platform: gp2ap
    type: IR
    name: "GP2AP Ir Channel"
  - platform: gp2ap
    type: PROXIMITY
    name: "GP2AP Proximity Channel"
gp2ap:
  address: 0x39
  update_interval: 2s

i2c:
  sda: GPIO21
  scl: GPIO1
  scan: true
  id: bus_a
  frequency: 200khz # could couse problems going faster for GP2AP
```
# Example an32183a config entry:
```yaml
display:
  - platform: an32183a_panel
    id: led_disp
    nrst_pin: 4
    update_interval: 0.05s
    rotation: 0
    lambda: |- //insert code here
```  
