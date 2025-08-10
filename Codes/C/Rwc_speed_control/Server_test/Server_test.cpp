#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PIN 16 // 使用 GPIO16

void set_servo_angle(uint gpio, float angle)
{
    // 限制角度在 0-180
    if (angle < 0)
        angle = 0;
    if (angle > 180)
        angle = 180;

    // 将角度映射到脉宽 (0.5ms~2.5ms)，以 50Hz 为例周期为 20ms => 20000us
    // 占空比范围对应为 2.5% 到 12.5%
    uint slice = pwm_gpio_to_slice_num(gpio);
    float min_duty = 0.025f;
    float max_duty = 0.125f;
    float duty = min_duty + (angle / 180.0f) * (max_duty - min_duty);

    pwm_set_gpio_level(gpio, duty * 65535);
}

int main()
{
    stdio_init_all();

    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(SERVO_PIN);

    // 设置 50Hz PWM: wrap = clk_sys / (freq * divider)
    uint32_t freq = 50;
    uint32_t clk_sys = 125000000;
    float divider = 64.0f; // 任意可用值
    uint32_t wrap = clk_sys / (freq * divider);

    pwm_set_wrap(slice, wrap);
    pwm_set_clkdiv(slice, divider);
    pwm_set_enabled(slice, true);

    while (1)
    {
        // 0°, 90°, 180°
        set_servo_angle(SERVO_PIN, 0);
        sleep_ms(1000);
        absolute_time_t start_time = get_absolute_time();

        // 等待2400秒
        while (absolute_time_diff_us(start_time, get_absolute_time()) < 2400LL * 1000 * 1000)
        {
            tight_loop_contents();
        }
        set_servo_angle(SERVO_PIN, 120);
        sleep_ms(5000);
        // set_servo_angle(SERVO_PIN, 180);
        // sleep_ms(1000);
        set_servo_angle(SERVO_PIN, 10);
            while (true) {
       tight_loop_contents();
    }
    }
}
