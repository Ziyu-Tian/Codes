#include "QuadratureEncoder.hpp"
#include <stdio.h>

QuadratureEncoder::QuadratureEncoder(PIO pio, uint8_t pin, float ppr, float gear_ratio) 
    : pio_instance(pio), ppr(ppr * 1.0f), gear_ratio(gear_ratio) 
{
    // Apply a state machine 
    sm = pio_claim_unused_sm(pio_instance, true);
    
    // Ensure the sm applied successfully 
    if (sm == -1) {
        printf("Error: No free state machine found for pin %d!\n", pin);
        return;
    }

    // Choose correct pio_loaded
    static bool pio_program_loaded[2] = {false, false};
    static uint offset[2];

    uint pio_idx = pio_get_index(pio_instance);
    if (!pio_program_loaded[pio_idx]) {
        offset[pio_idx] = pio_add_program(pio_instance, &quadrature_encoder_program);
        pio_program_loaded[pio_idx] = true;
    }

    //Initialize PIO
    quadrature_encoder_program_init(pio_instance, sm, pin, offset[pio_idx]);
    printf("Encoder pin %d assigned to PIO%d, SM%d\n", pin, pio_idx, sm);
}


void QuadratureEncoder::update(float dt)
{
    _last_count = _count;
    _count = quadrature_encoder_get_count(pio_instance, sm);
    _dt = dt;
}

int32_t QuadratureEncoder::get_count()
{
    return _count;
}

float QuadratureEncoder::get_position(bool degrees)
{
    if (degrees)
        return 360.0f * _count / (ppr * gear_ratio); // deg
    else
        return 2 * M_PI * _count / (ppr * gear_ratio); // rad
}

float QuadratureEncoder::get_velocity(bool degrees)
{
    float velocity = ((_count - _last_count) / (ppr * gear_ratio)) / _dt;

    if (degrees)
        return 360.0f * velocity; // deg/s
    else
        return 2 * M_PI * velocity; // rad/s
}

bool QuadratureEncoder::get_direction()
{
    return _count > _last_count ? 1 : -1;
}

int8_t QuadratureEncoder::get_sign()
{
    return (_count > 0) - (_count < 0);
}
