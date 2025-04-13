/*
**Updates for Integration Test 2.2:
- Implement receiving CAN data (standard data frame) from PC to Pico.
- The target speed could be 4 bytes float number (IEEE 754 standard, range 0 - 15)
- The digital pot could covert the speed into a value from 0 - 240 (0 - 4.7 V)
**Updates for Integration 2.1:
- Integrate RPM Serial output with CAN Sending Message, provides the
possibility to compare latency between serial output and CAN Master.
- Multi-core implementation: Core-0 read encoder ; Core-1 process CAN TX/RX
- It is estimated the RPM is correct and almost same in CAN side and Serial
output.
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "mcp2515/mcp2515.h"
#include "hardware/spi.h"
#include "pico/multicore.h"
#include <cstring>
#include <QuadratureEncoder.hpp>
#include "hardware/adc.h"

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
#define A_pin 23  // Noted that A and B should be swapped in physical connection
#define A2_pin 21 // 12
#define A3_pin 19 //
#define ppr 600.0 // PPR
// Sampling Time for Speed Calculation
#define sampling_time 10e-3

int latest_rpm = 0;                  // Global latest_rpm
bool new_rpm_available = false;      // new_rpm_calculated_flag
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
uint8_t map_can_to_pot_int(uint8_t value)
{
    return (uint8_t)value * (255.0 / 15.0); // 0 ~ 15 --> 0 ~ 240
}

// Digital Pot Mapping - Float
uint8_t map_float_to_pot(float value)
{

    if (value < -15.0)
        value = -15.0;
    if (value > 15.0)
        value = 15.0;

    // Linear Mapping: (0~ 15) → (0 ~ 240)
    return (uint8_t)(value * (255.0 / 15.0));
}

void send_can_data()
{
    struct can_frame tx_frame;
    tx_frame.can_id = 0x126; // CAN ID
    tx_frame.can_dlc = 8;    // IEEE 754 Float

    // RPM to float
    //memcpy(tx_frame.data, &latest_rpm, sizeof(float));

  
    memset(tx_frame.data, 0, sizeof(tx_frame.data)); 


    memcpy(tx_frame.data, &latest_rpm, sizeof(float)); 

    // Send Message
    if (can0.sendMessage(&tx_frame) == MCP2515::ERROR_OK)
    {
        printf("CAN Successful! ID=0x%X\n", tx_frame.can_id);
        printf("Core 1 - RPM: %.2f\n", latest_rpm);
    }
    else
    {
        printf("CAN Failed!\n");
    }
}

bool timerCallback(struct repeating_timer *t)
{
    can_send_flag = true; // Set send_flag
    return true;          // continue timer
}

void core0_entry()
{
    // Initialize PIO_Encoder Class
    QuadratureEncoder encoder(pio1, A_pin, ppr);
    QuadratureEncoder encoder_2(pio0, A2_pin, ppr);
    QuadratureEncoder encoder_3(pio1, A3_pin, ppr);

    while (true)
    {
        encoder.update(sampling_time); // Initialize starting time
        encoder_2.update(sampling_time); // Initialize starting time 
        encoder_3.update(sampling_time); // Initialize starting time 

        // Position: return angle (default radium)
        // Velocity: return velocity in rad/s
        // Counter: return counting number (CW++, ACW--, 1200 per round)
        auto position = encoder.get_position();
        auto velocity = encoder.get_velocity();
        auto counter = encoder.get_count();

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
        printf("Core 1 - RPM: %d\n", rpm);
        memcpy(&rpm_data, &rpm, sizeof(int));   // change to 32 bits
        multicore_fifo_push_blocking(rpm_data); // TX RPM data

        sleep_ms(10); // Send frequency
    }
}

float read_voltage(uint channel) {

    adc_select_input(channel);
    uint16_t raw = adc_read(); 
    
    float voltage = raw * 3.3f / 4095.0f;
    return voltage;
}

void core1_entry()
{
    while (true)
    {
        if (can0.readMessage(&rx) == MCP2515::ERROR_OK)
        {
            // CAN Data (0-15)
            printf("Raw CAN Data: ");
            for (int i = 0; i < rx.can_dlc; i++)
            {
                printf("%02X ", rx.data[i]); // Print each bytes
            }
            printf("\n");

            int received_float = rx.data[0];
            //memcpy(&received_float, &rx.data[0], sizeof(int));
            printf("Received Float: %d\n", received_float);

            // 0 ~ 15 to 0 ~ 255
            // write into digital pot
            //uint8_t pot_value = map_float_to_pot(received_float);
            uint8_t pot_value = map_can_to_pot_int(received_float);
            //write_pot(0x21,0);
            write_pot(0x12, 2); // Pot_2

            float voltage0 = read_voltage(0);  // A0
            printf("Received: %d, Voltage Output: %.5f, Pot Output: %d\n", received_float, voltage0, pot_value);
            // printf("Received: %d, Pot Output: %d\n", received_raw, received_value);
        }

        if (multicore_fifo_rvalid())
        {
            uint32_t rpm_data = multicore_fifo_pop_blocking(); // FIFO Read
            memcpy(&latest_rpm, &rpm_data, sizeof(int));       // Float conversion
            new_rpm_available = true;                          // new_rpm already being calculated
        }

        if (can_send_flag)
        {
            can_send_flag = false; // Clear flag
            send_can_data();       // CAN data send function
        }
    }
}



int main()
{
    stdio_init_all();
    init_spi();
    adc_init();

    // Initialize interface
    can0.reset();
    can0.setBitrate(CAN_50KBPS, MCP_16MHZ);
    // can0.setNormalMode();

    // Config Mode
    can0.setConfigMode();

    // Mask0 & Mask1 All 11-bit need to be same
    can0.setFilterMask(MCP2515::MASK0, false, 0x7FF);
    can0.setFilterMask(MCP2515::MASK1, false, 0x7FF);

    // Filter（Only ID 0x126）
    can0.setFilter(MCP2515::RXF0, false, 0x126);
    can0.setFilter(MCP2515::RXF1, false, 0x126);
    can0.setFilter(MCP2515::RXF2, false, 0x126);
    can0.setFilter(MCP2515::RXF3, false, 0x126);
    can0.setFilter(MCP2515::RXF4, false, 0x126);
    can0.setFilter(MCP2515::RXF5, false, 0x126);

    // Change to normal mode
    can0.setNormalMode();

    struct repeating_timer timer;
    add_repeating_timer_ms(100, timerCallback, NULL, &timer); // 100 ms timer for CAN

    multicore_launch_core1(core1_entry); // Core-1
    core0_entry();                       // Core 0

    // Listen loop
    while (true)
    {
        tight_loop_contents(); // Low-power mode in main-loop
    }

    return 0;
}