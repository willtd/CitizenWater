#ifndef SENSORS_H_

#define SENSORS_H_

#define ATLSCI_DO            0
#define ATLSCI_ORP           1
#define ATLSCI_PH            2 
#define ATLSCI_CONDUCTIVITY  3

#include "../shared/macros.h"

/**
 *  Translate the MRAA error code to a human-readable string.
 */
const char * mraa_result_to_string(int res);

/**
 *  Logs an MRAA error code
 */
void log_mraa_response(char * caller, int res);

/**
 *  Returns the type of the sensor (conductivity, pH, etc.) as a string.
 */
char * get_sensor_type_as_string(int sensor_type);

/*
 * The pH circuit has single point, dual point, and three point calibration.
 * PH_MID_CALIBRATION_POINT must be calibrated first with 7.xx before calibrating low
 * or high point
 *
 * PH_MID_CALIBRATION_POINT - typically 7.00
 * PH_LOW_CALIBRATION_POINT - range 1 - 6
 * PH_HIGH_CALIBRATION_POINT - range 8 - 14
 */
typedef enum atlsci_cal_ph {
	PH_CALIBRATION_CLEAR,
	PH_CALIBRATION_QUERY,
	PH_LOW_CALIBRATION_POINT,
	PH_MID_CALIBRATION_POINT,
	PH_HIGH_CALIBRATION_POINT,
}atlsci_cal_ph_param;

/*
 * The DO circuit can be calibrated to atmospheric oxygen level
 * and 0 Dissolved oxygen.
 */
typedef enum atlsci_cal_do {
	DO_CALIBRATION_CLEAR,
	DO_CALIBRATION_QUERY,
	DO_ATMOSPHERIC_CALIBRATION,
	DO_ZERO_LEVEL_CALIBRATION,
}atlsci_cal_do_param;

/*
 * The ORP circuit can be calibrated using just a
 * single point. ORP_SINGLE_CALIBRATION_POINT accepts
 * integer value nnn
 */
typedef enum atlsci_cal_orp {
	ORP_CALIBRATION_CLEAR,
	ORP_CALIBRATION_QUERY,
	ORP_SINGLE_CALIBRATION_POINT,
}atlsci_cal_orp_param;

/*
 * The Conductivity circuit permits one or dual calibration
 * points. Dry Calibration must always be done first (make sure
 * the probe is dry). The value that the circuit take
 * is expressed in microsiemens. The probe that will be used 
 * is model K1, which follows the following parameters:
 *
 * For simplicity, let single point be within 5-50000 range,
 * low dual calibration point be between 5 - 200000,
 * and high dual calibration point be between 5 - 200000
 */
typedef enum atlsci_cal_conductivity {
	CONDUCTIVITY_CALIBRATION_CLEAR,
	CONDUCTIVITY_CALIBRATION_QUERY,
	CONDUCTIVITY_DRY_CALIBRATION,
	CONDUCTIVITY_SINGLE_CALIBRATION_POINT,
	CONDUCTIVITY_LOW_DUAL_CALIBRATION_POINT,
	CONDUCTIVITY_HIGH_DUAL_CALIBRATION_POINT,
}atlsci_cal_conductivity_param;

typedef struct atlsci_cal_param_t {
	atlsci_cal_ph_param cal_ph;
	atlsci_cal_do_param cal_do;
	atlsci_cal_orp_param cal_orp;
	atlsci_cal_conductivity_param cal_conductivity;
}atlsci_cal_param_t;

/**
 *  By Atlas-Scientific standard, the device would response
 *  a 1 if it has been successfully processed. It's contrary to C standard of
 *  0 being success, but to make things easy, we will stick with
 *  the Atlas-Scientific standard and make success = 1 if command succeeded.
 */
typedef struct atlsci_cal_ret_t {
    int success;
    char * query;      //string return of query
} atlsci_cal_ret_t;

atlsci_cal_ret_t atlsci_cal(int sensor_type, atlsci_cal_param_t param, float value);

typedef struct atlsci_device_info_ret_t {
    int    success;           // An indication of whether the command succeeded
    char * device_type;       // pH, EC, DO, or OR(P)
    char * firmware_version;  // Firmware version number
} atlsci_device_info_ret_t;

typedef enum atlsci_temp_comp_param {
	SET_TEMPERATURE,
	QUERY_TEMPERATURE,
}atlsci_temp_comp_param;

typedef struct atlsci_temp_comp_ret_t {
	int success;
}atlsci_temp_comp_ret_t;

atlsci_temp_comp_ret_t atlsci_temp_comp(int sensor_type, atlsci_temp_comp_param param, float value);
/**
 * Run the command 'I' to retrieve device information, such as device type and
 * firmware version.
 */
atlsci_device_info_ret_t atlsci_device_info(int sensor_type);

typedef enum atlsci_led_action_t {
    ATLSCI_LED_ON,
    ATLSCI_LED_OFF,
    ATLSCI_LED_QUERY
} atlsci_led_action_t;

typedef struct atlsci_led_ret_t {
    int success;   // Indicates whether the given action succeeded
    int led_state; // Applicable only if action = ATLSCI_LED_QUERY.
                   // Either 1 (enabled) or 0 (disabled)
} atlsci_led_ret_t;

/**
 * Run the LED command 'L' with various combination (on, off, query)
 * to control the LED of the selected sensor
 *
 * Descriptions of actions:
 *    ATLSCI_LED_ON:    Enable the blue power and standby LED.
 *    ATLSCI_LED_OFF:   Disable the LEDs.
 *    ATLSCI_LED_QUERY: Query the LED's status.
 */
atlsci_led_ret_t atlsci_led(int sensor_type, int action);

typedef struct atlsci_read_ret_t {
    int success;
    char data[STRING_SIZE]; // Data. Note the different format of the conductivity sensor.
} atlsci_read_ret_t;

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
atlsci_read_ret_t atlsci_read(int sensor_type);

typedef struct atlsci_reset_ret_t {
    int success;
} atlsci_reset_ret_t;

/**
 *  Run the command reset to reset the device to factory settings.
 *  For further details, refer to the Atlas-Scientific datasheets.
 *
 *  A STATUS command can be issued
 *  separately to confirm that the device was reset.
 */
atlsci_reset_ret_t atlsci_reset(int sensor_type);


typedef struct atlsci_sleep_ret_t {
    int success;
} atlsci_sleep_ret_t;

/**
 *   This command can be used to put the device in a low-power sleep mode,
 *   To wake the circuit after putting it to sleep, send it any command.
 *   After waking it, X consecutive readings should be taken before the
 *   readings are considered valid, where:
 *
 *      X = 4 if ORP, Conductivity, or pH
 *          16 if dissolved oxygen
 */
atlsci_sleep_ret_t atlsci_sleep(int sensor_type);

typedef struct atlsci_status_ret_t {
    int success;
    char last_reset_event; // P -> power on reset, S -> software reset,
                           // B -> brown out reset, W -> watchdog reset,
                           // U -> unknown
    char * vcc_voltage;    // string of a floating point number, e.g., 5.038
} atlsci_status_ret_t;

/**
 * Run the command status to report device voltage at VCC pin and the
 * reason the device was last restarted.
 */
atlsci_status_ret_t atlsci_status(int sensor_type);

int atlsci_init();

int atlsci_deinit();

#endif
