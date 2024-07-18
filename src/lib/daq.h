#ifndef STREAM_H
#define STREAM_H
#include "zmq.h"

// Macro to detect endianness
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define IS_BIG_ENDIAN 1
#else
#define IS_BIG_ENDIAN 0
#endif


typedef enum {
    INTERNAL = 0,
    EXTERNAL = 1
} TrigMode;

typedef struct Channel Channel;

typedef struct Channel {
    int id;
    void *socket;
    uint8_t *stream_id;
    size_t stream_id_size;
    uint8_t *buffer;
    size_t buffer_size;
    size_t read_count;
    int ctrl;
    int gain;
    int iepe;
    int coupling;
    // 0:internal, 1:external
    int trig_mode;
    int cal_enable;
    // int primary;
    int isconnected;
    float gain_value;
};

extern __declspec(dllexport) int open_exe(char *file);
extern __declspec(dllexport) int lock();
extern __declspec(dllexport) int init();
extern __declspec(dllexport) int rst();
extern __declspec(dllexport) int board_init(const char *config); // return is key
extern __declspec(dllexport) int board_free(int key);
extern __declspec(dllexport) int get_channel_count(int key);
extern __declspec(dllexport) int sample_rate(int key, int sample_rate);
extern __declspec(dllexport) int trig_mode(const char *ip, int internal);
extern __declspec(dllexport) int sample_enable(int key, int enable);
extern __declspec(dllexport) int read(int key, void *data, int data_size, int channel_size);

int test1(uint32_t *data, int size);
int show_board(int key);
int show_conf();
#endif 