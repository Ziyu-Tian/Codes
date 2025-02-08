/*
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/uart.h"

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5



int main()
{
    stdio_init_all();

    // Timer example code - This example fires off the callback after 2000ms
    add_alarm_in_ms(2000, alarm_callback, NULL, false);
    // For more examples of timer use see https://github.com/raspberrypi/pico-examples/tree/master/timer

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    
    // Send out a string, with CR/LF conversions
    uart_puts(UART_ID, " Hello, UART!\n");
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define ENCODER_A 23    // GPIO Pin for Encoder A
#define ENCODER_B 24    // GPIO Pin for Encoder B
#define PPR 600 // Pulse per Revolution for encoder E38S6G5-600B-G24N
#define SAMPLE_TIME_MS 100  // Sampling Time

volatile int pulse_count = 0;  // Pulses Number in sampling time 
volatile int encoder_position = 0;  // Encoder position, volatile to prevent optimization
volatile bool last_A = 0;  // Last A state for direction detection


void encoder_isr(uint gpio, uint32_t events) {
    bool A = gpio_get(ENCODER_A);
    bool B = gpio_get(ENCODER_B);

  
    //printf("Interrupt triggered: A = %d, B = %d\n", A, B);

    
    if (A != last_A) {
        
        encoder_position += (A ^ B) ? 1 : -1;
        pulse_count++; 
        last_A = A;  

        
        //printf("Encoder position: %d\n", encoder_position);
    }
}


void encoder_init() {
    gpio_init(ENCODER_A);
    gpio_set_dir(ENCODER_A, GPIO_IN);
    gpio_pull_up(ENCODER_A);
    gpio_set_irq_enabled_with_callback(ENCODER_A, GPIO_IRQ_EDGE_RISE, true, &encoder_isr);
    gpio_set_irq_enabled_with_callback(ENCODER_A, GPIO_IRQ_EDGE_FALL, true, &encoder_isr);

    gpio_init(ENCODER_B);
    gpio_set_dir(ENCODER_B, GPIO_IN);
    gpio_pull_up(ENCODER_B);
    //gpio_set_irq_enabled_with_callback(ENCODER_B, GPIO_IRQ_EDGE_RISE, true, &encoder_isr);
    //gpio_set_irq_enabled_with_callback(ENCODER_B, GPIO_IRQ_EDGE_FALL, true, &encoder_isr);
}


bool timer_callback(struct repeating_timer *t) {

    // Rotation Speed Calculation
    float rpm = ((float)pulse_count / PPR) * 60.0 / (SAMPLE_TIME_MS / 1000.0);
    float angular_velocity = ((float)pulse_count / PPR) * 2 * 3.1415926 / (SAMPLE_TIME_MS / 1000.0);

    // Output 
    printf("Encoder position: %d, Pulse count: %d\n", encoder_position, pulse_count);
    printf("Speed: %.2f RPM, Angular velocity: %.2f rad/s\n", rpm, angular_velocity);

    // Counter Reset 
    pulse_count = 0;
    //printf("Encoder position (from timer): %d\n", encoder_position);
    return true;  // true means repeat
}



int main() {
    stdio_init_all();
    sleep_ms(3000);  

    encoder_init();  

    
    struct repeating_timer timer;
    //add_repeating_timer_ms(1000, timer_callback, NULL, &timer);
    add_repeating_timer_ms(SAMPLE_TIME_MS, timer_callback, NULL, &timer);


    while (true) {
        /* Debug GPIO 24/23
        gpio_pull_up(ENCODER_B);
        gpio_pull_up(ENCODER_A);
        bool A = gpio_get(ENCODER_A);
        bool B = gpio_get(ENCODER_B);
        printf("A: %d B: %d\n",A,B);
        sleep_ms(1); 
        */
        tight_loop_contents();  
    }
}




