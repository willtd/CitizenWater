package citwater.gui;

import citwater.app.MainActivity;
import citwater.data.Cw_DataManipulation;
import citwater.data.Cw_DatabaseHelper;
//import citwater.data.Cw_DatabaseManager;


import java.sql.SQLException;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_HalfHalf;
import simpleui.modifiers.v3.M_TextModifier;
import simpleui.modifiers.v3.M_Toolbar;

@SuppressWarnings("serial")
public class Cw_DeviceKeyModifier extends M_Container {

	private M_TextModifier idField;
	private M_TextModifier keyField;
	private String devName;
	private static final String LOG_TAG = "Cw_DeviceKeyModifier";
	
	public Cw_DeviceKeyModifier(String deviceName)
	{
		add(new M_Toolbar(deviceName));

		devName = deviceName;
		
		idField = new M_TextModifier() {

			@Override
			public String getVarName() {
				return "ID: ";
			}

			@Override
			public String load() {
				return Cw_DataManipulation.getDeviceIdInDb(devName);
			}

			@Override
			public boolean save(String arg0) {
				return true;
			}
			
		};
		
		keyField = new M_TextModifier() {
			@Override
			public String getVarName() {
				return "Key: ";
			}

			@Override
			public String load() {
				return Cw_DataManipulation.getDeviceKeyInDb(devName);
			}

			@Override
			public boolean save(String arg0) {
				return true;
			}
			
		};
		
		add(idField);
		add(keyField);
		
		add(new M_HalfHalf(new M_Button("Save!") {
							@Override
							public void onClick(Context context, Button button) {
								Cw_DataManipulation.setDeviceIdInDb(devName, idField.getCurrentTextValue());
								Cw_DataManipulation.setDeviceKeyInDb(devName, keyField.getCurrentTextValue());
								Toast.makeText(context, "New Device ID: " + idField.getCurrentTextValue() + "\n" + 
														"New Device Key: " + keyField.getCurrentTextValue(), Toast.LENGTH_LONG).show();
								
							}
						   }, 
						   new M_BackButton("Back", this)));
	}
}