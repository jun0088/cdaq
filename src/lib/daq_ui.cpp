#include "daq_ui.h"
#include <stdio.h>

int channel_init(Channel *channel) {
    channel->ctrl = false;
    channel->gain = false;
    channel->iepe = false;
    channel->coupling = false;
    channel->calenable = false;
    return 0;
}

int board_init(Board *board) {
    for (int i = 0; i < 7; i++) {
        channel_init(board->chs + i);
    }
    return 0;
}


