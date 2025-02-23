/*
GPIO Directly Connection with PIO - 4x frequency 
PPR measured as 2400 correctly
*/
#include <QuadratureEncoder.hpp>
#include "pico/stdlib.h"

int main() {
    
    stdio_init_all();

    // 
    uint8_t A_pin = 23; // 
    float ppr = 2400.0; // 

    // 
    QuadratureEncoder encoder(A_pin, ppr);

    // 
    float sampling_time = 1e-3;

    while (true) {
        // 
        encoder.update(sampling_time);

        // 
        auto position = encoder.get_position();
        auto velocity = encoder.get_velocity();
        auto counter = encoder.get_count();

        // 
        // 
        printf("Position: %f, Velocity: %f\n, Counter: %d\n", position, velocity, counter);

        // 
        sleep_ms(1000);
    }

    return 0;
}
