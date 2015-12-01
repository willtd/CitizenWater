/**
 *   An object holding all information necessary to represent a Citizenwater Device,   
 *   including Bluetooth information
 */

package citwater.data;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothSocket;
import util.Log;


public class Cw_Device {

	// Any necessary bluetooth information.
	
	private static final String LOG_TAG = "Cw_Device";
	private UUID uuid;
	private boolean connected = false;
	private BluetoothSocket socket;
	
	public BluetoothSocket getSocket() {
		return socket;
	}

	public void setSocket(BluetoothSocket mSocket) {
		this.socket = mSocket;
	}

	// Device name
	private String deviceName = "N/A";
	
	// Schedule
	private List<Cw_DaySchedule>  dayList    = new ArrayList<Cw_DaySchedule>();
	
	// List of Sensors
	private List<Cw_SensorDevice> sensorList = new ArrayList<Cw_SensorDevice>();
	

	public boolean isConnected() {
		return connected;
	}

	public void setConnected(boolean connected) {
		this.connected = connected;
	}

	public String getDeviceName() {
		return deviceName;
	}

	public void setDeviceName(String deviceName) {
		this.deviceName = deviceName;
	}
	
	public Cw_Device()
	{
		setUuid(UUID.fromString("00001101-0000-1000-8000-00805f9b34fb")); // usual uuid for serial ports
	}

	public List<Cw_DaySchedule> getDayList() {
		return dayList;
	}

	public void setDayList(List<Cw_DaySchedule> dayList) {
		this.dayList = dayList;
	}

	public List<Cw_SensorDevice> getSensorList() {
		return sensorList;
	}

	public void setSensorList(List<Cw_SensorDevice> sensorList) {
		this.sensorList = sensorList;
	}

	public UUID getUuid() {
		return uuid;
	}

	public void setUuid(UUID uuid) {
		this.uuid = uuid;
	}

}
