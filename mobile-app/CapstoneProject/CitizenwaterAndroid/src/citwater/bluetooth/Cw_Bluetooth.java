package citwater.bluetooth;


import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;
import android.widget.Toast;
import citwater.app.Cw_JSONMessageFactory;
import citwater.data.Cw_DatabaseHelper;
import citwater.data.Cw_DaySchedule;
import citwater.data.Cw_Device;
import citwater.data.Cw_ReplyMessage;
import citwater.data.Cw_SensorDevice;

public class Cw_Bluetooth {

	private final static String LOG_TAG = "Cw_Bluetooth";
	
	/*
	 *  Sets up Cw_Device with information about a mock device
	 */
	public static void setupFakeCwDeviceFromJSON(Cw_Device device)
	{
		JSONObject obj;
		
		try {
			obj = Cw_JSONMessageFactory.generateFakeDeviceInfo();
			Cw_JSONMessageFactory.translateInitJSON(device, obj);
			
		} catch (JSONException e) {
			Log.i(LOG_TAG, "Failed to setup fake device");
		}
	}
	
	private static boolean DEBUG_NO_BLUETOOTH = false;
	
	@SuppressLint("NewApi")
	private static BluetoothSocket setupBluetoothSocket(Cw_Device citwDevice, BluetoothDevice blueDevice)
	{
		BluetoothSocket sock = null;
		try {
			
			sock = blueDevice.createInsecureRfcommSocketToServiceRecord(citwDevice.getUuid());
			sock.connect();

		}
		catch (IOException e)
		{
			Log.i(LOG_TAG, "IOException");
			sock = null;
		}
		return sock;
	}
	
	/*
	 *  Connects to a Citizenwater Device.
	 *  Necessary information is stored in Cw_Device.
	 */
	public static void connectToDevice(Context context, Cw_Device citwDevice, BluetoothDevice blueDevice)
	{
		citwDevice.setConnected(false);
		if (DEBUG_NO_BLUETOOTH)
		{
			setupFakeCwDeviceFromJSON(citwDevice);
			
			citwDevice.setConnected(true);
		}
		else
		{
			BluetoothSocket sock = setupBluetoothSocket(citwDevice, blueDevice);

			if (sock == null)
			{
				Log.i(LOG_TAG, "setupBluetoothSocket() failed");
			}
			else
			{
				Log.i(LOG_TAG, "setupBluetoothSocket() success");
				citwDevice.setSocket(sock);
	
				JSONObject jsonMessage = Cw_JSONMessageFactory.generateInitMessage();
				Cw_ReplyMessage replyMessage = Cw_BluetoothSocket.sendJSONMessage(citwDevice, jsonMessage);
				try 
				{
					Cw_JSONMessageFactory.translateInitJSON(citwDevice, new JSONObject(replyMessage.getMessage()));
				} catch (JSONException e) 
				{
					Log.i(LOG_TAG, "Failed to translate INIT message JSON");
				}
				citwDevice.setConnected(true);
			}
		}
	}
	
	/*
	 *   Disconnects from a Citizenwater device
	 */
	public static void disconnectFromDevice(Context context, Cw_Device device)
	{
		if (DEBUG_NO_BLUETOOTH)
		{
			Toast.makeText(context, "DEBUG_NO_BLUETOOTH enabled. Disconnected.", Toast.LENGTH_LONG).show();
			device.setConnected(false);
		}
		else
		{
			BluetoothSocket sock = device.getSocket();
			if (sock != null)
			{
				try {
					sock.close();
				} catch (IOException e) {
					Log.i(LOG_TAG, "Failed to close socket");
				}
			}
			Toast.makeText(context, "Disconnected.", Toast.LENGTH_LONG).show();
			device.setSocket(null);
			device.setConnected(false);
		}
	}
	
	/*
	 *   Get data on device and transfer it to phone.
	 */
	public static void syncDatabase(Cw_Device device, Cw_DatabaseHelper db) throws JSONException
	{
		// TODO: Decide how to transfer data between the Edison and the phone

		if (DEBUG_NO_BLUETOOTH)
		{
			// Toast.makeText(context, "DEBUG_NO_BLUETOOTH enabled. This has no effect.", Toast.LENGTH_LONG).show();
		}
		else
		{
			long time_from_db = db.getLastestTime(device.toString()); //TODO fix getLatestTime function
			String time = Long.toString(time_from_db);
			Cw_ReplyMessage replyMessage;
			JSONArray table;
			
			int deviceID = db.getDeviceID(device.getDeviceName());
			
			Cw_BluetoothSocket.writeToSocket(device.getSocket(), 'D',time);		
			JSONObject jsonMessage;
			
			do {
				replyMessage = Cw_BluetoothSocket.readFromSocket(device.getSocket());	
				Log.i(LOG_TAG, "Reply message" +replyMessage.getMessage());
				
				jsonMessage = new JSONObject(replyMessage.getMessage());
				Log.i(LOG_TAG, "JSON object" +jsonMessage.toString());	
				
					table = jsonMessage.getJSONArray("table");
					if(table == null) return; 
					for(int i=0; i<table.length(); i++)
					{
						JSONObject row = table.getJSONObject(i);
						int ReadingID= row.getInt("ReadingID"); 
						int Type = 0 ; 
						int ReadingTypesID = row.getInt("ReadingTypeID");; 
						int Time = row.getInt("Time");
						double Value = row.getDouble("Value");
						
						db.insertIntoReadings(ReadingID, Type, ReadingTypesID, Time, Value, deviceID);// TODO fix insert function
					}
			} while(!(jsonMessage.getString("message").equals("end")));
			
		}

	}
	
	/*
	 *  Take measurements from all sensors on the device immediately
	 */
	public static void takeMeasurementsFromDevice(Context context, Cw_Device device)
	{
		if (DEBUG_NO_BLUETOOTH)
		{
			Toast.makeText(context, "DEBUG_NO_BLUETOOTH enabled. This has no effect.", Toast.LENGTH_LONG).show();
		}
		else
		{
			JSONObject jsonMessage = Cw_JSONMessageFactory.generateTakeReadingsMessage(device.getSensorList());
			Cw_JSONBluetoothTask jsonBluetoothTask = new Cw_JSONBluetoothTask(context, 
																			         device,
																			         jsonMessage);
			jsonBluetoothTask.execute();
		}
	}
	
	/*
	 *  Sends a message to the device updating the schedule
	 */
	public static void handleScheduleUpdate(Context context, Cw_Device device, List<Cw_DaySchedule> schedule)
	{
		if (DEBUG_NO_BLUETOOTH)
		{
			Toast.makeText(context, "DEBUG_NO_BLUETOOTH enabled. This has no effect.", Toast.LENGTH_LONG).show();
		}
		else
		{
			JSONObject jsonMessage = Cw_JSONMessageFactory.generateScheduleMessage(schedule);
			Cw_JSONBluetoothTask jsonBluetoothTask = new Cw_JSONBluetoothTask(context, device, jsonMessage);
			jsonBluetoothTask.execute();			
		}
	}
	
	/*
	 *  Sends a calibration message to the given sensor
	 */
	public static void sendCalibrationMessage(Context context, Cw_Device device, Cw_SensorDevice sensor, String calibrationType, char paramType, String param)
	{
		if (DEBUG_NO_BLUETOOTH)
		{
			Toast.makeText(context, "DEBUG_NO_BLUETOOTH enabled. This has no effect.", Toast.LENGTH_LONG).show();
		}
		else
		{
			JSONObject jsonMessage = Cw_JSONMessageFactory.generateCalibrationMessage(sensor, calibrationType, paramType, param);
			Cw_JSONBluetoothTask jsonBluetoothTask = new Cw_JSONBluetoothTask(context, 
			         device,
			         jsonMessage);
			jsonBluetoothTask.execute();
		}
	}
}
