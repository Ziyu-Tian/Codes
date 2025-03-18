/*
GPIO directly connection via PIO
PPR captured with 2x frequency, measured as 1200 correctly
*** Updates 14-03-25
- RPM Test: Manual Test with 70-80 rpm, correct (400 ms sample)
- Support three encoders

*** Updates 17-03-25
- RPM Sample frequency changed to 1000 ms
- Reduce PPR to 600 

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
    uint8_t A2_pin = 21;
    uint8_t A3_pin = 19;
    float ppr = 600.0; // 

    // Initialize PIO_Encoder Class
    QuadratureEncoder encoder_1(pio0,A_pin, ppr);
    QuadratureEncoder encoder_2(pio0,A2_pin, ppr);
    QuadratureEncoder encoder_3(pio1,A3_pin, ppr);
    // Sampling Time for Speed Calculation
    float sampling_time = 1000e-3;

    // Digital Pot settings
    int digital_pot_output = 255; // 0 - 255 (-100% to +100%)
    // 241 - 4.695 V
    // 242 - 4.7152 V

    while (true) {
        
        encoder_1.update(sampling_time); // Initialize starting time
        encoder_2.update(sampling_time); // Initialize starting time 
        encoder_3.update(sampling_time); // Initialize starting time 
        // Position: return angle (default radium)
        // Velocity: return velocity in rad/s
        // Counter: return counting number (CW++, ACW--, 1200 per round)
        auto position = encoder_1.get_position();
        auto velocity = encoder_1.get_velocity();
        auto counter = encoder_1.get_count();

        auto position_2 = encoder_2.get_position();
        auto velocity_2 = encoder_2.get_velocity();
        auto counter_2 = encoder_2.get_count();

        auto position_3 = encoder_3.get_position();
        auto velocity_3 = encoder_3.get_velocity();
        auto counter_3  = encoder_3.get_count();
        
        float rpm_1 = (30 * velocity) / M_PI;
        float rpm_2 = (30 * velocity_2) / M_PI;
        float rpm_3 = (30 * velocity_3) / M_PI;

        write_pot(0x11, digital_pot_output); 

        // printf("Velocity_1: %.2f RPM , Counter_1: %d ;", rpm_1, counter);
        // printf("Velocity_2: %.2f RPM , Counter_2: %d\n", rpm_2, counter_2);
        printf("RPM_1: %.2f, ",rpm_1);
        printf("RPM_2: %.2f, ",rpm_2);
        printf("RPM_3: %.2f\n",rpm_3);
        sleep_ms(500);
    }

    return 0;

}