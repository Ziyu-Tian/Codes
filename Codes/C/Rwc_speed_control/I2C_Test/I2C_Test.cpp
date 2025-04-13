#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c1
#define I2C_SDA 6
#define I2C_SCL 7

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5



int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);

    //printf("Hello, world!\n");
    
    // gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA);
    // gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // // Set up our UART
    // uart_init(UART_ID, BAUD_RATE);
    // // Set the TX and RX pins by using the function select on the GPIO
    // // Set datasheet for more information on function select
    // gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    // gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // // Use some the various UART functions to send out data
    // // In a default system, printf will also output via the default UART
    

    // int nack_count = 0;

    // for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
    //     uint8_t dummy = 0;
    //     int result = i2c_write_timeout_us(I2C_PORT, addr, &dummy, 1, false, 1000);
    //     if (result < 0) {
    //         nack_count++;
    //     } else {
    //         printf("✅ Unexpected device found at 0x%02X\n", addr);
    //     }
    // }

    // if (nack_count == (0x77 - 0x08 + 1)) {
    //     printf("✅ I2C port works! No devices found (as expected).\n");
    // } else {
    //     printf("⚠️ Warning: Detected devices, but none expected.\n");
    // }
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart

    while (true) {
        //printf("Hello, world!\n");
        sleep_ms(100);
    }
}
