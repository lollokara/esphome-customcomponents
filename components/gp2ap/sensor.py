import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_TYPE,
    STATE_CLASS_MEASUREMENT,
    ICON_LIGHTBULB,
)
from . import GP2AP, CONF_GP2AP_ID

DEPENDENCIES = ["gp2ap"]

TYPES = ["clear", "ir", "proximity"]

CONFIG_SCHEMA = sensor.sensor_schema(
    icon=ICON_LIGHTBULB,
    accuracy_decimals=1,
    state_class=STATE_CLASS_MEASUREMENT,
).extend(
    {
        cv.Required(CONF_TYPE): cv.one_of(*TYPES, lower=True),
        cv.GenerateID(CONF_GP2AP_ID): cv.use_id(GP2AP),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_GP2AP_ID])
    var = await sensor.new_sensor(config)
    func = getattr(hub, f"set_{config[CONF_TYPE]}_sensor")
    cg.add(func(var))
