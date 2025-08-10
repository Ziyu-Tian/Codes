/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <time.h>
#include "lwip/apps/sntp.h"

// static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
//     if (result) {
//         printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
//             result->ssid, result->rssi, result->channel,
//             result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
//             result->auth_mode);
//     }
//     return 0;
// }

#include "hardware/vreg.h"
#include "hardware/clocks.h"
#include "wifi_config.h"

// int main() {
//     stdio_init_all();

//     if (cyw43_arch_init()) {
//         printf("failed to initialise\n");
//         return 1;
//     }

//     cyw43_arch_enable_sta_mode();

//     absolute_time_t scan_time = nil_time;
//     bool scan_in_progress = false;
//     while(true) {
//         if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0) {
//             if (!scan_in_progress) {
//                 cyw43_wifi_scan_options_t scan_options = {0};
//                 int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
//                 if (err == 0) {
//                     printf("\nPerforming wifi scan\n");
//                     scan_in_progress = true;
//                 } else {
//                     printf("Failed to start scan: %d\n", err);
//                     scan_time = make_timeout_time_ms(10000); // wait 10s and scan again
//                 }
//             } else if (!cyw43_wifi_scan_active(&cyw43_state)) {
//                 scan_time = make_timeout_time_ms(10000); // wait 10s and scan again
//                 scan_in_progress = false; 
//             }
//         }
//         // the following #ifdef is only here so this same example can be used in multiple modes;
//         // you do not need it in your code
// #if PICO_CYW43_ARCH_POLL
//         // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
//         // main loop (not from a timer) to check for Wi-Fi driver or lwIP work that needs to be done.
//         cyw43_arch_poll();
//         // you can poll as often as you like, however if you have nothing else to do you can
//         // choose to sleep until either a specified time, or cyw43_arch_poll() has work to do:
//         cyw43_arch_wait_for_work_until(scan_time);
// #else
//         // if you are not using pico_cyw43_arch_poll, then WiFI driver and lwIP work
//         // is done via interrupt in the background. This sleep is just an example of some (blocking)
//         // work you might be doing.
//         sleep_ms(1000);
// #endif
//     }

//     cyw43_arch_deinit();
//     return 0;
// }

 // 自动连接 pool.ntp.org
void init_sntp(void) {
    sntp_init();  // 或包含你想写的初始化操作
}
int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD,
        CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Wi-Fi connect failed\n");
        return -1;
    }

    printf("Wi-Fi connected!\n");

    init_sntp();

    // 等待 NTP 同步完成
    while (time(NULL) < 100000) {
        printf("Waiting for NTP sync...\n");
        sleep_ms(1000);
    }

    printf("NTP sync complete.\n");

    while (true) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        printf("Current time: %02d:%02d:%02d\n",
               tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);

        sleep_ms(30000);  // 每 30 秒输出一次
    }

    return 0;
}