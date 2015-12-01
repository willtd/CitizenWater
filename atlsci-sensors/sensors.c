/*
 *  Implementation of sending commands to AtlasScientific sensors.
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mraa.h"

#include "../shared/log.h"
#include "../shared/macros.h"

#include "sensors.h"

#define ATLSCI_MIN           			0
#define ATLSCI_MAX           			3

// default I2C addresses
#define I2C_ADDR_EZO_DO 			0x61
#define I2C_ADDR_EZO_ORP 			0x62
#define I2C_ADDR_EZO_PH				0x63
#define I2C_ADDR_EZO_CONDUCTIVITY 		0x64

//Calibration buffer sizes and offset pointer values to insert calibrate value at last index
//Only need for PH, Conductivity, and ORP. DO does not require calibrate value.
//Buffer size need to be specific in order to use 'sprintf' to insert specific format
//(float, int, char ascii, etc) to the buffer

#define PH_CAL_LOW_SIZE				12
#define PH_CAL_MID_SIZE  			12
#define PH_CAL_HIGH_SIZE 			14
#define PH_INDEX_LOW 				8
#define PH_INDEX_MID				8
#define PH_INDEX_HIGH				10

#define CONDUCTIVITY_CAL_DRY_SIZE		7
#define CONDUCTIVITY_CAL_ONE_SIZE		9
#define CONDUCTIVITY_CAL_LOW_SIZE		9
#define CONDUCTIVITY_CAL_HIGH_SIZE		10
#define CONDUCTIVITY_INDEX_ONE 			8
#define CONDUCTIVITY_INDEX_LOW			8
#define CONDUCTIVITY_INDEX_HIGH			9

#define QUERY_SIZE                   7
#define ORP_CAL_SIZE				7
#define ORP_INDEX				4

#define	TEMP_COMP_SIZE				5
#define TEMP_COMP_INDEX				3

// For atlsci sensors, a 300ms wait is typically needed between sending a command
// and reading a response. Set to 2 seconds because millisecond resolution is difficult
// to achieve.
#define TX_WAIT_TIME				2

#define ATLSCI_SUCCESS 				0
#define ATLSCI_FAILURE 				-1
#define RETURN_SUCCESS 				0
#define ATLSCI_SUCCESS_BYTE_INDICATOR 		1

//Initialize global variables for system configuration
int I2C_ADDR[4] = {I2C_ADDR_EZO_DO,
                         I2C_ADDR_EZO_ORP,
                         I2C_ADDR_EZO_PH,
                         I2C_ADDR_EZO_CONDUCTIVITY};

char * SENSOR_NAME[4] = {"Dissolved Oxygen",
                               "Oxygen Reduction Potential",
                               "pH",
                               "Conductivity"};

mraa_i2c_context i2c_context = NULL;

char * cal_clear = "Cal,clear";
char * cal_query = "Cal,?";

static int set_address(mraa_i2c_context i2c, int addr);
static int get_i2c_addr(int sensor_type);
static int set_sensor_address(mraa_i2c_context i2c_context, int sensor_type);
static atlsci_cal_ret_t atlsci_cal_ph(atlsci_cal_param_t param, float value);
static atlsci_cal_ret_t atlsci_cal_conductivity(atlsci_cal_param_t param, int value);
static atlsci_cal_ret_t atlsci_cal_dissolved_oxygen(atlsci_cal_param_t param);
static atlsci_cal_ret_t atlsci_cal_orp(atlsci_cal_param_t param, int value);
static atlsci_temp_comp_ret_t atlsci_temp_comp_fail(atlsci_temp_comp_param param);
static atlsci_cal_ret_t atlsci_cal_fail(atlsci_cal_param_t param);
static atlsci_device_info_ret_t parse_atlsci_device_info(uint8_t * device_info_string);
static atlsci_led_ret_t parse_atlsci_led(uint8_t * led_string, int action);
static atlsci_read_ret_t parse_atlsci_read(int sensor_type, uint8_t * led_string);
static atlsci_reset_ret_t parse_atlsci_reset(int success);
static atlsci_sleep_ret_t parse_atlsci_sleep(int success);
static atlsci_status_ret_t parse_atlsci_status(uint8_t * status_string, int success);

//////////////////////////////////////////////////////////////////////////
///////////// Implementation of helper functions below ///////////////////
//////////////////////////////////////////////////////////////////////////

/**
 *Set the i2c bus and address of where the data will be written to.
 *
 *@param i2c context, pass in the i2c context that represents
 *a master on the i2c bus.
 *@param addr The address of the I2C device
 *
*/
static int set_address(mraa_i2c_context i2c, int addr) {
	mraa_result_t mraa_result = mraa_i2c_address(i2c, addr);

    if (mraa_result != MRAA_SUCCESS)
    {
        DEBUG("set_address(): mraa_i2c_address() failure, error code: %d\n", mraa_result);
        mraa_result_print(mraa_result);
    }

    return mraa_result;
}

/**
 *  Returns the I2C address of the sensor.
 */
static int get_i2c_addr(int sensor_type)
{
    if (sensor_type < ATLSCI_MIN || sensor_type > ATLSCI_MAX)
        return ATLSCI_FAILURE;
    else
        return I2C_ADDR[sensor_type];
}

/**
 *  Sets the I2C address of the given sensor.
 */
static int set_sensor_address(mraa_i2c_context i2c_context, int sensor_type)
{
     return set_address(i2c_context, get_i2c_addr(sensor_type));
}

/*
 *  Transmits a string. msg should be null-terminating, but the null byte
 *  is not transmitted. This function should be called after calling
 *  set_sensor_address(). Delays for TX_WAIT_TIME seconds after sending message.
 */
static int send_string(mraa_i2c_context i2c_context, char * msg)
{
    DEBUG("Sending string %s of length %d\n", msg, strlen(msg));

    int ret = mraa_i2c_write(i2c_context, (uint8_t *) msg, strlen(msg));
    sleep(TX_WAIT_TIME);
    return ret;
}

const char * mraa_result_to_string(int res)
{
    switch (res)
    {
        case MRAA_SUCCESS:                       return "Expected response";
        case MRAA_ERROR_FEATURE_NOT_IMPLEMENTED: return "Feature TODO";
        case MRAA_ERROR_FEATURE_NOT_SUPPORTED:   return "Feature not supported by HW";
        case MRAA_ERROR_INVALID_VERBOSITY_LEVEL: return "Verbosity level wrong";
        case MRAA_ERROR_INVALID_PARAMETER:       return "Parameter invalid";
        case MRAA_ERROR_INVALID_HANDLE:          return "Handle invalid";
        case MRAA_ERROR_NO_RESOURCES:            return "No resource of that type avail";
        case MRAA_ERROR_INVALID_RESOURCE:        return "Resource invalid";
        case MRAA_ERROR_INVALID_QUEUE_TYPE:      return "Queue type incorrect";
        case MRAA_ERROR_NO_DATA_AVAILABLE:       return "No data available";
        case MRAA_ERROR_INVALID_PLATFORM:        return "Platform not recognised";
        case MRAA_ERROR_PLATFORM_NOT_INITIALISED: return "Board information not initialised";
        case MRAA_ERROR_PLATFORM_ALREADY_INITIALISED: return "Board is already initialised";
        case MRAA_ERROR_UNSPECIFIED:             return "Unknown Error";
        default:                                 return "???";
    }
}

void log_mraa_response(char * caller, int res)
{
    DEBUG("%s: %s", caller, mraa_result_to_string(res));
}

char * get_sensor_type_as_string(int sensor_type)
{
    if (sensor_type < ATLSCI_MIN || sensor_type > ATLSCI_MAX)
        return "Invalid";
    else
        return SENSOR_NAME[sensor_type];
}

//////////////////////////////////////////////////////////////////////////
///////////// Implementation of sensor commands below ////////////////////
//////////////////////////////////////////////////////////////////////////

static atlsci_cal_ret_t atlsci_cal_ph(atlsci_cal_param_t param, float value)
{
    atlsci_cal_ret_t ret;
	int success = set_sensor_address(i2c_context, ATLSCI_PH);
    uint8_t rx_buffer[STRING_SIZE] = {0};
    char cal_low[PH_CAL_LOW_SIZE] = "Cal,low,";
    char cal_mid[PH_CAL_MID_SIZE] = "Cal,mid,";
    char cal_high[PH_CAL_HIGH_SIZE] = "Cal,high";
    char *pt_cal = NULL;

    switch(param.cal_ph)
    {
    	case PH_CALIBRATION_CLEAR:		success = send_string(i2c_context, cal_clear); break;

    	case PH_CALIBRATION_QUERY:		success = send_string(i2c_context, cal_query); break;

    	case PH_LOW_CALIBRATION_POINT:  	pt_cal = cal_low;
    						sprintf(pt_cal + PH_INDEX_LOW, "%.2f", value);
    						success = send_string(i2c_context, cal_low); break;

    	case PH_MID_CALIBRATION_POINT:		pt_cal = cal_mid;
    						sprintf((pt_cal + PH_INDEX_MID), "%.2f", value);
    						success = send_string(i2c_context, cal_mid); break;

    	case PH_HIGH_CALIBRATION_POINT: 	pt_cal = cal_high;
    						sprintf((pt_cal + PH_INDEX_HIGH), "%.2f", value);
    						success = send_string(i2c_context, cal_high); break;
        
        default:         			return atlsci_cal_fail(param);
    }

    if (success == MRAA_SUCCESS)
    {
        success = mraa_i2c_read(i2c_context, rx_buffer, sizeof(rx_buffer));
    	if(param.cal_ph == PH_CALIBRATION_QUERY)
        {
            ret.query = malloc(QUERY_SIZE);
            //ret.query = rx_buffer;
            uint8_t * rx_ptr = rx_buffer; rx_ptr++;
            sprintf(ret.query, "%s", rx_ptr);
        }
        //First bit of rx_buffer will be '1' if write is successful
    	ret.success = (int)rx_buffer[0];
    }
    else
    {
    	log_mraa_response("atlsci_cal_ph()", success);
    	ret.success = -1;
    }
	return ret;
}

static atlsci_cal_ret_t atlsci_cal_conductivity(atlsci_cal_param_t param, int value)
{
	atlsci_cal_ret_t ret;
	int success = set_sensor_address(i2c_context, ATLSCI_CONDUCTIVITY);
	uint8_t rx_buffer[STRING_SIZE] = {0};
	char cal_dry[CONDUCTIVITY_CAL_DRY_SIZE] = "Cal,dry";
	char cal_one[CONDUCTIVITY_CAL_ONE_SIZE] = "Cal,one,";
	char cal_low[CONDUCTIVITY_CAL_LOW_SIZE] = "Cal,low,";
	char cal_high[CONDUCTIVITY_CAL_HIGH_SIZE] = "Cal,high,";
	char *pt_cal = NULL;

	switch(param.cal_conductivity)
	{
		case CONDUCTIVITY_CALIBRATION_CLEAR:			success = send_string(i2c_context, cal_clear); break;

		case CONDUCTIVITY_CALIBRATION_QUERY:			success = send_string(i2c_context, cal_query); break;

		case CONDUCTIVITY_DRY_CALIBRATION:			success = send_string(i2c_context, cal_dry); break;

		case CONDUCTIVITY_SINGLE_CALIBRATION_POINT:		pt_cal = cal_one;
									sprintf((pt_cal + CONDUCTIVITY_INDEX_ONE), "%d", value); 	//set value to end of string
									success = send_string(i2c_context, cal_one); break;

		case CONDUCTIVITY_LOW_DUAL_CALIBRATION_POINT:   	pt_cal = cal_low;
									sprintf((pt_cal + CONDUCTIVITY_INDEX_LOW), "%d", value);
									success = send_string(i2c_context, cal_low); break;

		case CONDUCTIVITY_HIGH_DUAL_CALIBRATION_POINT:  	pt_cal = cal_high;
									sprintf((pt_cal + CONDUCTIVITY_INDEX_HIGH), "%d", value);
									success = send_string(i2c_context, cal_high); break;
        	
        	default:         					return atlsci_cal_fail(param);
	}
    if (success == MRAA_SUCCESS)
    {
        success = mraa_i2c_read(i2c_context, rx_buffer, sizeof(rx_buffer));
    	if(param.cal_conductivity == CONDUCTIVITY_CALIBRATION_QUERY)
        {
            ret.query = malloc(QUERY_SIZE);
            uint8_t * rx_ptr = rx_buffer; rx_ptr++;
            sprintf(ret.query, "%s", rx_ptr);
            //ret.query = rx_buffer;
        }
    		//printf("%s\n", rx_buffer);
        //First bit of rx_buffer will be '1' if write is successful
    	ret.success = (int)rx_buffer[0];
    }
    else
    {
    	log_mraa_response("atlsci_cal_conductivity()", success);
    	ret.success = -1;
    }
	return ret;
}

static atlsci_cal_ret_t atlsci_cal_dissolved_oxygen(atlsci_cal_param_t param)
{
	atlsci_cal_ret_t ret;
	int success = set_sensor_address(i2c_context, ATLSCI_DO);
	uint8_t rx_buffer[STRING_SIZE] = {0};
	char * cal_atmospheric_oxygen_level = "Cal";
	char * cal_0 = "Cal,0";

	switch(param.cal_do)
	{
		case DO_CALIBRATION_CLEAR:			success = send_string(i2c_context, cal_clear); break;
		case DO_CALIBRATION_QUERY:			success = send_string(i2c_context, cal_query); break;
		case DO_ATMOSPHERIC_CALIBRATION:		success = send_string(i2c_context, cal_atmospheric_oxygen_level); break;
		case DO_ZERO_LEVEL_CALIBRATION:			success = send_string(i2c_context, cal_0); break;
        	
        	default:         return atlsci_cal_fail(param);
	}

    if (success == MRAA_SUCCESS)
    {
        success = mraa_i2c_read(i2c_context, rx_buffer, sizeof(rx_buffer));
    	if(param.cal_do == DO_CALIBRATION_QUERY)
        {
            ret.query = malloc(QUERY_SIZE);
            uint8_t * rx_ptr = rx_buffer; rx_ptr++;
            sprintf(ret.query, "%s", rx_ptr);
            //ret.query = (char *)rx_buffer;
            //ret.query = rx_buffer;
        }
    		//printf("%s\n", rx_buffer);
        //First bit of rx_buffer will be '1' if write is successful
    	ret.success = (int)rx_buffer[0];
    }
    else
    {
    	log_mraa_response("atlsci_cal_do()", success);
    	ret.success = -1;
    }
	return ret;
}

static atlsci_cal_ret_t atlsci_cal_orp(atlsci_cal_param_t param, int value)
{
	atlsci_cal_ret_t ret;
	uint8_t rx_buffer[STRING_SIZE] = {0};
	char cal_single[ORP_CAL_SIZE] = "Cal,";
	int success = set_sensor_address(i2c_context, ATLSCI_ORP);
	char *pt_cal = cal_single;

	switch(param.cal_orp)
	{
		case ORP_CALIBRATION_CLEAR:			success = send_string(i2c_context, cal_clear); break;

		case ORP_CALIBRATION_QUERY:			success = send_string(i2c_context, cal_query); break;

		case ORP_SINGLE_CALIBRATION_POINT:		sprintf((pt_cal + ORP_INDEX), "%d", value);
								success = send_string(i2c_context, cal_single); break;

        	default:         return atlsci_cal_fail(param);
	}

    if (success == MRAA_SUCCESS)
    {
        success = mraa_i2c_read(i2c_context, rx_buffer, sizeof(rx_buffer));
    	if(param.cal_orp == ORP_CALIBRATION_QUERY)
        {
            ret.query = malloc(QUERY_SIZE);
            uint8_t * rx_ptr = rx_buffer; rx_ptr++;
            sprintf(ret.query, "%s", rx_ptr);
            //ret.query = rx_buffer;
        }
    		//printf("%s\n", rx_buffer);

        //First bit of rx_buffer will be '1' if write is successful
    	ret.success = (int)rx_buffer[0];
    }
    else
    {
    	log_mraa_response("atlsci_cal_orp()", success);
    	ret.success = -1;
    }
	return ret;
}

static atlsci_temp_comp_ret_t atlsci_temp_comp_fail(atlsci_temp_comp_param param)
{
	atlsci_temp_comp_ret_t ret;
	log_mraa_response("atlsci_temp_comp_fail()", MRAA_ERROR_INVALID_PARAMETER);
	ret.success = -1;
	return ret;
}

/*
 * Temperature compensation is only enable for pH, DO, and Conductivity sensor.
 * ORP does not have this feature. The value that it takes is expressed in float value nn.n
 */
atlsci_temp_comp_ret_t atlsci_temp_comp(int sensor_type, atlsci_temp_comp_param param, float value)
{
	atlsci_temp_comp_ret_t ret;
	uint8_t rx_buffer[STRING_SIZE] = {0};
	char cal_temp[TEMP_COMP_SIZE] = "T,";
	int success = set_sensor_address(i2c_context, sensor_type);
	char *pt_cal = cal_temp;

	switch(param)
	{
		case SET_TEMPERATURE:		sprintf(pt_cal + TEMP_COMP_INDEX, "%.1f", value);
						success = send_string(i2c_context, cal_temp); break;
						
		case QUERY_TEMPERATURE:		success = send_string (i2c_context, cal_query); break;
		
		default:			return atlsci_temp_comp_fail(param);
	}
    if (success == MRAA_SUCCESS)
    {
        success = mraa_i2c_read(i2c_context, rx_buffer, sizeof(rx_buffer));
    	if(param == QUERY_TEMPERATURE)
    		printf("%s\n", rx_buffer);
        //First bit of rx_buffer will be '1' if write is successful
    	ret.success = (int)rx_buffer[0];
    }
    else
    {
    	log_mraa_response("atlsci_temp_comp()", success);
    	ret.success = -1;
    }
	return ret;
}

static atlsci_cal_ret_t atlsci_cal_fail(atlsci_cal_param_t param)
{
	atlsci_cal_ret_t ret;
	log_mraa_response("atlsci_cal_fail()", MRAA_ERROR_INVALID_PARAMETER);
	ret.success = -1;
	return ret;
}

atlsci_cal_ret_t atlsci_cal(int sensor_type, atlsci_cal_param_t param, float value)
{
    switch (sensor_type)
    {
        case ATLSCI_DO:  return atlsci_cal_dissolved_oxygen(param);
        case ATLSCI_ORP: return atlsci_cal_orp(param, (int)value);
        case ATLSCI_PH:  return atlsci_cal_ph(param, value);
        case ATLSCI_CONDUCTIVITY: return atlsci_cal_conductivity(param, (int)value);
        default:         return atlsci_cal_fail(param);
    }
}

/**
 * Return message format:
 *    1 ? I , P H ; 1 . 0 null
 *      where pH = device type. This can also be EC, DO, or OR(P)
 *      and   1.0 = firmware version number
 */
static atlsci_device_info_ret_t parse_atlsci_device_info(uint8_t * device_info_string)
{
    atlsci_device_info_ret_t ret;
    ret.success = (int)device_info_string[0];
    int offset_device_type = 4;
    int offset_firmware = 3;
    int device_type_chars = 2;
    int firmware_chars = 4;

    ret.device_type = malloc(device_type_chars);
    ret.firmware_version = malloc(firmware_chars);

    //point to the first char of device type
    uint8_t *pt_device_str = device_info_string + offset_device_type;

    memcpy(ret.device_type, pt_device_str, device_type_chars);
    ret.device_type[device_type_chars] = '\0';

    memcpy(ret.firmware_version, pt_device_str+offset_firmware, firmware_chars);

    return ret;
}

atlsci_device_info_ret_t atlsci_device_info(int sensor_type)
{
    int ret = -1;
    uint8_t rx_buffer[STRING_SIZE] = {0};

    ret = set_sensor_address(i2c_context, sensor_type);

    if (ret == MRAA_SUCCESS)
    {
        ret = send_string(i2c_context, "I");

        if (ret == MRAA_SUCCESS)
        {
            ret = mraa_i2c_read(i2c_context, rx_buffer, sizeof(rx_buffer));
        }
        else
        {
        	log_mraa_response("atlsci_device_info()", ret);
        }
    }

    else
    {
        log_mraa_response("atlsci_device_info()", ret);
    }

    return parse_atlsci_device_info(rx_buffer);
}

/**
 * Return message format: Descriptions of actions:
 *    ATLSCI_LED_ON: 1 null
 *      indicates the command was successfully processed.
 *    ATLSCI_LED_OFF: 1 null
 *      indicates the command was successfully processed.
 *    ATLSCI_LED_QUERY: 1 ? L , 1 null
 *      where the last characters are "?L,1" if the LED is enabled and
 *      "?L,0" if the LED is disabled.
 */
static atlsci_led_ret_t parse_atlsci_led(uint8_t * led_string, int action)
{
    atlsci_led_ret_t ret;

    ret.success = (int)led_string[0];
    if(action == ATLSCI_LED_QUERY)
    	ret.led_state = (int)led_string[4] - '0';	//convert char to int
    else
    	ret.led_state = 0;

    return ret;
}

atlsci_led_ret_t atlsci_led(int sensor_type, int action)
{
    char * led_on =    "L,1";	//Turn on LEDs
    char * led_off =   "L,0"; 	//Turn off LEDs
    char * led_query = "L,?"; 	//Query LEDs
    uint8_t rx_buffer[STRING_SIZE] = {0};

    int ret = set_sensor_address(i2c_context, sensor_type);

    switch (action)
    {
        case ATLSCI_LED_ON:    ret = send_string(i2c_context, led_on); break;
        case ATLSCI_LED_OFF:   ret = send_string(i2c_context, led_off); break;
        case ATLSCI_LED_QUERY: ret = send_string(i2c_context, led_query); break;
        default:               ret = -1; break;
    }

    if (ret == MRAA_SUCCESS)
    {
        ret = mraa_i2c_read(i2c_context, rx_buffer, sizeof(rx_buffer));
        if(action == ATLSCI_LED_QUERY)
			printf("%s\n", rx_buffer);
        return parse_atlsci_led(rx_buffer, action);
    }

    log_mraa_response("atlsci_led()", ret);

    return parse_atlsci_led(rx_buffer, action);

}

static atlsci_read_ret_t parse_atlsci_read(int sensor_type, uint8_t * ret_string)
{
    atlsci_read_ret_t ret = {0};

    //The first byte of the rx_buffer led_string indicates
    //that the device receive the command successfully
    ret.success = (ret_string[0] = '1') ? 1 : 0;

    if (ret.success == 1)
    {
        //parse and retrieve only the EC value
        if(sensor_type == ATLSCI_CONDUCTIVITY)
        {
            int counter = 1;
	        while(ret_string[counter] != ',')
	        {
		        ret.data[counter-1] = ret_string[counter];
		        ++counter;
	        }
	        ret.data[counter-1] = '\0';
            //ret.data = cal_ret;
        }
        else
        {
        	//strncpy(ret.data, ret_string + 1, STRING_SIZE - 1);
            sprintf(ret.data, "%s", ret_string + 1); //test
        }
    }

    return ret;
}


static atlsci_read_ret_t parse_atlsci_read_fail(void)
{
    atlsci_read_ret_t ret;
    ret.success = 0;
    ERROR("Uh-oh! parse_atlsci_read_fail()\n");
    return ret;
}

/*
 * Run the read command 'r' to read the data obtained from the sensor
 *
 * Return message format:
 *    For ORP, pH, D.O.:
 *      1 DATA null
 *    where DATA represents many bytes.
 *    For Conductivity:
 *      1 EC , TDS, SAL , SG null
 *    where EC, TDS, SAL, SG represent many bytes
 */
atlsci_read_ret_t atlsci_read(int sensor_type) {
    int ret = -1;
    uint8_t rx_buffer[STRING_SIZE] = {0};

    ret = set_sensor_address(i2c_context, sensor_type);

    if (ret == MRAA_SUCCESS)
    {
        ret = send_string(i2c_context, "r");
        if (ret == MRAA_SUCCESS)
        {
            ret = mraa_i2c_read(i2c_context,
                                rx_buffer,
                                sizeof(rx_buffer));

            return parse_atlsci_read(sensor_type, rx_buffer);
        }
        else
        {
        	log_mraa_response("atlsci_read()", ret);
        }
    }
    else
    {
    	log_mraa_response("atlsci_read()", ret);
    }

    return parse_atlsci_read_fail();
}


static atlsci_reset_ret_t parse_atlsci_reset(int success)
{
    atlsci_reset_ret_t ret;
    ret.success = success;

    return ret;
}

atlsci_reset_ret_t atlsci_reset(int sensor_type) {
    int ret = -1;
    char * reset_msg = "X";

    ret = set_sensor_address(i2c_context, sensor_type);

    if (ret == MRAA_SUCCESS)
    {
        ret = send_string(i2c_context, reset_msg);

        if (ret == MRAA_SUCCESS)
        {
            return parse_atlsci_reset(ATLSCI_SUCCESS_BYTE_INDICATOR);
        }
    }

    log_mraa_response("atlsci_read()", ret);

    return parse_atlsci_reset(ATLSCI_FAILURE);
}

static atlsci_sleep_ret_t parse_atlsci_sleep(int success)
{
    atlsci_sleep_ret_t ret;
    ret.success = success;

    return ret;
}

atlsci_sleep_ret_t atlsci_sleep(int sensor_type)
{
    char * sleep_msg = "SLEEP";
    int ret = -1;

    ret = set_sensor_address(i2c_context, sensor_type);

    if (ret == MRAA_SUCCESS)
    {
        ret = send_string(i2c_context, sleep_msg);

        if (ret == MRAA_SUCCESS)
        {
            return parse_atlsci_sleep(ATLSCI_SUCCESS_BYTE_INDICATOR);
        }
    }

    log_mraa_response("atlsci_sleep()", ret);

    return parse_atlsci_sleep(ATLSCI_FAILURE);
}

/**
 * Return message format: "?STATUS,P,5.038", where:
 *  P is the reason for the last reset event
 *    Possible events are: P -> power on reset,
 *                         S -> software reset,
 *                         B -> brown out reset,
 *                         W -> watchdog reset,
 *                         U -> unknown
 *  5.038 is its voltage at the VCC pin
 *
 */
static atlsci_status_ret_t parse_atlsci_status(uint8_t * status_string, int success)
{
    atlsci_status_ret_t ret;
    int vcc_voltage_size = 6;
    int vcc_index = 11;

    ret.success = success;
    ret.last_reset_event = status_string[9];
    ret.vcc_voltage = malloc(vcc_voltage_size);

    memcpy(ret.vcc_voltage, &status_string[vcc_index], vcc_voltage_size);

    return ret;
}

atlsci_status_ret_t atlsci_status(int sensor_type) {
    int ret = -1;
    uint8_t rx_buffer[STRING_SIZE] = {0};
    char * status_msg = "STATUS";
    int success = 0;

    ret = set_sensor_address(i2c_context, sensor_type);

    if (ret == MRAA_SUCCESS)
    {
        ret = send_string(i2c_context, status_msg);
        if (ret == MRAA_SUCCESS)
        {
            ret = mraa_i2c_read(i2c_context, rx_buffer, sizeof(rx_buffer));
            success = ATLSCI_SUCCESS_BYTE_INDICATOR;
        }
        else
        {
        	log_mraa_response("atlsci_status()", ret);
        }
    }
    else
    {
    	log_mraa_response("atlsci_status()", ret);
    }
    return parse_atlsci_status(rx_buffer, success);
}

int atlsci_init()
{
	i2c_context = mraa_i2c_init_raw(6);

    if (i2c_context == NULL)
    {
        DEBUG("atlsci_init(): mraa_i2c_init_raw() failure. Failed to initialize I2C\n");
        return ATLSCI_FAILURE;
    }

    return ATLSCI_SUCCESS;
}

int atlsci_deinit()
{
    mraa_i2c_stop(i2c_context);
    i2c_context = NULL;

    return ATLSCI_SUCCESS;
}
