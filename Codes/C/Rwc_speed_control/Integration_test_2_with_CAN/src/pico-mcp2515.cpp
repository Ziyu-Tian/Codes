#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "mcp2515/mcp2515.h"
#include "hardware/spi.h"
#include <QuadratureEncoder.hpp>

// CAN Config
MCP2515 can0;
struct can_frame rx;
struct can_frame tx_frame;

// SPI PIN Config
#define SPI_PORT spi0 // spi0 applied
#define PIN_SCK 2     // SCK = GPIO2
#define PIN_MOSI 3    // MOSI = GPIO3
#define PIN_MISO 4    // MISO
#define PIN_CS 8      // CS


// SPI Init Function
void init_spi()
{
    spi_init(SPI_PORT, 1000 * 1000); // 1Mhz SPI
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1); // Unselect chip
}

// Sending SPI cmd + data
void write_pot(uint8_t cmd, uint8_t value)
{
    uint8_t data[2] = {cmd, value};

    gpio_put(PIN_CS, 0); // Select chip, start sending data
    spi_write_blocking(SPI_PORT, data, 2);
    gpio_put(PIN_CS, 1); // Unselect chip, end data sending
}

// Digital Pot Mapping
int8_t map_can_to_pot(int8_t can_value)
{                                 
    return (int8_t)(255.0 / 15.0); // 映射到 0-255
}

int main()
{
    stdio_init_all();
    init_spi();

    // Pin_A (Pin_B would be set as A_pin + 1)
    uint8_t A_pin = 23; // Noted that A and B should be swapped in physical connection
    float ppr = 600.0;  //

    // Initialize PIO_Encoder Class
    QuadratureEncoder encoder(A_pin, ppr);

    // Sampling Time for Speed Calculation
    float sampling_time = 100e-3;

    // Digital Pot settings (Testing)
    // int digital_pot_output = 127; // 0 - 255 (-100% to +100%)

    // Initialize interface
    can0.reset();
    can0.setBitrate(CAN_100KBPS, MCP_16MHZ);
    can0.setNormalMode();

    // Config Mode
    can0.setConfigMode();

    // Mask0 & Mask1 All 11-bit need to be same
    can0.setFilterMask(MCP2515::MASK0, false, 0x7FF);
    can0.setFilterMask(MCP2515::MASK1, false, 0x7FF);

    // Filter（Only ID 0x123）
    can0.setFilter(MCP2515::RXF0, false, 0x123);
    can0.setFilter(MCP2515::RXF1, false, 0x123);
    can0.setFilter(MCP2515::RXF2, false, 0x123);
    can0.setFilter(MCP2515::RXF3, false, 0x123);
    can0.setFilter(MCP2515::RXF4, false, 0x123);
    can0.setFilter(MCP2515::RXF5, false, 0x123);

    // Change to normal mode
    can0.setNormalMode();

    // struct repeating_timer timer;
    // add_repeating_timer_ms(5000, send_can_data, NULL, &timer);

    // Listen loop
    while (true)
    {
        if (can0.readMessage(&rx) == MCP2515::ERROR_OK)
        {
            int8_t received_value = (int8_t)rx.data[0];

            int8_t digital_pot_output = map_can_to_pot(received_value);

           

            printf("New frame from ID: %10x\n", rx.can_id);
            int setting_value = (int)digital_pot_output;
            printf("Digital Pot: %10x\n", setting_value);
            write_pot(0x11, setting_value);

            // Print the received data (if any)
            printf("Data: ");
            for (int i = 0; i < rx.can_dlc; i++)
            {
                printf("%02X ", rx.data[i]);
            }
            printf("\n");
        }
        
        encoder.update(sampling_time); // Initialize starting time

        // Position: return angle (default radium)
        // Velocity: return velocity in rad/s
        // Counter: return counting number (CW++, ACW--, 1200 per round)
        auto position = encoder.get_position();
        auto velocity = encoder.get_velocity();
        auto counter = encoder.get_count();

        float rpm = (15 * velocity) / M_PI;

        //printf("Velocity: %.2f RPM \n, Counter: %d\n", velocity, counter);

        sleep_ms(1000);
    }

    return 0;
}