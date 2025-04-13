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
// #include <QuadratureEncoder.hpp>
// #include "pico/stdlib.h"
// #include "hardware/spi.h"
// #include <stdio.h>
// #include "hardware/uart.h"
// #include <string.h>

// #define UART_ID uart0
// #define UART_TX_PIN 16
// #define UART_RX_PIN 17
// #define BAUD_RATE 115200

// void init_uart()
// {
//     uart_deinit(UART_ID);
//     uart_init(UART_ID, BAUD_RATE);
//     gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
//     gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
//     uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
// }

// int main()
// {

//     stdio_usb_init();//stdio_init_all();
//     init_uart();

//     // Pin_A (Pin_B would be set as A_pin + 1)
//     uint8_t A_pin = 4; // Noted that A and B should be swapped in physical connection
//     uint8_t A2_pin = 20;
//     uint8_t A3_pin = 16;
//     float ppr = 600.0;

//     // Initialize PIO_Encoder Class
//     QuadratureEncoder encoder_1(pio0, A_pin, ppr);
//     QuadratureEncoder encoder_2(pio1, A2_pin, ppr);
//     QuadratureEncoder encoder_3(pio2, A3_pin, ppr);
//     // Sampling Time for Speed Calculation
//     float sampling_time = 500e-3;

//     while (true)
//     {

//         encoder_1.update(sampling_time); // Initialize starting time
//         encoder_2.update(sampling_time); // Initialize starting time
//         encoder_3.update(sampling_time); // Initialize starting time
//         // Position: return angle (default radium)
//         // Velocity: return velocity in rad/s
//         // Counter: return counting number (CW++, ACW--, 1200 per round)
//         auto position = encoder_1.get_position();
//         auto velocity = encoder_1.get_velocity();
//         auto counter = encoder_1.get_count();

//         auto position_2 = encoder_2.get_position();
//         auto velocity_2 = encoder_2.get_velocity();
//         auto counter_2 = encoder_2.get_count();

//         auto position_3 = encoder_3.get_position();
//         auto velocity_3 = encoder_3.get_velocity();
//         auto counter_3 = encoder_3.get_count();

//         float rpm_1 = (30 * velocity) / M_PI;
//         float rpm_2 = (30 * velocity_2) / M_PI;
//         float rpm_3 = (30 * velocity_3) / M_PI;

//         float rpm_avg = (rpm_1 + rpm_2 + rpm_3) / 3.0;

//         // printf("Velocity_1: %.2f RPM , Counter_1: %d ;", rpm_1, counter);
//         // printf("Velocity_2: %.2f RPM , Counter_2: %d\n", rpm_2, counter_2);
//         // printf("RPM_1: %.2f, ",rpm_1);
//         // printf("RPM_2: %.2f, ",rpm_2);
//         // printf("RPM_3: %.2f\n",rpm_3);

//         uart_write_blocking(UART_ID, (uint8_t *)&rpm_avg, sizeof(rpm_avg));
//         printf("Sent RPM: %.2f\n", rpm_avg);

//         // if (uart_is_readable(UART_ID))
//         // {
//         //     // uint8_t received_data = uart_getc(UART_ID);
//         //     // printf("Echoed data: %c\n", received_data);
//         //     float received_rpm_avg;
//         //     uint8_t buffer[sizeof(float)];
//         //     uart_read_blocking(UART_ID, buffer, sizeof(buffer));
//         //     memcpy(&received_rpm_avg, buffer, sizeof(received_rpm_avg));
//         //     printf("Echoed data: %.2f\n", received_rpm_avg);
//         //     printf("Test\n");
//         // }

//         if (uart_is_readable(UART_ID)) {
//             uint8_t buffer[sizeof(float)];
//             uart_read_blocking(UART_ID, buffer, sizeof(buffer));

//             float received_rpm_avg;
//             memcpy(&received_rpm_avg, buffer, sizeof(received_rpm_avg));

//             printf("Echoed data: %.2f\n", received_rpm_avg);
//         } else {
//             printf("No data received yet.\n");
//         }

//         sleep_ms(500);
//     }

//     return 0;
// }


// #include "pico/stdlib.h"
// #include "hardware/uart.h"
// #include <stdio.h>

// #define UART_ID uart0
// #define UART_TX_PIN 16
// #define UART_RX_PIN 17
// #define BAUD_RATE 115200

// void init_uart()
// {
//     uart_deinit(UART_ID);
//     uart_init(UART_ID, BAUD_RATE);
//     gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
//     gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
//     uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
// }

// int main()
// {
    
//     init_uart();
//     stdio_usb_init();
//     while (true)
//     {
//         // 发送 2 字节的数据
//         uint16_t tx_data = 12;                   // 作为示例，发送 1234
//         uart_putc(UART_ID, (tx_data >> 8) & 0xFF); // 发送高字节
//         uart_putc(UART_ID, tx_data & 0xFF);        // 发送低字节
//         printf("Sent 2 Bytes: High Byte: %d, Low Byte: %d\n", (tx_data >> 8) & 0xFF, tx_data & 0xFF);

//         sleep_ms(500);

//         // 接收 2 字节的数据
//         if (uart_is_readable(UART_ID))
//         {
//             uint8_t rx_high_byte = uart_getc(UART_ID);            // 接收高字节
//             uint8_t rx_low_byte = uart_getc(UART_ID);             // 接收低字节
//             uint16_t rx_data = (rx_low_byte << 8) | rx_high_byte; // 合并字节为 16 位数据
//                                                                   // 打印接收到的字节
//             printf("Received 2 Bytes: High Byte: %d, Low Byte: %d, Combined Data: %d\n", rx_high_byte, rx_low_byte, rx_data);
//         }
//         else
//         {
//             printf("No Data Received!\n");
//         }
//     }

//     return 0;
// }



#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

/// \tag::hello_uart[]

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 16
#define UART_RX_PIN 17

int main() {
    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));

    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART

    // Send out a character without any conversions
    uart_putc_raw(UART_ID, 'A');

    // Send out a character but do CR/LF conversions
    uart_putc(UART_ID, 'B');

    // Send out a string, with CR/LF conversions
    uart_puts(UART_ID, " Hello, UART!\n");
    return 0;
}