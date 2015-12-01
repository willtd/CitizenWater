//Initialize sensors. Require root privileges

#include "mraa.h"

int main(void)
{
    mraa_i2c_init_raw(6);
}
