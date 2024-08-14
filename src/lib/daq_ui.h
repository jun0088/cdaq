#ifndef UI_H
#define UI_H

#include <stdint.h>
#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

typedef struct Channel Channel;

typedef struct Channel {
    bool ctrl;
    bool gain;
    bool iepe;
    bool coupling;
    bool calenable;
};

typedef struct Board Board;

typedef struct Board {
    Channel chs[7];
};

typedef enum {
    CTRL = 1,
    GAIN = 2,
    IEPE = 3,
    COUPLING = 4,
    CALENABLE = 5
} ChannelIndex;

int board_init(Board *board);
#endif

