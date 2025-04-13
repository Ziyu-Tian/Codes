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


int main() {
    
    stdio_init_all();

    // Pin_A (Pin_B would be set as A_pin + 1)
    uint8_t A_pin  = 4; // Noted that A and B should be swapped in physical connection
    uint8_t A2_pin = 20;
    uint8_t A3_pin = 16;
    float ppr = 600.0; 

    // Initialize PIO_Encoder Class
    QuadratureEncoder encoder_1(pio0,A_pin, ppr);
    QuadratureEncoder encoder_2(pio1,A2_pin, ppr);
    QuadratureEncoder encoder_3(pio2,A3_pin, ppr);
    // Sampling Time for Speed Calculation
    float sampling_time = 500e-3;

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


        // printf("Velocity_1: %.2f RPM , Counter_1: %d ;", rpm_1, counter);
        // printf("Velocity_2: %.2f RPM , Counter_2: %d\n", rpm_2, counter_2);
        printf("RPM_1: %.2f, ",rpm_1);
        printf("RPM_2: %.2f, ",rpm_2);
        printf("RPM_3: %.2f\n",rpm_3);
        sleep_ms(500);
    }

    return 0;

}