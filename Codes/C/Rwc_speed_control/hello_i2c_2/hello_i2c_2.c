/*

Polling encoder values from Adafruit I2C breakout, cannot capture the pulses precisely 

Testing Result: 20 - 100 single-phase PPR depend on spining speed

*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define ENCODER_ADDR 0x1
#define SEESAW_ENCODER_BASE 0x11
#define SEESAW_ENCODER_POSITION 0x30
#define SEESAW_ENCODER_INTENSET 0x10
#define SEESAW_ENCODER_INTFLAG 0x44

int32_t read_encoder_position(i2c_inst_t *i2c)
{
    uint8_t cmd[2] = {SEESAW_ENCODER_BASE, SEESAW_ENCODER_POSITION};
    uint8_t buffer[4];

    // Write Reg Address
    int result = i2c_write_blocking(i2c, ENCODER_ADDR, cmd, 2, true);
    if (result < 0)
    {
        printf("I2C write failed!\n");
        return -1;
    }

    // Read 4 Bytes
    result = i2c_read_blocking(i2c, ENCODER_ADDR, buffer, 4, false);
    if (result < 0)
    {
        printf("I2C read failed!\n");
        return -1;
    }

    int32_t position = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) |
                       ((uint32_t)buffer[2] << 8) | (uint32_t)buffer[3];

    return position;
}

// Encoder Interrupt
void enable_encoder_interrupt(i2c_inst_t *i2c)
{
    uint8_t cmd[2] = {SEESAW_ENCODER_BASE, SEESAW_ENCODER_INTENSET};
    uint8_t enable_interrupt = 0x01; 
    i2c_write_blocking(i2c, ENCODER_ADDR, cmd, 2, true);
    i2c_write_blocking(i2c, ENCODER_ADDR, &enable_interrupt, 1, false);
}

int main()
{
    // I2C Pin for CANBed Pico
    const uint sda_pin = 6;
    const uint scl_pin = 7;
    i2c_inst_t *i2c = i2c1;

    // Serial Port Init
    stdio_init_all();
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }
    sleep_ms(100); // 
    printf("I2C Scanner Starting...\n");

    // Init I2C
    i2c_init(i2c, 400 * 1000);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

  

    printf("Starting I2C Encoder Read...\n");

    while (true)
    {
            int32_t encoder_position = read_encoder_position(i2c);
            enable_encoder_interrupt(i2c);
            if (encoder_position != -1) {
                printf("Encoder Position: %ld\n", encoder_position);
            }
            sleep_ms(100);
        // }
    }
}

