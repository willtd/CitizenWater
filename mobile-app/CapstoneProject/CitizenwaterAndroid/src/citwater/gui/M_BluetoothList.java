package citwater.gui;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.LinkedHashSet;
import java.util.UUID;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.util.SparseArray;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import citwater.app.R;
import citwater.data.Cw_Device;
import simpleui.modifiers.ModifierInterface;
import simpleui.modifiers.v1.uiDecoration.UiDecoratable;
import simpleui.modifiers.v1.uiDecoration.UiDecorator;
import simpleui.modifiers.v3.M_HalfHalf;
import simpleui.modifiers.v3.M_Toolbar;
import simpleui.util.Log;

public class M_BluetoothList implements ModifierInterface, UiDecoratable {
	private BluetoothAdapter mBluetoothAdapter;
	private ListView listView;
    private ArrayAdapter<String> listAdapter;
    private ArrayList<String> activeDevices;
    private UiDecorator myDecorator;
    private SparseArray<BluetoothDevice> mDevices; // holds BT devices found
    private Cw_Device cw_device;
    
	public M_BluetoothList(Cw_Device e,  Activity mainActivity) {
		listView = (ListView)mainActivity.findViewById(R.id.mainListView);
		activeDevices = new ArrayList<String>();
		listAdapter = new ArrayAdapter<String>(mainActivity, R.layout.simplerow,activeDevices);
		cw_device = e;
		//listView.setAdapter(listAdapter);			
	}
	
	public void addUniqueDevice(BluetoothDevice foundDevice)
	{
		String deviceName =  foundDevice.getName(); //foundDevice.getName()+"\n" + foundDevice.getAddress();
		String deviceAddress = foundDevice.getAddress();
		boolean notUnique = false;
		
		if(deviceName == null) deviceName = "";
		
		for (int i = 0; i < listAdapter.getCount() && notUnique == false; i++) {
			notUnique = (deviceName+"\n"+deviceAddress).equals(listAdapter.getItem(i));
		}
		
		if(!notUnique)	{
			int deviceIndex;
			final String TAG = "Devices";
			
			listAdapter.add(deviceName+"\n"+deviceAddress);
			deviceIndex = listAdapter.getPosition(deviceName+"\n"+deviceAddress); // adds device to list adapter array list
			mDevices.put(deviceIndex, foundDevice); // adds device to bluetooth device array list
			//mDevices.setValueAt(deviceIndex, foundDevice);
			Log.d(TAG , "Device Address = " +mDevices.get(deviceIndex).getAddress());	
		}
	}
	
	public ListView getListView() {
		return listView;
	}
	
	@SuppressLint("NewApi")
	public void connectToDevice(BluetoothDevice device, Context context)
	{
		try {
			cw_device.setSocket(device.createInsecureRfcommSocketToServiceRecord(cw_device.getUuid()));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		
		try {
			cw_device.getSocket().connect();
			Toast.makeText(context, "Connection to "+device.getName()+"\n"+device.getAddress()+" Successful!", Toast.LENGTH_LONG).show();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
    private void initChartView(Context context, LinearLayout.LayoutParams p2)
    {
    	mDevices = new SparseArray<BluetoothDevice>();
    	listView = new ListView(context);
		listAdapter = new ArrayAdapter<String>(context, R.layout.simplerow, activeDevices);
    	listView.setAdapter(listAdapter);
    	listView.setLayoutParams(p2);
    	
    	
    	listView.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
				BluetoothDevice selectedDevice = mDevices.get(position);
				String deviceName = selectedDevice.getName();
				String deviceAddress = selectedDevice.getAddress();
				
				if(deviceName == null) deviceName = "";
			
				//TODO stop scanning when device is connected
				Toast.makeText(parent.getContext(), "Connecting to device: "+deviceName + "\n" + deviceAddress, Toast.LENGTH_LONG).show();		
				
				connectToDevice(selectedDevice, parent.getContext());
			}
		});
    }
    
	public boolean assignNewDecorator(UiDecorator decorator) {
		myDecorator = decorator;
		return true;
	}
	
	public View getView(Context context) {
		LinearLayout l = new LinearLayout(context);
		l.setGravity(Gravity.CENTER_VERTICAL);
	
		LinearLayout.LayoutParams p2 = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, 700, //TODO check to see if there is way to set for whatever device
				0);

		initChartView(context, p2);
		l.addView(listView);
		
		l.setPadding(DEFAULT_PADDING, DEFAULT_PADDING, DEFAULT_PADDING,
				DEFAULT_PADDING);

		if (myDecorator != null) {
			int level = myDecorator.getCurrentLevel();
			myDecorator.decorate(context, listView, level + 1,
					UiDecorator.TYPE_EDIT_TEXT);
		}
		return l;
}

	public boolean save() {
		return true;
	}
}
