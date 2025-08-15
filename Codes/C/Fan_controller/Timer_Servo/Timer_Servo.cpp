#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PIN 16

uint16_t wrap = 0;

static inline void wait_for_interrupt(void) {
    __asm volatile ("wfi");
}

void set_servo_angle(uint gpio, float angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    float duty = 0.025f + (angle / 180.0f) * (0.125f - 0.025f);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_gpio_level(gpio, duty * wrap);
}

int main() {
    stdio_init_all();

    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(SERVO_PIN);
    float clkdiv = 64.0f;
    wrap = 125000000 / (50 * clkdiv);  // 50Hz PWM
    pwm_set_clkdiv(slice, clkdiv);
    pwm_set_wrap(slice, wrap);
    pwm_set_enabled(slice, true);

    set_servo_angle(SERVO_PIN, 0);
    sleep_ms(500);

    absolute_time_t start_time = get_absolute_time();

    while (absolute_time_diff_us(start_time, get_absolute_time()) < 10 * 1000 * 1000) {
        tight_loop_contents();
    }

    set_servo_angle(SERVO_PIN, 90);
    sleep_ms(500);

    pwm_set_enabled(slice, false); // 关闭PWM

    while (true) {
        wait_for_interrupt();  // CPU停止，等待中断唤醒
    }
}
