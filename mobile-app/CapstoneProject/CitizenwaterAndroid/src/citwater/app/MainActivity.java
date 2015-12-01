/**
 *   
 * 
 *    Some of the files used in this project are modified/adapted from the SimpleUI project.
 *    makers of https://github.com/bitstars/SimpleUi
 *    
 *    This file has been adapted from simpleui.examples.MainActivity
 *    
 */


package citwater.app;

import simpleui.SimpleUI;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Caption;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_InfoText;
import simpleui.modifiers.v3.M_SeperatorLine;
import simpleui.modifiers.v3.M_Toolbar;

import java.io.IOException;
import java.sql.SQLException;

import org.json.JSONException;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;
import citwater.bluetooth.Cw_Bluetooth;
import citwater.data.Cw_DatabaseHelper;
//import citwater.data.Cw_DatabaseManager;
import citwater.data.Cw_Device;
import citwater.gui.Cw_SyncToCloud;
import citwater.gui.M_BluetoothConnect;
import citwater.gui.M_ScheduleReadings;
import citwater.gui.M_SensorStatus;
import citwater.gui.M_ViewStoredData;


public class MainActivity extends Activity {
	private static final String LOG_TAG = MainActivity.class.getSimpleName();
	
	private final M_Container mainContainer = new M_Container();
	private final Cw_Device device = new Cw_Device();
	private static Cw_DatabaseHelper db;
	
	private final int BLUETOOTH_ENABLE_CODE = 144;
	
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
    	if (requestCode == BLUETOOTH_ENABLE_CODE)
    	{
    		if (resultCode == RESULT_OK)
    		{
    			BluetoothAdapter adapter =  BluetoothAdapter.getDefaultAdapter();
    			if (adapter != null)
    				SimpleUI.showUi(this, new M_BluetoothConnect(device, adapter));
    			else
    				Toast.makeText(this, "Error. Could not find Bluetooth adapter", Toast.LENGTH_LONG).show();
    		}
    		else
    		{
    			Toast.makeText(this, "You must enable Bluetooth to use this feature.", Toast.LENGTH_LONG).show();
    		}
    	}
    }
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		//Cw_DatabaseHelper dbHelper = new Cw_DatabaseHelper();
		//Cw_DatabaseManager.initializeInstance(new Cw_DatabaseHelper(getApplicationContext()));
		db = new Cw_DatabaseHelper(this);
		
		try {
			db.copyDB();
		} catch (IOException e) {
			Log.i(LOG_TAG, "copyDB() failed");
		}
		
		mainContainer.add(new M_Toolbar("Citizenwater Mobile"));
		mainContainer.add(new M_InfoText("ASU Capstone 2015"));
		mainContainer.add(M_SeperatorLine.newMaterialOne());
		
		addNondeviceOptions(mainContainer);
		addDeviceOptions(mainContainer, device);

		mainContainer.save();
		
		setContentView(mainContainer.getView(this));

	}
	

	private void addNondeviceOptions(M_Container c) {
		M_Container innerContainer = new M_Container();
		c.add(innerContainer);
		innerContainer.add(new M_Caption("Non-device Options"));
		
		addViewDataButton(innerContainer); /* Visualize/Export data. Needs: Database access */
		addSyncWithCloudButton(innerContainer);
		innerContainer.add(M_SeperatorLine.newMaterialOne());
	}
	
	private void addDeviceOptions(M_Container c,Cw_Device e) {
		M_Container innerContainer = new M_Container();
		c.add(innerContainer);
		innerContainer.add(new M_Caption("Device Options"));

		addConnectToBluetoothButton(innerContainer, e);		    /* Connects to Device. Needs: Communication with Device */
		addGetDataFromDeviceButton(innerContainer, e); 		    /* Forces a reading. Needs: Communication with Device */
		addSetupReadingTimesButton(innerContainer, e); 		    /* Sets up reading schedule. Needs: Communication with Device */
		addSyncDatabaseWithSensorButton(innerContainer, e);  /* Syncs database. Needs: Database access, Communication with device */
		addQuerySensorButton(innerContainer, e);				/* Gives status of sensors. Needs: Communication with Device */
		innerContainer.add(M_SeperatorLine.newMaterialOne());
	}
	
	private void addViewDataButton(M_Container c) {
		// Pressing this button opens a new window displaying graphs of any information stored on the phone
		c.add(new M_Button("View stored data") {
			@Override
			public void onClick(Context context, Button b) {
				if (Cw_ButtonTimer.isClickAllowed())
				{
					SimpleUI.showUi(context, new M_ViewStoredData());
				}
			}

		});
	}

	private void addSyncWithCloudButton(M_Container c)
	{
		c.add(new M_Button("Sync with Cloud") {
			@Override
			public void onClick(Context context, Button b) {
				if (Cw_ButtonTimer.isClickAllowed())
				{
					SimpleUI.showUi(context, new Cw_SyncToCloud());
				}
			}

		});
	}
	private void addConnectToBluetoothButton(M_Container c, final Cw_Device e) {
		c.add(new M_Button("Connect to Device") {
			@Override
			public void onClick(Context context, Button button) {
				if (Cw_ButtonTimer.isClickAllowed())
				{
					if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
					    Toast.makeText(context, "Bluetooth Low Energy is not supported by this device", Toast.LENGTH_SHORT).show();
					}
					else
					{
						BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
						
						if (bluetoothAdapter != null)
						{
							if (!bluetoothAdapter.isEnabled())
							{
								Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
								startActivityForResult(enableBtIntent, BLUETOOTH_ENABLE_CODE);
							}
							else
							{
								SimpleUI.showUi(context, new M_BluetoothConnect(e, bluetoothAdapter));
							}
						}
						else
						{
						    Toast.makeText(context, "Bluetooth is not supported by this device", Toast.LENGTH_SHORT).show();
						}
					}
				}
			}
		});
	}

	private void addGetDataFromDeviceButton(M_Container c, final Cw_Device e) {
		c.add(new M_Button("Take measurements") {
			@Override
			public void onClick(Context context, Button button) {
				if (device.isConnected())
				{
					if (Cw_ButtonTimer.isClickAllowed())
					{
						Cw_Bluetooth.takeMeasurementsFromDevice(context, device);
					}
				}
				else
				{
					Toast.makeText(context, "You must be connected to a device to do that!", Toast.LENGTH_LONG).show();
				}
			}
		});
	}
	
	private void addSetupReadingTimesButton(M_Container c, final Cw_Device e) {
		c.add(new M_Button("Schedule readings") {
			@Override
			public void onClick(Context context, Button button) {
				if (device.isConnected())
				{
					if (Cw_ButtonTimer.isClickAllowed())
					{
						SimpleUI.showUi(context, new M_ScheduleReadings(e));
					}
				}
				else
				{
					Toast.makeText(context, "You must be connected to a device to do that!", Toast.LENGTH_LONG).show();
				}
			}
		});
	}
	
	private void addSyncDatabaseWithSensorButton(M_Container c, Cw_Device e) {
		c.add(new M_Button("Sync database") {
			@Override
			public void onClick(Context context, Button button) {
				if (device.isConnected())
				{
					if (Cw_ButtonTimer.isClickAllowed())
					{
						SQLiteDatabase database;
						try {
							database = db.openDB();
							Cw_Bluetooth.syncDatabase(device, db);
							db.close();
						} catch (SQLException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						} catch (JSONException e) {
							Log.i(LOG_TAG, "Error syncing the database");
						}
					}
				}
				else
				{
					Toast.makeText(context, "You must be connected to a device to do that!", Toast.LENGTH_LONG).show();
				}
			}
		});
	}
	
	private void addQuerySensorButton(M_Container c, final Cw_Device e) {
		c.add(new M_Button("Sensor Status/Calibration") {
			@Override
			public void onClick(Context context, Button button) {
				if (device.isConnected())
				{
					if (Cw_ButtonTimer.isClickAllowed())
						SimpleUI.showUi(context, new M_SensorStatus(e));
				}
				else
				{
					Toast.makeText(context, "You must be connected to a device to do that!", Toast.LENGTH_LONG).show();
				}
			}
		});
	}
	
  public static synchronized Cw_DatabaseHelper getInstance() {
	  return db;
  }
}
