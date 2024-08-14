#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cc_array.h"
#include "cc_hashtable.h"
#include "daq.h"



int loop_read(int key, char *data, size_t data_size, size_t channel_size)
{
    int count = 0;
    while (count < 1000)
    {
        int rc = read(key, data, data_size, channel_size);
        if (rc < 0) {
            printf("Error %d read failed\n", rc);
            return -1;
        }
        printf("------------------------------------------read success:%d------------------------------------------\n", count++);
    }
    return 0;
}

int test_gui()
{

    return 0;
}


int main() {
    // printf("hello world\n");
    // uint32_t data1[8] = {0x01020304, 0x05060708, 0x090a0b0c, 0x0d0e0f10,0x11121314, 0x15161718, 0x191a1b1c, 0x1d1e1f20};
    // for (int i = 0; i < 8; i++) {
    //     printf("%f\n", (float)data1[i]);
    // }   
    // test1(data1, 8);
    // for (int i = 0; i < 8; i++) {
    //     printf("%f\n", (float)data1[i]);
    // }
    // return 0;

    // int rc1 = open_exe("C:/Users/prm/Desktop/cdaq/test/hello.exe");
    // printf("rc:%d\n", rc1);
    // printf("open success\n");
    // return 0;

    int rc = 0;
    
    FILE *fp = fopen("config.txt", "r");
    if (fp == NULL) {
        printf("Failed to open file\n");
        return -1;
    }

    char buf[1024] = {0};
    size_t size;
    while ((size = fread(buf, 1, sizeof(buf), fp)) > 0){
        // fwrite(buf, 1, size, stdout);
    }
    // printf("config:%s\n", buf);
    fclose(fp);

    // char* buf = "192.168.0.159/ai0?ctrl=1&gain=1&iepe=1&coupling=1\n";
    init();
    int key = board_init(buf);
    if (key < 0) {
        printf("Error %d board_init failed\n", key);
        return -1;
    }
    // printf("key:%d\n", key);
    // show_conf();    
    // show_board(key);

    int channel_count = get_channel_count(key);
    printf("channel_count:%d\n", channel_count);

    rc = sample_rate(key, 25000);
    if (rc < 0) {
        printf("Error %d sample_rate failed\n", rc);
        return -1;
    }
    printf("sample_rate success!\n");

    // rc = trig_mode("192.168.0.173", 1);
    // if (rc < 0) {
    //     printf("Error %d trig_mode failed\n", rc);
    //     return -1;
    // }
    // printf("trig_mode success!\n");

    rc = sample_enable(key, 1);
    if (rc < 0) {
        printf("Error %d sample_enable failed\n", rc);
        return -1;
    }
    printf("sample_enable success!\n");
    
    int channel_size = 1000;
    int data_size = channel_size * channel_count;
    float *data = (float *)calloc(sizeof(float),data_size);
    rc = read(key, data, data_size, channel_size);
    if (rc < 0) {
        printf("Error %d read failed\n", rc);
        return -1;
    }
    printf("read success!\n");
    // loop_read(key, data, data_size, channel_size);

    FILE *fp2 = fopen("read_data.txt", "w");
    if (fp2 == NULL) {
        printf("无法打开文件!\n");
        return -1;
    }
    for(int i=0;i<data_size;i++){
        fprintf(fp2, "%f\n", data[i]);
    }

    show_board(key);
    // show_conf();

    rc = board_free(key);
    if (rc < 0){
        printf("Error %d board_free failed\n", rc);
        return -1;
    }
    printf("board_free success!\n");
    rst();
    printf("done!\n");
    return 0;
}