/*
Integration Test 2.1:
Implement receiving CAN data (standard data frame) from PC to Pico.
The target speed could be 4 bytes float number (IEEE 754 standard, range 0 - 15)
The digital pot could covert the speed into a value from 0 - 240 (0 - 4.7 V)
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
#define A_pin 23  // Noted that A and B should be swapped in physical connection
#define ppr 600.0 // PPR
// Sampling Time for Speed Calculation
#define sampling_time 100e-3

float latest_rpm = 0.0;         // 全局变量存最新的 RPM 数据
bool new_rpm_available = false; // 标记数据是否更新
volatile bool can_send_flag = false; // 发送标志位

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
uint8_t map_float_to_pot(float value)
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
    // tx_frame.can_id = 0x123; // 设置 CAN ID
    // tx_frame.can_dlc = 8;    // 数据长度（最大 8 字节）

    // // 填充数据
    // tx_frame.data[0] = 0x11;
    // tx_frame.data[1] = 0x22;
    // tx_frame.data[2] = 0x33;
    // tx_frame.data[3] = 0x44;
    // tx_frame.data[4] = 0x55;
    // tx_frame.data[5] = 0x66;
    // tx_frame.data[6] = 0x77;
    // tx_frame.data[7] = 0x88;

    tx_frame.can_id = 0x200; // 设置 CAN ID，可以更改
    tx_frame.can_dlc = 4;    // 发送 4 字节（IEEE 754 浮点数）

    // 将 RPM 转换为 4 字节浮点数
    memcpy(tx_frame.data, &latest_rpm, sizeof(float));

    // 发送消息
    if (can0.sendMessage(&tx_frame) == MCP2515::ERROR_OK) {
        printf("CAN Successful! ID=0x%X\n", tx_frame.can_id);
    } else {
        printf("CAN Failed!\n");
    }
}

bool timerCallback(struct repeating_timer *t) {
    can_send_flag = true;  // 仅设置标志
    return true;           // 继续运行定时器
}

void core0_entry()
{
    // Initialize PIO_Encoder Class
    QuadratureEncoder encoder(A_pin, ppr);

    while (true)
    {
        encoder.update(sampling_time); // Initialize starting time

        // Position: return angle (default radium)
        // Velocity: return velocity in rad/s
        // Counter: return counting number (CW++, ACW--, 1200 per round)
        auto position = encoder.get_position();
        auto velocity = encoder.get_velocity();
        auto counter = encoder.get_count();

        float rpm = (15 * velocity) / M_PI;

        // printf("Velocity: %.2f RPM \n, Counter: %d\n", velocity, counter);

        uint32_t rpm_data;
        memcpy(&rpm_data, &rpm, sizeof(float)); // 转换为 32 位数据
        multicore_fifo_push_blocking(rpm_data); // 传输 RPM 数据

        sleep_ms(1000); // 控制发送频率
    }
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

            // uint8_t received_raw = (uint8_t)rx.data[7];

            float received_float;
            memcpy(&received_float, &rx.data[4], sizeof(float));
            // IEEE 754 Float - Big Endian in PC CAN Debugger
            printf("Received Float: %.2f\n", received_float);

            // 0 ~ 15 to 0 ~ 255
            // received_value = map_can_to_pot(received_raw);
            // write_pot(0x12, received_value);
            // write into digital pot
            uint8_t pot_value = map_float_to_pot(received_float);
            write_pot(0x12, pot_value); // Pot_2
            printf("Received: %.2f, Pot Output: %d\n", received_float, pot_value);
            // printf("Received: %d, Pot Output: %d\n", received_raw, received_value);
        }

        if (multicore_fifo_rvalid())
        {
            uint32_t rpm_data = multicore_fifo_pop_blocking(); // 读取 FIFO
            memcpy(&latest_rpm, &rpm_data, sizeof(float));     // 解析浮点数
            printf("Core 1 - RPM: %.2f\n", latest_rpm);
            new_rpm_available = true; // 标记新数据到来
        }

        if (can_send_flag) {
            can_send_flag = false;  // 清除标志位
            send_can_data();        // 发送 CAN 数据
        }
        // 构建一个CAN帧
        // //struct can_frame tx_frame;
        // tx_frame.can_id = 0x123; // CAN ID
        // tx_frame.can_dlc = 8;    // 数据长度（最大8字节）
        // tx_frame.data[0] = 0x11; // 数据内容
        // tx_frame.data[1] = 0x22;
        // tx_frame.data[2] = 0x33;
        // tx_frame.data[3] = 0x44;
        // tx_frame.data[4] = 0x55; // 数据内容
        // tx_frame.data[5] = 0x66;
        // tx_frame.data[6] = 0x77;
        // tx_frame.data[7] = 0x88;

        // // 发送消息
        // if (can0.sendMessage(&tx_frame) == MCP2515::ERROR_OK)
        // {
        //     printf("Message sent successfully!\n");
        // }
        // else
        // {
        //     printf("Failed to send message\n");
        // }
    }
}

int main()
{
    stdio_init_all();
    init_spi();

    // // Pin_A (Pin_B would be set as A_pin + 1)
    // uint8_t A_pin = 23; // Noted that A and B should be swapped in physical connection
    // float ppr = 600.0;  //

    // // Initialize PIO_Encoder Class
    // QuadratureEncoder encoder(A_pin, ppr);

    // // Sampling Time for Speed Calculation
    // float sampling_time = 100e-3;

    // Digital Pot settings (Testing)
    // int digital_pot_output = 127; // 0 - 255 (-100% to +100%)

    // Initialize interface
    can0.reset();
    can0.setBitrate(CAN_100KBPS, MCP_16MHZ);
    // can0.setNormalMode();

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

    // // 构建一个CAN帧
    // struct can_frame tx_frame;
    // tx_frame.can_id = 0x123; // CAN ID
    // tx_frame.can_dlc = 4;    // 数据长度（最大8字节）
    // tx_frame.data[0] = 0x11; // 数据内容
    // tx_frame.data[1] = 0x22;
    // tx_frame.data[2] = 0x33;
    // tx_frame.data[3] = 0x44;

    // // 发送消息
    // if (can0.sendMessage(&tx_frame) == MCP2515::ERROR_OK)
    // {
    //     printf("Message sent successfully!\n");
    // }
    // else
    // {
    //     printf("Failed to send message\n");
    // }

    struct repeating_timer timer;
    add_repeating_timer_ms(1000, timerCallback, NULL, &timer); // 每 1000ms 触发一次

    // struct repeating_timer timer;
    // add_repeating_timer_ms(5000, send_can_data, NULL, &timer);

    multicore_launch_core1(core1_entry); // Core-1
    core0_entry();                       // Core 0 直接运行

    // Listen loop
    while (true)
    {
        // if (can0.readMessage(&rx) == MCP2515::ERROR_OK)
        // {
        //     int8_t received_value = (int8_t)rx.data[0];

        //     int8_t digital_pot_output = map_can_to_pot(received_value);

        //     printf("New frame from ID: %10x\n", rx.can_id);
        //     int setting_value = (int)digital_pot_output;
        //     printf("Digital Pot: %10x\n", setting_value);
        //     write_pot(0x11, setting_value);

        //     // Print the received data (if any)
        //     printf("Data: ");
        //     for (int i = 0; i < rx.can_dlc; i++)
        //     {
        //         printf("%02X ", rx.data[i]);
        //     }
        //     printf("\n");
        // }

        // encoder.update(sampling_time); // Initialize starting time

        // // Position: return angle (default radium)
        // // Velocity: return velocity in rad/s
        // // Counter: return counting number (CW++, ACW--, 1200 per round)
        // auto position = encoder.get_position();
        // auto velocity = encoder.get_velocity();
        // auto counter = encoder.get_count();

        // float rpm = (15 * velocity) / M_PI;

        // printf("Velocity: %.2f RPM \n, Counter: %d\n", velocity, counter);

        // sleep_ms(1000);

        tight_loop_contents(); // Low-power mode in main-loop
    }

    return 0;
}