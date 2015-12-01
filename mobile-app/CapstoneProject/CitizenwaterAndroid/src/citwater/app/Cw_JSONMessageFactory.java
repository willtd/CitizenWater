package citwater.app;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Scanner;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.util.Log;
import citwater.data.Cw_DaySchedule;
import citwater.data.Cw_DayScheduleComparator;
import citwater.data.Cw_Device;
import citwater.data.Cw_SensorCalibrationOption;
import citwater.data.Cw_SensorDevice;

public class Cw_JSONMessageFactory {

	private static final String LOG_TAG = "Cw_JSONMessageFactory";
	
	public static void translateScheduleJSON(List<Cw_DaySchedule> schedule, JSONArray scheduleJSON) throws JSONException 
	{
		schedule.clear();
		for (int i = 0; i < scheduleJSON.length(); i++)
		{
			JSONObject dayJSON = scheduleJSON.getJSONObject(i);

			Cw_DaySchedule day = new Cw_DaySchedule(dayJSON.getString("day")
												  , dayJSON.getString("start_time")
												  , dayJSON.getString("finish_time")
												  , dayJSON.getString("interval"));
			Log.i(LOG_TAG, dayJSON.getString("day") + " " + dayJSON.getString("start_time") + " " + dayJSON.getString("finish_time") + " " +  dayJSON.getString("interval"));
			schedule.add(day);
		}
		
		Collections.sort(schedule, new Cw_DayScheduleComparator());
	}
	
	private static Cw_SensorCalibrationOption generateCalibrationOption(String calTypeName, String optString)
	{
		Scanner scanner = new Scanner(optString);
		Cw_SensorCalibrationOption opt = null;
		
		if (scanner.hasNext())
		{
			char type = scanner.next(".").charAt(0);
			
			switch (type)
			{
				case 'N':
					opt = new Cw_SensorCalibrationOption(calTypeName, scanner.nextLine());
					break;
				case 'D':
					double lowerBound; 
					double upperBound; 
					
					if (scanner.hasNextDouble())	
					{
						lowerBound = scanner.nextDouble();
						if (scanner.hasNextDouble())
						{
							upperBound = scanner.nextDouble();
							if (scanner.hasNextLine())
							{
								opt = new Cw_SensorCalibrationOption(calTypeName, scanner.nextLine(), lowerBound, upperBound);
							}
						}
					}

					break;
				case 'I':
					int integerLowerBound = 0; 
					int integerUpperBound; 
					
					if (scanner.hasNextInt())
					{
						integerLowerBound = scanner.nextInt();
						if (scanner.hasNextInt())
						{
							integerUpperBound = scanner.nextInt();
							if (scanner.hasNextLine())
							{
								opt = new Cw_SensorCalibrationOption(calTypeName, scanner.nextLine(), integerLowerBound, integerUpperBound);	
							}
						}
					}
					
					break;
				default:
					opt = null;
			}
		}

		scanner.close();
		
		return opt;
	}
	
	private static ArrayList<Cw_SensorCalibrationOption> translateCalibrationJSON(JSONObject calibrationJSON)
	{
		ArrayList<Cw_SensorCalibrationOption> calibrationOptions = new ArrayList<Cw_SensorCalibrationOption>();
		
		if (calibrationJSON.names() != null)
		{
			for (int j = 0; j < calibrationJSON.names().length(); j++)
			{
				String calTypeName = "";
				String optString   = "";
				
				try {
					calTypeName = calibrationJSON.names().getString(j);
					optString = calibrationJSON.getString(calTypeName);
					Log.i(LOG_TAG, calTypeName + " " + optString);
					Cw_SensorCalibrationOption opt = generateCalibrationOption(calTypeName, optString);
					
					if (opt != null)
					{
						calibrationOptions.add(opt);
					}
				} catch (JSONException e) {
				}

			}
		}
		
		return calibrationOptions;
	}

	private static List<Cw_SensorDevice> translateSensorListJSON(JSONArray sensorListJSON) throws JSONException
	{
		List<Cw_SensorDevice> sensorList = new ArrayList<Cw_SensorDevice>();
		Log.i(LOG_TAG, "Entered translateSensorListJSON");
		
		for (int i = 0; i < sensorListJSON.length(); i++)
		{
			Log.i(LOG_TAG, "Entered translateSensorListJSON, rd 1");
			
			JSONObject sensorJSON = sensorListJSON.getJSONObject(i);
			
			ArrayList<Cw_SensorCalibrationOption> calibrationOptions 
					= translateCalibrationJSON(sensorJSON.getJSONObject("sensorCalParam"));
			
			Cw_SensorDevice sensor = new Cw_SensorDevice(sensorJSON.getString("sensorType")
													   , sensorJSON.getString("sensorBrand")
													   , sensorJSON.getString("sensorDescription")
													   , calibrationOptions);
			
			sensorList.add(sensor);
		}
		
		return sensorList;
	}
	

	public static boolean translateReplyJSON(ArrayList<String> successMessages, 
											 ArrayList<String> errorMessages,
											 JSONObject replyMessage)
	{
		try
		{
			boolean success = replyMessage.getBoolean("success");
			
			if (success)
			{
				successMessages.add("Success");
			}
			else
			{
				errorMessages.add("Failure");
			}
			
			JSONArray messageArray = replyMessage.getJSONArray("messages");
			
			for (int i = 0; i < messageArray.length(); i++)
			{
				successMessages.add(messageArray.getString(i));
			}
			
			messageArray = replyMessage.getJSONArray("error_messages");
			
			for (int i = 0; i < messageArray.length(); i++)
			{
				errorMessages.add(messageArray.getString(i));
			}	
		}
		catch (JSONException e)
		{
			Log.i(LOG_TAG, "Failed to parse reply message");
			successMessages.clear();
			errorMessages.clear();
			return false;
		}
		
		return true;
	}
	
	/* 
	 *  Example JSON init message:
	 * 		{
	 * 			"success": true
	 * 
	 * 			"deviceName": ""
	 * 			
	 * 			"sensors": [
	 * 				{
	 * 					"sensorType": "pH",
	 * 					"sensorBrand": "atlas-scientific",
	 * 					"sensorDescription": ""
	 * 				},
	 * 				{
	 * 					"sensorType": "conductivity"
	 * 				},
	 * 				...
	 * 			]
	 * 			"schedule": [
	 * 				{
	 * 					"day": "MONDAY",
	 * 					"start_time": "4:00",
	 * 					"finish_time": "5:00",
	 * 					"interval": "5"
	 * 				}
	 * 				...
	 * 			]
	 * 
	 * 		}
	 */
	public static boolean translateInitJSON(Cw_Device device, 
											 JSONObject obj)
	{
		try {
			device.setDeviceName(obj.getString("deviceName"));
		}
		catch (JSONException e)
		{
			Log.i(LOG_TAG, "No deviceName field in init message");
			return false;
		}
		
		try {
			Log.i(LOG_TAG, obj.getJSONArray("sensors").toString());
			device.setSensorList(translateSensorListJSON(obj.getJSONArray("sensors")));
		}
		catch (JSONException e) {
			Log.i(LOG_TAG, "Failed to parse sensors in init message");
			return false;
		}	
		
		try {
			translateScheduleJSON(device.getDayList(), obj.getJSONArray("schedule"));
		} 
		catch (JSONException e) {
			Log.i(LOG_TAG, "Failed to parse schedule in init message");
			return false;
		}
		
		return true;
	}
	
	/*
	 *  Example JSON take readings message format:
	 *  {
	 *  	sensors: [
	 *  		{
	 *  			"sensorType": "pH",
	 *  			"sensorBrand": "atlas-scientific",
	 *  			"actions": {
	 *  				"take_reading": null
	 *  			}
	 *  		},
	 *  		{
	 *  			"sensorType": "conductivity",
	 *  			"sensorBrand": "atlas-scientific",
	 *  			"actions": {
	 *  				"take_reading": null
	 *  			}
	 *  		}
	 *  	]
	 *  }
	 * 
	 */
	public static JSONObject generateTakeReadingsMessage(List<Cw_SensorDevice> sensors)
	{
		JSONObject obj = new JSONObject();
		JSONArray sensorsArray = new JSONArray();
		
		try 
		{
			
			for (int i = 0; i < sensors.size(); i++)
			{
				JSONObject sensorJSON = new JSONObject();
				Cw_SensorDevice sensor = sensors.get(i);
		
				sensorJSON.put("sensorType", sensor.getSensorType());
				sensorJSON.put("sensorBrand", sensor.getSensorBrand());
				JSONObject actionJSON = new JSONObject();
				actionJSON.put("take_reading",  JSONObject.NULL);
				sensorJSON.put("actions", actionJSON);
				sensorsArray.put(sensorJSON);
			}
			obj.put("sensors", sensorsArray);
		}
		catch (JSONException e)
		{
			Log.w("Cw_Bluetooth", "takeMeasurementsFromDevice: error occurred in creating JSON message");
			obj = null;
		}

		return obj;
	}
	
	/*
	 *  Example initial JSON message sent:
	 *  	{
	 *  		"init" : null
	 *  	}
	 */
	public static JSONObject generateInitMessage()
	{
		JSONObject msg = new JSONObject();
		
		try {
			Log.i("JSONMessageFactory", "Inserting init");
			msg.put("init", JSONObject.NULL);
		}
		catch (JSONException e)
		{
			Log.i(LOG_TAG, "Failed to generate init message");
			msg = null;
		}
		
		return msg;
	}
	
	/* 
	 * Example JSON scheduling message format:
	 * 		{
	 * 			"schedule": [
	 * 				{
	 * 					"day": "MONDAY",
	 * 					"start_time": "4:00",
	 * 					"finish_time": "5:00",
	 * 					"interval": "5"
	 * 				}
	 * 				...
	 * 			]
	 * 
	 * 		}
	 */
	public static JSONObject generateScheduleMessage(List<Cw_DaySchedule> schedule)
	{
		JSONObject obj = new JSONObject();
		JSONArray scheduleArray = new JSONArray();
		DecimalFormat fmt = new DecimalFormat("00");
		
		try
		{
			for (int i = 0; i < schedule.size(); i++)
			{
				JSONObject dayJSON = new JSONObject();
				
				Cw_DaySchedule day = schedule.get(i);
				dayJSON.put("day", day.getDay());
				dayJSON.put("start_time", fmt.format(day.getStartTimeHours()) + ":" + fmt.format(day.getStartTimeMinutes()));
				dayJSON.put("finish_time",  fmt.format(day.getEndTimeHours()) + ":" + fmt.format(day.getEndTimeMinutes()));
				dayJSON.put("interval",  (day.getIntervalHours() * 60 + day.getIntervalMinutes()) + "");
				scheduleArray.put(dayJSON);
			}
			
			obj.put("schedule", scheduleArray);
		}
		catch (JSONException e)
		{
			Log.w(LOG_TAG, "Failed to generate schedule message");
			obj = null;
		}
		
		return obj;
	}
	
	/*
	 *  Example JSON Calibration message format:
	 *  {
	 *  	sensors: [
	 *  		{
	 *  			"sensorType": "pH",
	 *  			"sensorBrand": "atlas-scientific",
	 *  			"actions": {
	 *  				"calibrate": 
	 *  					{
	 *  						"FACTORY_RESET" : null,
	 *  					}
	 *  			}
	 *  		}
	 *  	]
	 *  }
	 */
	public static JSONObject generateCalibrationMessage(Cw_SensorDevice sensor, 
														String calibrationType, 
														char paramType, 
														String param)
	{
		JSONObject obj = new JSONObject();
		JSONArray sensorsArray = new JSONArray();
		JSONObject sensorJSON = new JSONObject();
		JSONObject actionJSON = new JSONObject();
		JSONObject calibrateJSON = new JSONObject();
		
		try {
			sensorJSON.put("sensorType", sensor.getSensorType());
			sensorJSON.put("sensorBrand", sensor.getSensorBrand());

			switch (paramType)
			{
				case 'N': calibrateJSON.put(calibrationType,  JSONObject.NULL);     					/* no param */
						  break;
				case 'I': calibrateJSON.put(calibrationType,  Integer.parseInt(param));     /* integer */
						  break;
				case 'D': calibrateJSON.put(calibrationType,  Double.parseDouble(param));   /* float */
						  break;
				default:  obj = null;
						  break;
			}
			
			if (obj != null)
			{
				actionJSON.put("calibrate",  calibrateJSON);
				sensorJSON.put("actions", actionJSON);
				sensorsArray.put(sensorJSON);
				obj.put("sensors", sensorsArray);
			}
		}
		catch (JSONException e)
		{
			Log.i(LOG_TAG, "Error in generating calibration message");
			obj = null;
		}
		
		return obj;
	}

	/* TODO: Implement this */
	public static JSONObject generateDatabaseMessage() {
		JSONObject obj = new JSONObject();
		return obj;
	}

	/*
	 *  Generates a JSONObject containing mock device information
	 */
	public static JSONObject generateFakeDeviceInfo() throws JSONException
	{
		JSONObject obj = new JSONObject();
		
		obj.put("deviceName", "Citizenwater Dev 1");
		
		JSONArray sensors = new JSONArray();
		String[] sensorTypeList = {"pH", "Conductivity", "Dissolved Oxygen", "ORP" };
		for (int i = 0; i < sensorTypeList.length; i++)
		{
			JSONObject sensor = new JSONObject();
			sensor.put("sensorBrand", "atlas-scientific");
			sensor.put("sensorDescription", "This is a sensor from Atlas-Scientific...");
			sensor.put("sensorType", sensorTypeList[i]);
			JSONObject calibrationObj = new JSONObject();
			calibrationObj.put("TEST_CALIBRATION_TYPE_1", "N Reset sensor to factory settings.");
			calibrationObj.put("TEST_CALIBRATION_TYPE_2", "D 1.00 6.00 Perform single-point calibration. Enter a value between 1.00 and 6.00 [pH]");
			calibrationObj.put("TEST_CALIBRATION_TYPE_3", "I 5 200000  Perform double-point calibration. Enter a value between 5 and 200000 [microsiemens]");
			calibrationObj.put("TEST_CALIBRATION_TYPE_4", "Z bad input");
			calibrationObj.put("TEST_CALIBRATION_TYPE_5", "I 12.22 42 bad input");
			calibrationObj.put("TEST_CALIBRATION_TYPE_6", "I");
			calibrationObj.put("TEST_CALIBRATION_TYPE_7", "I 1 2");
			// calibrationObj.put("TEST_CALIBRATION_TYPE_8", "D -13.0 52.0 "); // Valid
			sensor.put("sensorCalParam", calibrationObj);
			sensors.put(sensor);
		}
		obj.put("sensors", sensors);
		
		JSONArray schedule = new JSONArray();
		
		String[] dayList = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
		for (int i = 0; i < dayList.length; i++)
		{
			JSONObject day = new JSONObject();
			day.put("day", dayList[i]);
			day.put("start_time",  "4:00");
			day.put("finish_time", "5:00");
			day.put("interval",    "5");
			schedule.put(day);
		}
		
		obj.put("schedule", schedule);
		
		return obj;
	}
}
