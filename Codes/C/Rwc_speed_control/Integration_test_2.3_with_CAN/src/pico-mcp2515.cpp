/*

**Updates for Integration 2.1:
- Integrate RPM Serial output with CAN Sending Message, provides the
possibility to compare latency between serial output and CAN Master.
- Multi-core implementation: Core-0 read encoder ; Core-1 process CAN TX/RX
- It is estimated the RPM is correct and almost same in CAN side and Serial 
output. 

**Updates for Integration Test 2.2:
- Implement receiving CAN data (standard data frame) from PC to Pico.
- The target speed could be 4 bytes float number (IEEE 754 standard, range 0 - 15)
- The digital pot could covert the speed into a value from 0 - 240 (0 - 4.7 V)

**Updates for Integration Test 2.3:
- Multi-encoder supported
- Average float number being TX via CAN

*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "mcp2515/mcp2515.h"
#include "hardware/spi.h"
#include "pico/multicore.h"
#include <cstring>
#include <QuadratureEncoder.hpp>

// CAN Config
MCP2515 can0;
struct can_frame rx;
struct can_frame tx_frame;
volatile uint8_t received_value = 0; // received CAN data

// SPI PIN Config
#define SPI_PORT spi0 // spi0 applied
#define PIN_SCK 2     // SCK = GPIO2
#define PIN_MOSI 3    // MOSI = GPIO3
#define PIN_MISO 4    // MISO
#define PIN_CS 8      // CS

// Pin_A (Pin_B would be set as A_pin + 1)
#define A_pin 23  // Noted that A and B should be swapped in physical connection //14
#define A2_pin 21 //12
#define A3_pin 19 //
#define ppr 600.0 // PPR
// Sampling Time for Speed Calculation
#define sampling_time 1000e-3

int latest_rpm = 0;     //float latest_rpm = 0.0;         // Global latest_rpm 
bool new_rpm_available = false; // new_rpm_calculated_flag
volatile bool can_send_flag = false; // can_send_flag

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

// Sending SPI cmd + data to Digital Pot-0
void write_pot(uint8_t cmd, uint8_t value)
{
    uint8_t data[2] = {cmd, value};

    gpio_put(PIN_CS, 0); // Select chip, start sending data
    spi_write_blocking(SPI_PORT, data, 2);
    gpio_put(PIN_CS, 1); // Unselect chip, end data sending
}

// Digital Pot Mapping - Int
uint8_t map_can_to_pot(uint8_t can_value)
{
    return can_value * (240.0 / 15.0); // 0 ~ 15 --> 0 ~ 240
}

// Digital Pot Mapping - Float
uint8_t map_float_to_pot(int value) // float
{

    if (value < -15.0)
        value = -15.0;
    if (value > 15.0)
        value = 15.0;

    // Linear Mapping: (0~ 15) → (0 ~ 240)
    return (uint8_t)(value * (240.0 / 15.0));
}

void send_can_data() {
    struct can_frame tx_frame;
    tx_frame.can_id = 0x126; // CAN ID
    tx_frame.can_dlc = 4;    // IEEE 754 Float

    // RPM to float
    memcpy(tx_frame.data, &latest_rpm, sizeof(uint8_t));

    // Send Message
    if (can0.sendMessage(&tx_frame) == MCP2515::ERROR_OK) {
        // printf("\e[1;1H\e[2J");
        // printf("CAN Successful! ID=0x%X\n", tx_frame.can_id);
        // printf("Core 1 - RPM: %d\n", latest_rpm);
    } else {
       // printf("CAN Failed!\n");
    }
}

bool timerCallback(struct repeating_timer *t) {
    can_send_flag = true;  // Set send_flag
    return true;           // continue timer
}

void core0_entry()
{
    // Initialize PIO_Encoder Class
    QuadratureEncoder encoder_1(pio0, A_pin,  ppr);
    QuadratureEncoder encoder_2(pio0, A2_pin, ppr);
    QuadratureEncoder encoder_3(pio1, A3_pin, ppr);

    while (true)
    {
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

        int rpm = (30 * velocity) / M_PI;
        int rpm_2 = (30 * velocity_2) / M_PI;
        int rpm_3 = (30 * velocity_3) / M_PI;

        rpm = (rpm + rpm_2 + rpm_3);

        uint32_t rpm_data;
        memcpy(&rpm_data, &rpm, sizeof(int)); // change to 32 bits
        //printf("\e[1;1H\e[2J");
        printf("Core 1 - RPM: %d\n", rpm_data);
        multicore_fifo_push_blocking(rpm_data); // TX RPM data

        //sleep_ms(1000); // Send frequency 
    }
}

void core1_entry()
{
    while (true)
    {
        if (can0.readMessage(&rx) == MCP2515::ERROR_OK)
        {
            // CAN Data (0-15)
            // printf("Raw CAN Data: ");
            // for (int i = 0; i < rx.can_dlc; i++)
            // {
            //     printf("%02X ", rx.data[i]); // Print each bytes
            // }
            // printf("\n");

            // int received_float;
            // memcpy(&received_float, &rx.data[4], sizeof(uint8_t));
            //int32_t received_float;
            int received_float = rx.data[0];  
            //printf("Received Int: %d\n", received_float);
            
            // IEEE 754 Float - Big Endian in PC CAN Debugger
            //printf("Received Float: %d\n", received_float);

            // 0 ~ 15 to 0 ~ 255
            // write into digital pot
            uint8_t pot_value = map_float_to_pot(received_float);
            write_pot(0x12, pot_value); // Pot_2
            //printf("Received: %.2f, Pot Output: %d\n", received_float, pot_value);
            // printf("Received: %d, Pot Output: %d\n", received_raw, received_value);
        }

        if (multicore_fifo_rvalid())
        {
            uint32_t rpm_data = multicore_fifo_pop_blocking(); // FIFO Read
            memcpy(&latest_rpm, &rpm_data, sizeof(uint8_t));     // Float conversion
            new_rpm_available = true; // new_rpm already being calculated
        }

        if (can_send_flag) {
            can_send_flag = false;  // Clear flag
            send_can_data();        // CAN data send function 
        }
    }
}

int main()
{
    stdio_init_all();
    init_spi();

    // Initialize interface
    can0.reset();
    can0.setBitrate(CAN_50KBPS, MCP_16MHZ);
    // can0.setNormalMode();

    // Config Mode
    can0.setConfigMode();

    // Mask0 & Mask1 All 11-bit need to be same
    can0.setFilterMask(MCP2515::MASK0, false, 0x7FF);
    can0.setFilterMask(MCP2515::MASK1, false, 0x7FF);

    // Filter（Only ID 0x124）
    can0.setFilter(MCP2515::RXF0, false, 0x124);
    can0.setFilter(MCP2515::RXF1, false, 0x124);
    can0.setFilter(MCP2515::RXF2, false, 0x124);
    can0.setFilter(MCP2515::RXF3, false, 0x124);
    can0.setFilter(MCP2515::RXF4, false, 0x124);
    can0.setFilter(MCP2515::RXF5, false, 0x124);

    // Change to normal mode
    can0.setNormalMode();

    struct repeating_timer timer;
    add_repeating_timer_ms(10, timerCallback, NULL, &timer); // 100 ms timer for CAN

    multicore_launch_core1(core1_entry); // Core-1
    core0_entry();                       // Core 0 

    // Listen loop
    while (true)
    {
        tight_loop_contents(); // Low-power mode in main-loop
    }

    return 0;
}