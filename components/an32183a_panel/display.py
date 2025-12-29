import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, i2c
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
    CONF_CONTRAST,
)

DEPENDENCIES = ['i2c']

CONF_RESET_PIN = 'nrst_pin'


an32183_ns = cg.esphome_ns.namespace("an32183")
Panel = an32183_ns.class_(
    "Panel", display.DisplayBuffer, i2c.I2CDevice
)


CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Panel),
            cv.Optional(CONF_RESET_PIN, default=12): cv.int_,
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(i2c.i2c_device_schema(0x5C)),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    #await cg.register_component(var, config)
    await display.register_display(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_pins(
        config[CONF_RESET_PIN],
    ))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
