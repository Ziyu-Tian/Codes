/*
GPIO directly connection via PIO
PPR captured with 2x frequency, measured as 1200 correctly
*/
#include <QuadratureEncoder.hpp>
#include "pico/stdlib.h"

int main() {
    
    stdio_init_all();

    // Pin_A (Pin_B would be set as A_pin + 1)
    uint8_t A_pin = 23; // Noted that A and B should be swapped in physical connection
    float ppr = 600.0; // 

    // Initialize PIO_Encoder Class
    QuadratureEncoder encoder(A_pin, ppr);

    // Sampling Time for Speed Calculation
    float sampling_time = 1e-3;

    while (true) {
        
        encoder.update(sampling_time); // Initialize starting time
            
        // Position: return angle (default radium)
        // Velocity: return velocity in rad/s
        // Counter: return counting number (CW++, ACW--, 1200 per round)
        auto position = encoder.get_position();
        auto velocity = encoder.get_velocity();
        auto counter = encoder.get_count();

        // 
        printf("Position: %f, Velocity: %f\n, Counter: %d\n", position, velocity, counter);

        // 
        sleep_ms(1000);
    }

    return 0;
}

