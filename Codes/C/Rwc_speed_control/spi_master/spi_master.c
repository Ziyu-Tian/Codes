/*

SPI_MASTER Connection Test - 04/13/25

- Testing with 3V3 as VCC of SPI Digital Pot
- Sampling with internal ADC in A0

*/


#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include <stdio.h>

#define SPI_PORT spi0
#define PIN_SCK 2
#define PIN_MOSI 3
#define PIN_MISO 4
#define PIN_CS 8
#define ADC_PIN 26 // A0 in CANBed 2040 

void init_spi()
{
    spi_init(SPI_PORT, 1000 * 1000);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
}

void write_pot(uint8_t cmd, uint8_t value)
{
    uint8_t data[2] = {cmd, value};
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, data, 2);
    gpio_put(PIN_CS, 1);
    sleep_ms(10);
}

void init_adc()
{
    adc_init();
    adc_gpio_init(ADC_PIN);
}

float read_voltage()
{
    adc_select_input(0);       // GPIO26
    uint16_t raw = adc_read(); // 0-4095
    return raw * 3.3f / 4095.0f;
}

int main()
{
    stdio_init_all();
    init_spi();
    init_adc();

    while (1)
    {
        for (int dn = 0; dn <= 255; dn += 5)
        {
            write_pot(0x12, dn); // Wiper 
            sleep_ms(50);
            float v = read_voltage();
            printf("Dn: %d, Voltage: %.3f V\n", dn, v);
            sleep_ms(500);
        }
    }
}
