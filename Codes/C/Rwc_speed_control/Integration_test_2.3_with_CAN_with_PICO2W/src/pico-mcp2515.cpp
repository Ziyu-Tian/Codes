// /*

// **Updates for Integration 2.1:
// - Integrate RPM Serial output with CAN Sending Message, provides the
// possibility to compare latency between serial output and CAN Master.
// - Multi-core implementation: Core-0 read encoder ; Core-1 process CAN TX/RX
// - It is estimated the RPM is correct and almost same in CAN side and Serial
// output.

// **Updates for Integration Test 2.2:
// - Implement receiving CAN data (standard data frame) from PC to Pico.
// - The target speed could be 4 bytes float number (IEEE 754 standard, range 0 - 15)
// - The digital pot could covert the speed into a value from 0 - 240 (0 - 4.7 V)

// **Updates for Integration Test 2.3:
// - Multi-encoder supported
// - Average float number being TX via CAN

// */

// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "hardware/timer.h"
// #include "mcp2515/mcp2515.h"
// #include "hardware/spi.h"
// #include "pico/multicore.h"
// #include <cstring>
// #include <QuadratureEncoder.hpp>

// #define UART_ID uart1
// #define UART_TX_PIN 0
// #define UART_RX_PIN 1
// #define BAUD_RATE 115200

// // CAN Config
// MCP2515 can0;
// struct can_frame rx;
// struct can_frame tx_frame;
// volatile uint8_t received_value = 0; // received CAN data

// // SPI PIN Config
// #define SPI_PORT spi0 // spi0 applied
// #define PIN_SCK 2     // SCK = GPIO2
// #define PIN_MOSI 3    // MOSI = GPIO3
// #define PIN_MISO 4    // MISO
// #define PIN_CS 8      // CS

// int latest_rpm = 0;                // Global latest_rpm
// bool new_rpm_available = false;      // new_rpm_calculated_flag
// volatile bool can_send_flag = false; // can_send_flag

// void init_uart()
// {
//     uart_deinit(UART_ID);
//     uart_init(UART_ID, BAUD_RATE);
//     gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
//     gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
//     uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
// }

// // SPI Init Function
// void init_spi()
// {
//     spi_init(SPI_PORT, 1000 * 1000); // 1Mhz SPI
//     gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
//     gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
//     gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

//     gpio_init(PIN_CS);
//     gpio_set_dir(PIN_CS, GPIO_OUT);
//     gpio_put(PIN_CS, 1); // Unselect chip
// }

// // Sending SPI cmd + data to Digital Pot-0
// void write_pot(uint8_t cmd, uint8_t value)
// {
//     uint8_t data[2] = {cmd, value};

//     gpio_put(PIN_CS, 0); // Select chip, start sending data
//     spi_write_blocking(SPI_PORT, data, 2);
//     gpio_put(PIN_CS, 1); // Unselect chip, end data sending
// }

// // Digital Pot Mapping - Int
// uint8_t map_can_to_pot(uint8_t can_value)
// {
//     return can_value * (240.0 / 15.0); // 0 ~ 15 --> 0 ~ 240
// }

// // Digital Pot Mapping - Float
// uint8_t map_float_to_pot(float value)
// {

//     if (value < -15.0)
//         value = -15.0;
//     if (value > 15.0)
//         value = 15.0;

//     // Linear Mapping: (0~ 15) → (0 ~ 240)
//     return (uint8_t)(value * (240.0 / 15.0));
// }

// void send_can_data()
// {
//     struct can_frame tx_frame;
//     tx_frame.can_id = 0x200; // CAN ID
//     tx_frame.can_dlc = 4;    // IEEE 754 Float

//     // RPM to float
//     memcpy(tx_frame.data, &latest_rpm, sizeof(float));

//     // Send Message
//     if (can0.sendMessage(&tx_frame) == MCP2515::ERROR_OK)
//     {
//         printf("CAN Successful! ID=0x%X\n", tx_frame.can_id);
//         printf("Core 1 - RPM: %d\n", latest_rpm);
//     }
//     else
//     {
//         printf("CAN Failed!\n");
//     }
// }

// bool timerCallback(struct repeating_timer *t)
// {
//     can_send_flag = true; // Set send_flag
//     return true;          // continue timer
// }

// int main()
// {
//     // stdio_init_all();
//     stdio_usb_init();
//     init_spi();
//     init_uart();

//     // Initialize interface
//     can0.reset();
//     can0.setBitrate(CAN_500KBPS, MCP_16MHZ);
//     // can0.setNormalMode();

//     // Config Mode
//     can0.setConfigMode();

//     // Mask0 & Mask1 All 11-bit need to be same
//     can0.setFilterMask(MCP2515::MASK0, false, 0x7FF);
//     can0.setFilterMask(MCP2515::MASK1, false, 0x7FF);

//     // Filter（Only ID 0x123）
//     can0.setFilter(MCP2515::RXF0, false, 0x123);
//     can0.setFilter(MCP2515::RXF1, false, 0x123);
//     can0.setFilter(MCP2515::RXF2, false, 0x123);
//     can0.setFilter(MCP2515::RXF3, false, 0x123);
//     can0.setFilter(MCP2515::RXF4, false, 0x123);
//     can0.setFilter(MCP2515::RXF5, false, 0x123);

//     // Change to normal mode
//     can0.setNormalMode();

//     struct repeating_timer timer;
//     add_repeating_timer_ms(1000, timerCallback, NULL, &timer); // 100 ms timer for CAN

//     // Listen loop
//     while (true)
//     {
//         if (can0.readMessage(&rx) == MCP2515::ERROR_OK)
//         {
//             // CAN Data (0-15)
//             printf("Raw CAN Data: ");
//             for (int i = 0; i < rx.can_dlc; i++)
//             {
//                 printf("%02X ", rx.data[i]); // Print each bytes
//             }
//             printf("\n");

//             float received_float;
//             memcpy(&received_float, &rx.data[4], sizeof(float));
//             // IEEE 754 Float - Big Endian in PC CAN Debugger
//             printf("Received Float: %.2f\n", received_float);

//             // 0 ~ 15 to 0 ~ 255
//             // write into digital pot
//             uint8_t pot_value = map_float_to_pot(received_float);
//             write_pot(0x12, pot_value); // Pot_2
//             printf("Received: %.2f, Pot Output: %d\n", received_float, pot_value);
//             // printf("Received: %d, Pot Output: %d\n", received_raw, received_value);
//         }

//         // if (multicore_fifo_rvalid())
//         // {
//         //     uint32_t rpm_data = multicore_fifo_pop_blocking(); // FIFO Read
//         //     memcpy(&latest_rpm, &rpm_data, sizeof(float));     // Float conversion
//         //     new_rpm_available = true; // new_rpm already being calculated
//         // }

//         // if (uart_is_readable(UART_ID))
//         // {
//         //     uint8_t buffer[4];
//         //     uart_read_blocking(UART_ID, buffer, 4);
//         //     memcpy(&latest_rpm, buffer, sizeof(float));
//         //     new_rpm_available = true;
//         // }

//         if (uart_is_readable(UART_ID))
//         {
//             uint8_t rx_high_byte = uart_getc(UART_ID);            // 接收高字节
//             uint8_t rx_low_byte = uart_getc(UART_ID);             // 接收低字节
//             uint16_t rx_data = (rx_low_byte << 8) | rx_high_byte; // 合并字节为 16 位数据
//                                                                   // 打印接收到的字节
//             // printf("Received 2 Bytes: High Byte: %d, Low Byte: %d, Combined Data: %d\n", rx_high_byte, rx_low_byte, rx_data);
//                 memcpy(&latest_rpm, rx_data, sizeof(int));
//                 new_rpm_available = true;
//             printf("UART Received Data: %d\n", rx_data);
//         }
//         else
//         {
//             printf("No Data Received!\n");
//         }

//         if (can_send_flag)
//         {
//             can_send_flag = false; // Clear flag
//             send_can_data();       // CAN data send function
//         }
//     }

//     return 0;
// }

#include <QuadratureEncoder.hpp>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

#define UART_ID uart0
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define BAUD_RATE 115200

void init_uart()
{
    uart_deinit(UART_ID);
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
}

int main()
{
    
    //stdio_init_all();
    init_uart();
    stdio_usb_init();
    while (true)
    {
        // 发送 2 字节的数据
        uint16_t tx_data = 1234;                // 作为示例，发送 1234
        uart_putc(UART_ID, (tx_data >> 8) & 0xFF); // 发送高字节
        uart_putc(UART_ID, tx_data & 0xFF);        // 发送低字节
        // printf("Combined Data: %d\n",tx_data);

        sleep_ms(500);

        // // 接收 2 字节的数据
        if (uart_is_readable(UART_ID))
        {
            uint8_t rx_high_byte = uart_getc(UART_ID);            // 接收高字节
            uint8_t rx_low_byte = uart_getc(UART_ID);             // 接收低字节
            uint16_t rx_data = (rx_low_byte << 8) | rx_high_byte; // 合并字节为 16 位数据
                                                                  // 打印接收到的字节
            // printf("Received 2 Bytes: High Byte: %d, Low Byte: %d, Combined Data: %d\n", rx_high_byte, rx_low_byte, rx_data);
            printf("Combined Data: %d\n", rx_data);
        }
        else
        {
            printf("No Data Received!\n");
        }
    }

    return 0;
}
