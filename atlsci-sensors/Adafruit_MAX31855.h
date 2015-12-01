#ifndef ADAFRUIT_MAX31855_H
#define ADAFRUIT_MAX31855_H

typedef struct {
    int success;
    char * data;
} adafruit_read_ret_t;

adafruit_read_ret_t adafruit_read(void);

#endif //ADAFRUIT_MAX31855_H
