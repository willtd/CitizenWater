#include "sensors.h"

void cal_test_ph()
{
	atlsci_cal_param_t ph_param;

	ph_param.cal_ph =  PH_CALIBRATION_CLEAR;
	printf("PH_CALIBRATION_CLEAR success: %d\n", atlsci_cal(ATLSCI_PH, ph_param, 0).success);

	ph_param.cal_ph = PH_CALIBRATION_QUERY;
	printf("PH_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_PH, ph_param, 0).success);

	ph_param.cal_ph = PH_MID_CALIBRATION_POINT;
	printf("PH_MID_CALIBRATION_POINT success: %d\n", atlsci_cal(ATLSCI_PH, ph_param, 7.00).success);

	ph_param.cal_ph = PH_CALIBRATION_QUERY;
	printf("PH_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_PH, ph_param, 0).success);

	ph_param.cal_ph = PH_LOW_CALIBRATION_POINT;
	printf("PH_LOW_CALIBRATION_POINT success: %d\n", atlsci_cal(ATLSCI_PH, ph_param, 2.5).success);

	ph_param.cal_ph = PH_CALIBRATION_QUERY;
	printf("PH_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_PH, ph_param, 0).success);

	ph_param.cal_ph = PH_HIGH_CALIBRATION_POINT;
	printf("PH_HIGH_CALIBRATION_POINT success: %d\n", atlsci_cal(ATLSCI_PH, ph_param, 10.5).success);

	ph_param.cal_ph = PH_CALIBRATION_QUERY;
	printf("PH_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_PH, ph_param, 0).success);

	ph_param.cal_ph =  PH_CALIBRATION_CLEAR;
	printf("PH_CALIBRATION_CLEAR success: %d\n", atlsci_cal(ATLSCI_PH, ph_param, 0).success);

	ph_param.cal_ph = PH_CALIBRATION_QUERY;
	printf("PH_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_PH, ph_param, 0).success);
}

void cal_test_do()
{
	atlsci_cal_param_t do_param;

	do_param.cal_do = DO_CALIBRATION_QUERY;
	printf("DO_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_DO, do_param, 0).success);

	do_param.cal_do = DO_ATMOSPHERIC_CALIBRATION;
	printf("DO_ATMOSPHERIC_CALIBRATION success: %d\n", atlsci_cal(ATLSCI_DO, do_param, 0).success);

	do_param.cal_do = DO_CALIBRATION_QUERY;
	printf("DO_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_DO, do_param, 0).success);

	do_param.cal_do = DO_ZERO_LEVEL_CALIBRATION;
	printf("DO_ZERO_LEVEL_CALIBRATION success: %d\n", atlsci_cal(ATLSCI_DO, do_param, 0).success);

	do_param.cal_do = DO_CALIBRATION_CLEAR;
	printf("DO_CALIBRATION_CLEAR success: %d\n", atlsci_cal(ATLSCI_DO, do_param, 0).success);

	do_param.cal_do = DO_CALIBRATION_QUERY;
	printf("DO_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_DO, do_param, 0).success);
}

void cal_test_orp()
{
	atlsci_cal_param_t orp_param;

	orp_param.cal_orp = ORP_CALIBRATION_QUERY;
	printf("ORP_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_ORP, orp_param, 0).success);

	orp_param.cal_orp = ORP_SINGLE_CALIBRATION_POINT;
	printf("ORP_SINGLE_CALIBRATION_POINT success: %d\n", atlsci_cal(ATLSCI_ORP, orp_param, 4.5).success);

	orp_param.cal_orp = ORP_CALIBRATION_QUERY;
	printf("ORP_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_ORP, orp_param, 0).success);

	orp_param.cal_orp = ORP_CALIBRATION_CLEAR;
	printf("ORP_CALIBRATION_CLEAR success: %d\n", atlsci_cal(ATLSCI_ORP, orp_param, 0).success);

	orp_param.cal_orp = ORP_CALIBRATION_QUERY;
	printf("ORP_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_ORP, orp_param, 0).success);

}

void cal_test_conductivity()
{
	atlsci_cal_param_t conductivity_param;

	conductivity_param.cal_conductivity = CONDUCTIVITY_CALIBRATION_QUERY;
	printf("CONDUCTIVITY_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 0).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_DRY_CALIBRATION;
	printf("CONDUCTIVITY_DRY_CALIBRATION success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 0).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_SINGLE_CALIBRATION_POINT;
	printf("CONDUCTIVITY_SINGLE_CALIBRATION_POINT success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 1000).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_CALIBRATION_QUERY;
	printf("CONDUCTIVITY_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 0).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_DRY_CALIBRATION;
	printf("CONDUCTIVITY_DRY_CALIBRATION success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 0).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_LOW_DUAL_CALIBRATION_POINT;
	printf("CONDUCTIVITY_LOW_DUAL_CALIBRATION_POINT success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 2000).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_CALIBRATION_QUERY;
	printf("CONDUCTIVITY_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 0).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_DRY_CALIBRATION;
	printf("CONDUCTIVITY_DRY_CALIBRATION success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 0).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_HIGH_DUAL_CALIBRATION_POINT;
	printf("CONDUCTIVITY_HIGH_DUAL_CALIBRATION_POINT success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 3000).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_CALIBRATION_QUERY;
	printf("CONDUCTIVITY_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 0).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_CALIBRATION_CLEAR;
	printf("CONDUCTIVITY_CALIBRATION_CLEAR success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 0).success);

	conductivity_param.cal_conductivity = CONDUCTIVITY_CALIBRATION_QUERY;
	printf("CONDUCTIVITY_CALIBRATION_QUERY success: %d\n", atlsci_cal(ATLSCI_CONDUCTIVITY, conductivity_param, 0).success);
}

void test_led()
{
	atlsci_led_ret_t led_DO, led_ORP, led_PH, led_Conductivity;

	//DO LED-----------
	led_DO = atlsci_led(ATLSCI_DO, ATLSCI_LED_QUERY);
	printf("DO - success: %d, state: %d\n", led_DO.success, led_DO.led_state);

	led_DO = atlsci_led(ATLSCI_DO, ATLSCI_LED_OFF);
	printf("DO - success: %d, state: %d\n", led_DO.success, led_DO.led_state);

	led_DO = atlsci_led(ATLSCI_DO, ATLSCI_LED_QUERY);
	printf("DO - success: %d, state: %d\n", led_DO.success, led_DO.led_state);

	led_DO = atlsci_led(ATLSCI_DO, ATLSCI_LED_ON);
	printf("DO - success: %d, state: %d\n", led_DO.success, led_DO.led_state);

	led_DO = atlsci_led(ATLSCI_DO, ATLSCI_LED_QUERY);
	printf("DO - success: %d, state: %d\n", led_DO.success, led_DO.led_state);

	//ORP LED------------
	led_ORP = atlsci_led(ATLSCI_ORP, ATLSCI_LED_QUERY);
	printf("ORP - success: %d, state: %d\n", led_ORP.success, led_ORP.led_state);

	led_ORP = atlsci_led(ATLSCI_ORP, ATLSCI_LED_OFF);
	printf("ORP - success: %d, state: %d\n", led_ORP.success, led_ORP.led_state);

	led_ORP = atlsci_led(ATLSCI_ORP, ATLSCI_LED_QUERY);
	printf("ORP - success: %d, state: %d\n", led_ORP.success, led_ORP.led_state);

	led_ORP = atlsci_led(ATLSCI_ORP, ATLSCI_LED_ON);
	printf("ORP - success: %d, state: %d\n", led_ORP.success, led_ORP.led_state);

	led_ORP = atlsci_led(ATLSCI_ORP, ATLSCI_LED_QUERY);
	printf("ORP - success: %d, state: %d\n", led_ORP.success, led_ORP.led_state);

	// PH LED----------------
	led_PH = atlsci_led(ATLSCI_PH, ATLSCI_LED_QUERY);
	printf("PH - success: %d, state: %d\n", led_PH.success, led_PH.led_state);

	led_PH = atlsci_led(ATLSCI_PH, ATLSCI_LED_OFF);
	printf("PH - success: %d, state: %d\n", led_PH.success, led_PH.led_state);

	led_PH = atlsci_led(ATLSCI_PH, ATLSCI_LED_QUERY);
	printf("PH - success: %d, state: %d\n", led_PH.success, led_PH.led_state);

	led_PH = atlsci_led(ATLSCI_PH, ATLSCI_LED_ON);
	printf("PH - success: %d, state: %d\n", led_PH.success, led_PH.led_state);

	led_PH = atlsci_led(ATLSCI_PH, ATLSCI_LED_QUERY);
	printf("PH - success: %d, state: %d\n", led_PH.success, led_PH.led_state);

	// Conductivity
	led_Conductivity = atlsci_led(ATLSCI_CONDUCTIVITY, ATLSCI_LED_QUERY);
	printf("Conductivity - success: %d, state: %d\n", led_Conductivity.success, led_Conductivity.led_state);

	led_Conductivity = atlsci_led(ATLSCI_CONDUCTIVITY, ATLSCI_LED_OFF);
	printf("Conductivity - success: %d, state: %d\n", led_Conductivity.success, led_Conductivity.led_state);

	led_Conductivity = atlsci_led(ATLSCI_CONDUCTIVITY, ATLSCI_LED_QUERY);
	printf("Conductivity - success: %d, state: %d\n", led_Conductivity.success, led_Conductivity.led_state);

	led_Conductivity = atlsci_led(ATLSCI_CONDUCTIVITY, ATLSCI_LED_ON);
	printf("Conductivity - success: %d, state: %d\n", led_Conductivity.success, led_Conductivity.led_state);

	led_Conductivity = atlsci_led(ATLSCI_CONDUCTIVITY, ATLSCI_LED_QUERY);
	printf("Conductivity - success: %d, state: %d\n", led_Conductivity.success, led_Conductivity.led_state);

}

void test_info()
{

	atlsci_device_info_ret_t info_ph = atlsci_device_info(ATLSCI_PH);
	printf("PH - success %d, device_type %s, firmware_version: %s\n",
			info_ph.success, info_ph.device_type, info_ph.firmware_version);

	atlsci_device_info_ret_t info_orp = atlsci_device_info(ATLSCI_ORP);
	printf("ORP - success %d, device_type %s, firmware_version: %s\n",
			info_orp.success, info_orp.device_type, info_orp.firmware_version);

	atlsci_device_info_ret_t info_do = atlsci_device_info(ATLSCI_DO);
	printf("DO - success %d, device_type %s, firmware_version: %s\n",
			info_do.success, info_do.device_type, info_do.firmware_version);

	atlsci_device_info_ret_t info_conductivity = atlsci_device_info(ATLSCI_CONDUCTIVITY);
	printf("Conductivity - success %d, device_type %s, firmware_version: %s\n",
			info_conductivity.success, info_conductivity.device_type, info_conductivity.firmware_version);
}

void test_reset()
{
	atlsci_reset_ret_t reset_ph = atlsci_reset(ATLSCI_PH);
	printf("PH - Success: %d\n", reset_ph.success);

	atlsci_reset_ret_t reset_orp = atlsci_reset(ATLSCI_ORP);
	printf("ORP - Success: %d\n", reset_orp.success);

	atlsci_reset_ret_t reset_do = atlsci_reset(ATLSCI_DO);
	printf("DO - Success: %d\n", reset_do.success);

	atlsci_reset_ret_t reset_conductivity = atlsci_reset(ATLSCI_CONDUCTIVITY);
	printf("Conductivity - Success: %d\n", reset_conductivity.success);
}


void test_read()
{

	atlsci_read_ret_t ret_DO = atlsci_read(ATLSCI_DO);
	printf("DO: %d\n%s\n", ret_DO.success, ret_DO.data);

	atlsci_read_ret_t ret_ORP = atlsci_read(ATLSCI_ORP);
	printf("ORP: %d\n%s\n", ret_ORP.success, ret_ORP.data);

	atlsci_read_ret_t ret_PH = atlsci_read(ATLSCI_PH);
	printf("PH: %d\n%s\n", ret_PH.success, ret_PH.data);

	atlsci_read_ret_t ret_Conductivity = atlsci_read(ATLSCI_CONDUCTIVITY);
	printf("Conductivity: %d\n%s\n", ret_Conductivity.success, ret_Conductivity.data);
}

void test_sleep()
{

	atlsci_sleep_ret_t sleep_DO = atlsci_sleep(ATLSCI_DO);
	printf("DO - Success: %d\n", sleep_DO.success);

	atlsci_sleep_ret_t sleep_PH = atlsci_sleep(ATLSCI_PH);
	printf("PH - Success: %d\n", sleep_PH.success);

	atlsci_sleep_ret_t sleep_ORP = atlsci_sleep(ATLSCI_ORP);
	printf("ORP - Success: %d\n", sleep_ORP.success);

	atlsci_sleep_ret_t sleep_Conductivity = atlsci_sleep(ATLSCI_CONDUCTIVITY);
	printf("Conductivity - Success: %d\n", sleep_Conductivity.success);
}

void test_status()
{
	atlsci_status_ret_t status_DO = atlsci_status(ATLSCI_DO);
	printf("DO success: %d, last_reset_event %c, vcc_voltage: %s\n", status_DO.success,
			status_DO.last_reset_event, status_DO.vcc_voltage);

	atlsci_status_ret_t status_PH = atlsci_status(ATLSCI_PH);
	printf("PH success: %d, last_reset_event %c, vcc_voltage: %s\n", status_PH.success,
			status_PH.last_reset_event, status_PH.vcc_voltage);

	atlsci_status_ret_t status_ORP = atlsci_status(ATLSCI_ORP);
	printf("ORP success: %d, last_reset_event %c, vcc_voltage: %s\n", status_ORP.success,
			status_ORP.last_reset_event, status_ORP.vcc_voltage);

	atlsci_status_ret_t status_conductivity = atlsci_status(ATLSCI_CONDUCTIVITY);
	printf("conductivity success: %d, last_reset_event %c, vcc_voltage: %s\n", status_conductivity.success,
			status_conductivity.last_reset_event, status_conductivity.vcc_voltage);
}

void temp_test()
{
	atlsci_temp_comp_param set_temp = SET_TEMPERATURE;
	atlsci_temp_comp_param query_temp = QUERY_TEMPERATURE;

	atlsci_temp_comp_ret_t temp_ph = atlsci_temp_comp(ATLSCI_PH, query_temp, 0);
	temp_ph = atlsci_temp_comp(ATLSCI_PH, set_temp, 10.1);
	printf("ph temp success: %d\n", temp_ph.success);
	temp_ph = atlsci_temp_comp(ATLSCI_PH, query_temp, 0);

	atlsci_temp_comp_ret_t temp_do = atlsci_temp_comp(ATLSCI_DO, query_temp, 0);
	temp_do = atlsci_temp_comp(ATLSCI_DO, set_temp, 15.4);
	printf("do temp success: %d\n", temp_do.success);
	temp_do = atlsci_temp_comp(ATLSCI_DO, query_temp, 0);

	atlsci_temp_comp_ret_t temp_conductivity = atlsci_temp_comp(ATLSCI_CONDUCTIVITY, query_temp, 0);
	temp_conductivity = atlsci_temp_comp(ATLSCI_CONDUCTIVITY, set_temp, 18.1);
	printf("conductivity temp success: %d\n", temp_conductivity.success);
	temp_conductivity = atlsci_temp_comp(ATLSCI_CONDUCTIVITY, query_temp, 0);
}

int main(void)
{
	atlsci_init();

	printf("Read test!\n");
	test_read();

	/*
	printf("Sleep test!\n");
	test_sleep();

	printf("Reset test!\n");
	test_reset();


	printf("info test!\n");
	test_info();

	printf("Status test!\n");
	test_status();

	printf("LED Test!\n");
	test_led();

	printf("Calibration test!\n");
	printf("PH CALIBRATION-----------\n");
	cal_test_ph();
	printf("\nDO CALIBRATION-----------\n");
	cal_test_do();
	printf("\nORP CALIBRATION----------\n");
	cal_test_orp();
	printf("\nCONDUCTIVITY CALIBRATION-------------\n");
	cal_test_conductivity();

	printf("Temperature Compensation test!\n");
	temp_test();
	*/
	
	atlsci_deinit();
}
