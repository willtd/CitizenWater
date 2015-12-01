package citwater.gui;

import java.io.IOException;

import citwater.app.Cw_ButtonTimer;
import citwater.bluetooth.Cw_Bluetooth;
import citwater.data.Cw_Device;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;
import simpleui.SimpleUI;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_InfoText;
import simpleui.modifiers.v3.M_SeperatorLine;
import simpleui.modifiers.v3.M_Toolbar;

@SuppressLint("NewApi")
public class M_BluetoothConnect extends M_Container {

	protected static final String LOG_TAG = "M_BluetoothConnect";
	private Cw_Device device = null;
	private M_BluetoothConnect container = this;
	private BluetoothAdapter mBluetoothAdapter;

	private M_Button pairButton = new M_Button("Pair with new Device")
	{
		@Override
		public void onClick(Context c, Button b) {
			if (Cw_ButtonTimer.isClickAllowed()) {
				Intent settingsIntent = new Intent(android.provider.Settings.ACTION_BLUETOOTH_SETTINGS);
				getContext().startActivity(settingsIntent);
			}
		}
	};
	
	private M_Button listPairedButton = new M_Button("List paired device(s)") 
	{
		@Override
		public void onClick(Context c, Button b) {
			SimpleUI.showUi(c, new Cw_PairedList(mBluetoothAdapter.getBondedDevices(), device));
		}	
	};
	
	private M_Button disconnectButton = new M_Button("Disconnect")
	{
		@Override
		public void onClick(Context c, Button b) {
			Cw_Bluetooth.disconnectFromDevice(c, device);
		}
	};

	public M_BluetoothConnect(Cw_Device e, BluetoothAdapter mBluetoothAdapter) {	
		device = e;
		this.mBluetoothAdapter = mBluetoothAdapter;
		
		add(new M_Toolbar("Connect to a device via Bluetooth"));
		
		add(new M_InfoText("Pair with a citizenwater device using the \" Pair with New Device \" button."));
		add(new M_InfoText("The passkey is fixed to be 1234."));
		add(new M_InfoText("After pairing, connect to the device by selecting it in \"List Paired Device(s)\""));
		add(M_SeperatorLine.newMaterialOne());
		
		add(pairButton);
		add(listPairedButton);
		add(disconnectButton);
		add(new M_BackButton("Cancel", container));
		add(M_SeperatorLine.newMaterialOne());
	}
}
