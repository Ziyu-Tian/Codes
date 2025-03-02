#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>

// SPI PIN Config
#define SPI_PORT spi0   // spi0 applied
#define PIN_SCK  2      // SCK = GPIO2
#define PIN_MOSI 3      // MOSI = GPIO3
#define PIN_MISO 4      // MISO
#define PIN_CS   8      // CS

void init_spi() {
    spi_init(SPI_PORT, 1000 * 1000); // 1Mhz SPI 
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1); // Unselect chip
}

// Sending SPI cmd + data 
void write_pot(uint8_t cmd, uint8_t value) {
    uint8_t data[2] = {cmd, value};

    gpio_put(PIN_CS, 0); // Select chip, start sending data
    spi_write_blocking(SPI_PORT, data, 2);
    gpio_put(PIN_CS, 1); // Unselect chip, end data sending
}

int main() {
    stdio_init_all();
    init_spi();
    float pid_output = 0; // PID (0 - 5 V, STOP - FULL SPEED)
    float digital_pot_output = (((pid_output / 5) * 4.7) /5.0) * 255; // Map 0-5 V to 0-4.7 V
    // then change voltage to values in 
    int output_voltage = (int) digital_pot_output;

    while (1) {
        printf("PID Output: %.2f / 5 V\n",pid_output);
        printf("Digital_Pot Output: %.2f V\n",(pid_output / 5) * 4.7);
        write_pot(0x11, output_voltage); 
        sleep_ms(1000);
    }
}
