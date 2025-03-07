#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "mcp2515/mcp2515.h"

MCP2515 can0;
struct can_frame rx;

struct can_frame tx_frame;

bool send_can_data(struct repeating_timer *t) {
    // Data need to be sent 
    tx_frame.can_id = 0x123;   // CAN ID
    tx_frame.can_dlc = 4;      // Data Length (8 bytes max)
    tx_frame.data[0] = 0x11;
    tx_frame.data[1] = 0x22;
    tx_frame.data[2] = 0x33;
    tx_frame.data[3] = 0x44;

    // Sending data 
    if (can0.sendMessage(&tx_frame) == MCP2515::ERROR_OK) {
        printf("CAN Message sent!\n");
    } else {
        printf("CAN Message send failed!\n");
    }

    return true;
}

int main()
{
    stdio_init_all();

    // Initialize interface
    can0.reset();
    can0.setBitrate(CAN_100KBPS, MCP_16MHZ);
    can0.setNormalMode();

    // Config Mode 
    can0.setConfigMode();

    // Mask0 & Mask1 All 11-bit need to be same 
    can0.setFilterMask(MCP2515::MASK0, false, 0x7FF);
    can0.setFilterMask(MCP2515::MASK1, false, 0x7FF);

    // Filter（Only ID 0x123）
    can0.setFilter(MCP2515::RXF0, false, 0x123);
    can0.setFilter(MCP2515::RXF1, false, 0x123);
    can0.setFilter(MCP2515::RXF2, false, 0x123);
    can0.setFilter(MCP2515::RXF3, false, 0x123);
    can0.setFilter(MCP2515::RXF4, false, 0x123);
    can0.setFilter(MCP2515::RXF5, false, 0x123);


    // Change to normal mode
    can0.setNormalMode();

    struct repeating_timer timer;
    add_repeating_timer_ms(5000, send_can_data, NULL, &timer);

    // Listen loop
    while (true)
    {
        if (can0.readMessage(&rx) == MCP2515::ERROR_OK)
        {
            printf("New frame from ID: %10x\n", rx.can_id);

            // Print the received data (if any)
            printf("Data: ");
            for (int i = 0; i < rx.can_dlc; i++)
            {
                printf("%02X ", rx.data[i]);
            }
            printf("\n");
        }

    }

    return 0;
}