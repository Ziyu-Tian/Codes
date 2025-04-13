/*
RWC 2025 Speed Control System Test-1 - Reading Speed from encoder via GPIO directly to Pico
PPR captured with 1200 as expected
*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define ENCODER_A 4    // GPIO Pin for Encoder A
#define ENCODER_B 5   // GPIO Pin for Encoder B
#define PPR 1200 // Pulse per Revolution for encoder E38S6G5-600B-G24N
#define SAMPLE_TIME_MS 100  // Sampling Time
#define SHAFT_DIAMETER 6 // Shaft Diameter of encoder (mm)

volatile int pulse_count = 0;  // Pulses Number in sampling time 
volatile int encoder_position = 0;  // Encoder position, volatile to prevent optimization
volatile bool last_A = 0;  // Last A state for direction detection


void encoder_isr(uint gpio, uint32_t events) {
    bool A = gpio_get(ENCODER_A);
    bool B = gpio_get(ENCODER_B);

    //printf("Interrupt triggered: A = %d, B = %d\n", A, B);
    if (A != last_A) {
        encoder_position += (A ^ B) ? 1 : -1; // Direction detect (CW:+, ACW:-)
        pulse_count++; // Pulses Counter
        last_A = A;  
    //printf("Encoder position: %d\n", encoder_position);
    }
}


void encoder_init() { // Interrupt triggered by Pulse 
    gpio_init(ENCODER_A);
    gpio_set_dir(ENCODER_A, GPIO_IN);
    gpio_pull_up(ENCODER_A);
    gpio_set_irq_enabled_with_callback(ENCODER_A, GPIO_IRQ_EDGE_RISE, true, &encoder_isr);
    gpio_set_irq_enabled_with_callback(ENCODER_A, GPIO_IRQ_EDGE_FALL, true, &encoder_isr);
    // Double Frequency with edge-detection in both edges of A
    // Real PPR = 600 x 2 = 1200
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
    printf("Encoder position: %d, Pulse count: %d, RPM: %.2f\n", encoder_position, pulse_count, rpm);


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




