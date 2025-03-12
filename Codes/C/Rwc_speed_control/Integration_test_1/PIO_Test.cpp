/*
GPIO directly connection via PIO
PPR captured with 2x frequency, measured as 1200 correctly
*/
#include <QuadratureEncoder.hpp>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>

// SPI PIN Config
#define SPI_PORT spi0   // spi0 applied
#define PIN_SCK  2      // SCK = GPIO2
#define PIN_MOSI 3      // MOSI = GPIO3
#define PIN_MISO 4      // MISO
#define PIN_CS   8      // CS

// SPI Init Function 
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

    // Pin_A (Pin_B would be set as A_pin + 1)
    uint8_t A_pin = 23; // Noted that A and B should be swapped in physical connection
    float ppr = 600.0; // 

    // Initialize PIO_Encoder Class
    QuadratureEncoder encoder(A_pin, ppr);

    // Sampling Time for Speed Calculation
    float sampling_time = 100e-3;

    // Digital Pot settings
    int digital_pot_output = 10; // 0 - 255 (-100% to +100%)

    while (true) {
        
        encoder.update(sampling_time); // Initialize starting time
            
        // Position: return angle (default radium)
        // Velocity: return velocity in rad/s
        // Counter: return counting number (CW++, ACW--, 1200 per round)
        auto position = encoder.get_position();
        auto velocity = encoder.get_velocity();
        auto counter = encoder.get_count();
        
        float rpm = (15 * velocity) / M_PI;

        write_pot(0x11, digital_pot_output); 

        printf("Velocity: %.2f RPM \n, Counter: %d\n", velocity, counter);

        sleep_ms(500);
    }

    return 0;
}

