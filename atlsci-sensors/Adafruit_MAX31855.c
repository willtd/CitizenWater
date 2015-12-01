#include "Adafruit_MAX31855.h"
#include "mraa.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void adafruit_parce_data(uint16_t* data, adafruit_read_ret_t* ret);
bool adafruit_data_has_error(uint16_t* data);
void adafruit_get_error(uint16_t* data, adafruit_read_ret_t* ret);
void adafruit_get_temp(uint16_t* data, adafruit_read_ret_t* ret);

adafruit_read_ret_t adafruit_read(void)
{
	adafruit_read_ret_t ret;
	ret.success = 0;
	ret.data = NULL;

    mraa_spi_context spiMAX31855;
    uint16_t* dataMAX31855;

    spiMAX31855 = mraa_spi_init(2);
    if (spiMAX31855 == NULL) {
    	strcpy(ret.data,"Initialization of spi failed");
    	return ret;
    }
    mraa_spi_frequency(spiMAX31855, 5000000);
    mraa_spi_lsbmode(spiMAX31855, MRAA_SPI_MODE0);

    // The MAX7219/21 Chip needs the data in word size
    if (mraa_spi_bit_per_word(spiMAX31855, 16) != MRAA_SUCCESS) {
    	strcpy(ret.data,"Could not set SPI Device to 16Bit mode");
    	return ret;
    };

    dataMAX31855 = mraa_spi_write_buf_word(spiMAX31855, NULL, 2);


    if(dataMAX31855 != NULL)
    {
    	adafruit_parce_data(dataMAX31855, &ret);
    }

    mraa_spi_stop(spiMAX31855);

	return ret;
}

void adafruit_parce_data(uint16_t* data,adafruit_read_ret_t* ret)
{
	if(adafruit_data_has_error(data))
	{
		adafruit_get_error(data, ret);
	}
	else
	{
		adafruit_get_temp(data, ret);
	}
}

bool adafruit_data_has_error(uint16_t* data)
{
	if(data == NULL) return true;
	return data[0] & 0x1;
}

void adafruit_get_error(uint16_t* data, adafruit_read_ret_t* ret)
{
	ret->success = 0;
	ret->data = malloc(100);

	if(data == NULL)
	{
		strcpy(ret->data, "data is NULL");
	}
	else
	{
		strcpy(ret->data, "thermocouple fault:");
		if(data[1] & 0x1)
		{
			strcat(ret->data," Open Circuit ");
		}

		if (data[1] & 0x2)
		{
			strcat(ret->data," Short to GND ");
		}

		if (data[1] & 0x4)
		{
			strcat(ret->data," Short to VCC ");
		}
	}
}


void adafruit_get_temp(uint16_t* data, adafruit_read_ret_t* ret)
{
	int16_t temp;
	ret->success = true;
	ret->data = malloc(32);

	if(data[0] & 0x8000)//negative
	{
		temp = 0xC000 | ((data[0] >> 2) & 0x3FFF);
	}
	else
	{
		temp = data[0] >> 2;
	}
	sprintf(ret->data,"%d", temp);
}
