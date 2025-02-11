
// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "hardware/i2c.h"

// void i2c_scan(i2c_inst_t *i2c) {
//     printf("Scanning I2C bus...\n");

//     for (int addr = 0x08; addr < 0x78; addr++) {  // I2C 7-bit 地址范围
//         int result = i2c_write_blocking(i2c, addr, NULL, 0, false);
//         if (result >= 0) {
//             printf("Found I2C device at 0x%02X\n", addr);
//         }
//     }

//     printf("I2C scan complete.\n");
// }

// int main() {
//     // I2C Pins
//     const uint sda_pin = 6;
//     const uint scl_pin = 7;
//     i2c_inst_t *i2c = i2c1;

//     // 初始化串口（等待 USB 连接）
//     stdio_init_all();
//     while (!stdio_usb_connected()) {
//         sleep_ms(100);
//     }
//     sleep_ms(2000);  // 额外等待 2 秒
//     printf("I2C Scanner Starting...\n");

//     // 初始化 I2C
//     i2c_init(i2c, 400 * 1000);
//     gpio_set_function(sda_pin, GPIO_FUNC_I2C);
//     gpio_set_function(scl_pin, GPIO_FUNC_I2C);
//     gpio_pull_up(sda_pin);
//     gpio_pull_up(scl_pin);

//     i2c_scan(i2c);  // 扫描 I2C 设备

//     while (true) {
//         sleep_ms(1000);
//     }
// }
//----------------------------------------

// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "hardware/i2c.h"

// #define ENCODER_ADDR 0x49
// #define SEESAW_ENCODER_BASE 0x11
// #define SEESAW_ENCODER_POSITION 0x30

// int32_t read_encoder_position(i2c_inst_t *i2c) {
//     uint8_t cmd[2] = {SEESAW_ENCODER_BASE, SEESAW_ENCODER_POSITION};
//     uint8_t buffer[4];

//     // 先写入寄存器地址
//     int result = i2c_write_blocking(i2c, ENCODER_ADDR, cmd, 2, true);
//     if (result < 0) {
//         printf("I2C write failed!\n");
//         return -1;
//     }

//     // 然后读取 4 字节的数据
//     result = i2c_read_blocking(i2c, ENCODER_ADDR, buffer, 4, false);
//     if (result < 0) {
//         printf("I2C read failed!\n");
//         return -1;
//     }

//     // 将 4 字节数据组合成 32 位整数
//     int32_t position = (int32_t)((buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3]);

//     return position;
// }

// int main() {
//     // I2C 引脚
//     const uint sda_pin = 6;
//     const uint scl_pin = 7;
//     i2c_inst_t *i2c = i2c1;

//     // 初始化串口（等待 USB 连接）
//     stdio_init_all();
//     while (!stdio_usb_connected()) {
//         sleep_ms(100);
//     }
//     sleep_ms(2000);  // 额外等待 2 秒
//     printf("I2C Scanner Starting...\n");

//     // 初始化 I2C
//     i2c_init(i2c, 400 * 1000);
//     gpio_set_function(sda_pin, GPIO_FUNC_I2C);
//     gpio_set_function(scl_pin, GPIO_FUNC_I2C);
//     gpio_pull_up(sda_pin);
//     gpio_pull_up(scl_pin);

//     printf("Starting I2C Encoder Read...\n");

//     while (true) {
//         int32_t encoder_position = read_encoder_position(i2c);
//         if (encoder_position != -1) {
//             printf("Encoder Position: %ld\n", encoder_position);
//         }
//         sleep_ms(1000);
//     }
// }

/*------------------------------Without Filter-------------------------------*/

// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "hardware/i2c.h"

// #define ENCODER_ADDR 0x49
// #define SEESAW_ENCODER_BASE 0x11
// #define SEESAW_ENCODER_POSITION 0x30
// #define SEESAW_ENCODER_INTENSET 0x10
// #define SEESAW_ENCODER_INTFLAG 0x44

// int32_t read_encoder_position(i2c_inst_t *i2c)
// {
//     uint8_t cmd[2] = {SEESAW_ENCODER_BASE, SEESAW_ENCODER_POSITION};
//     uint8_t buffer[4];

//     // 先写入寄存器地址
//     int result = i2c_write_blocking(i2c, ENCODER_ADDR, cmd, 2, true);
//     if (result < 0)
//     {
//         printf("I2C write failed!\n");
//         return -1;
//     }

//     // 然后读取 4 字节的数据
//     result = i2c_read_blocking(i2c, ENCODER_ADDR, buffer, 4, false);
//     if (result < 0)
//     {
//         printf("I2C read failed!\n");
//         return -1;
//     }

//     int32_t position = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) |
//                        ((uint32_t)buffer[2] << 8) | (uint32_t)buffer[3];

//     return position;
// }

// // 启用编码器中断
// void enable_encoder_interrupt(i2c_inst_t *i2c)
// {
//     uint8_t cmd[2] = {SEESAW_ENCODER_BASE, SEESAW_ENCODER_INTENSET};
//     uint8_t enable_interrupt = 0x01; // 假设是启用编码器中断的命令
//     i2c_write_blocking(i2c, ENCODER_ADDR, cmd, 2, true);
//     i2c_write_blocking(i2c, ENCODER_ADDR, &enable_interrupt, 1, false);
// }

// int main()
// {
//     // I2C 引脚
//     const uint sda_pin = 6;
//     const uint scl_pin = 7;
//     i2c_inst_t *i2c = i2c1;

//     // 初始化串口（等待 USB 连接）
//     stdio_init_all();
//     while (!stdio_usb_connected())
//     {
//         sleep_ms(100);
//     }
//     sleep_ms(2000); // 额外等待 2 秒
//     printf("I2C Scanner Starting...\n");

//     // 初始化 I2C
//     i2c_init(i2c, 400 * 1000);
//     gpio_set_function(sda_pin, GPIO_FUNC_I2C);
//     gpio_set_function(scl_pin, GPIO_FUNC_I2C);
//     gpio_pull_up(sda_pin);
//     gpio_pull_up(scl_pin);

  

//     printf("Starting I2C Encoder Read...\n");

//     while (true)
//     {
//             int32_t encoder_position = read_encoder_position(i2c);
//             enable_encoder_interrupt(i2c);
//             if (encoder_position != -1) {
//                 printf("Encoder Position: %ld\n", encoder_position);
//             }
//             sleep_ms(1000);
//         // }
//     }
// }

/*--------------------------With Filter--------------------------------------*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define ENCODER_ADDR 0x49
#define SEESAW_ENCODER_BASE 0x11
#define SEESAW_ENCODER_POSITION 0x30
#define SEESAW_ENCODER_INTENSET 0x10
#define SEESAW_ENCODER_INTFLAG 0x44

#define FILTER_ALPHA 0.5f  // 低通滤波器系数
#define FILTER_WINDOW_SIZE 5  // 滤波窗口大小

// 存储滤波后的数据
int32_t encoder_position_buffer[FILTER_WINDOW_SIZE];  // 存储编码器位置历史数据
float filtered_rpm = 0.0f;  // 滤波后的转速
uint8_t buffer_index = 0;  // 缓冲区索引

// 存储最后的编码器位置和时间
int32_t last_position = 0;
uint32_t last_time = 0;

// 读取编码器位置
int32_t read_encoder_position(i2c_inst_t *i2c)
{
    uint8_t cmd[2] = {SEESAW_ENCODER_BASE, SEESAW_ENCODER_POSITION};
    uint8_t buffer[4];

    // 先写入寄存器地址
    int result = i2c_write_blocking(i2c, ENCODER_ADDR, cmd, 2, true);
    if (result < 0)
    {
        printf("I2C write failed!\n");
        return -1;
    }

    // 然后读取 4 字节的数据
    result = i2c_read_blocking(i2c, ENCODER_ADDR, buffer, 4, false);
    if (result < 0)
    {
        printf("I2C read failed!\n");
        return -1;
    }

    int32_t position = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) |
                       ((uint32_t)buffer[2] << 8) | (uint32_t)buffer[3];

    return position;
}

// 启用编码器中断
void enable_encoder_interrupt(i2c_inst_t *i2c)
{
    uint8_t cmd[2] = {SEESAW_ENCODER_BASE, SEESAW_ENCODER_INTENSET};
    uint8_t enable_interrupt = 0x01; // 假设是启用编码器中断的命令
    i2c_write_blocking(i2c, ENCODER_ADDR, cmd, 2, true);
    i2c_write_blocking(i2c, ENCODER_ADDR, &enable_interrupt, 1, false);
}

// 低通滤波器应用于转速
float low_pass_filter(float current_rpm)
{
    filtered_rpm = FILTER_ALPHA * current_rpm + (1 - FILTER_ALPHA) * filtered_rpm;
    return filtered_rpm;
}

// 计算转速（RPM）
float calculate_rpm(int32_t current_position, uint32_t current_time)
{
    uint32_t delta_time = current_time - last_time;  // 计算时间间隔（毫秒）
    if (delta_time == 0) return 0;

    int32_t delta_position = current_position - last_position;  // 位置变化
    float rpm = (float)delta_position / delta_time * 1000.0f;  // 计算转速

    // 更新记录
    last_position = current_position;
    last_time = current_time;

    return rpm;
}

// 更新位置滤波缓存
void update_position_buffer(int32_t new_position)
{
    encoder_position_buffer[buffer_index] = new_position;
    buffer_index = (buffer_index + 1) % FILTER_WINDOW_SIZE;  // 循环更新
}

// 计算滤波后的平均位置
int32_t calculate_filtered_position()
{
    int32_t sum = 0;
    for (int i = 0; i < FILTER_WINDOW_SIZE; i++)
    {
        sum += encoder_position_buffer[i];
    }
    return sum / FILTER_WINDOW_SIZE;  // 返回平均值
}

int main()
{
    // I2C 引脚
    const uint sda_pin = 6;
    const uint scl_pin = 7;
    i2c_inst_t *i2c = i2c1;

    // 初始化串口（等待 USB 连接）
    stdio_init_all();
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }
    sleep_ms(2000); // 额外等待 2 秒
    printf("I2C Scanner Starting...\n");

    // 初始化 I2C
    i2c_init(i2c, 400 * 1000);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

    enable_encoder_interrupt(i2c);

    printf("Starting I2C Encoder Read...\n");

    while (true)
    {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());  // 获取当前时间（毫秒）
        int32_t current_position = read_encoder_position(i2c);  // 读取编码器位置

        if (current_position != -1)
        {
            // 计算转速并应用滤波
            float rpm = calculate_rpm(current_position, current_time);
            float filtered_rpm_value = low_pass_filter(rpm);  // 应用低通滤波器

            // 更新位置滤波缓存
            update_position_buffer(current_position);
            int32_t filtered_position = calculate_filtered_position();  // 计算滤波后的平均位置

            printf("Filtered Position: %ld, Filtered RPM: %.2f\n", filtered_position, filtered_rpm_value);
        }

        sleep_ms(1000);  // 小间隔等待下一次更新
    }
}
