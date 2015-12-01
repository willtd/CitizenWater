package citwater.gui;

import java.io.IOException;
import java.util.Set;

import org.json.JSONException;
import org.json.JSONObject;

import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_HalfHalf;
import simpleui.modifiers.v3.M_InfoText;
import simpleui.modifiers.v3.M_SeperatorLine;
import simpleui.modifiers.v3.M_Toolbar;
import citwater.app.Cw_JSONMessageFactory;
import citwater.bluetooth.Cw_Bluetooth;
import citwater.data.Cw_Device;
import citwater.data.Cw_ReplyMessage;


@SuppressLint("NewApi")
public class Cw_PairedList extends M_Container {
	private static final long serialVersionUID = -4167143444236963585L;
	private static final String LOG_TAG = "Cw_PairedList";
	
	private Cw_Device citwaterDevice = null;
	private Cw_PairedList container = this;

	private static class InitBluetoothTask extends AsyncTask<Void, Void, Boolean>
	{
		private Cw_Device citwDevice;
		private BluetoothDevice blueDevice;
		private Context context;
		private ProgressDialog progressDialog;
		
		public InitBluetoothTask(final Context context,
								 final Cw_Device citwDevice, 
								 final BluetoothDevice blueDevice)
		{
			this.context = context;
			this.citwDevice = citwDevice;
			this.blueDevice = blueDevice;
		}
		
		@Override
		protected void onPreExecute()
		{
			super.onPreExecute();
			
			progressDialog = new ProgressDialog(context);
			progressDialog.setIndeterminate(true);
			progressDialog.setMessage("Connecting. Please wait...");
			Toast.makeText(context, "Be patient while connection is setup...", Toast.LENGTH_LONG).show();
	        progressDialog.setCancelable(false);
	        progressDialog.setCanceledOnTouchOutside(false);
	        progressDialog.show();
		}
		
		@SuppressLint("NewApi")
		@Override
		protected Boolean doInBackground(Void... params) {
			Cw_Bluetooth.connectToDevice(context, citwDevice, blueDevice);
			return citwDevice.isConnected();
		}
	
		@Override
		protected void onPostExecute(Boolean result) // Run in UI thread
		{
			super.onPostExecute(result);
			progressDialog.dismiss();
			
			if (result == Boolean.valueOf(true))
				Toast.makeText(context, "Connection success!", Toast.LENGTH_LONG).show();
			else
				Toast.makeText(context, "Connection failure", Toast.LENGTH_LONG).show();
		}
	}


	private class ConnectButton extends M_Button {

		private BluetoothDevice dev;
		public ConnectButton(String buttonText, BluetoothDevice dev) {
			super(buttonText);
			this.dev = dev;
		}

		@Override
		public void onClick(Context context, Button button) {
			if (!citwaterDevice.isConnected())
			{
				InitBluetoothTask initBluetoothTask = new InitBluetoothTask(context, 
																            citwaterDevice,
																            dev);
				initBluetoothTask.execute();
			}
			else
			{
				Toast.makeText(context, "Already connected to a device. If you believe you are not connected to a device, try pressing the disconnect button and try again.", Toast.LENGTH_LONG).show();
			}
		}
		
	}
	
	public Cw_PairedList(Set<BluetoothDevice> devices, Cw_Device e)
	{
		citwaterDevice = e;
		add(new M_Toolbar("Paired List"));

		if (devices.isEmpty())
		{
			add(new M_InfoText("You are not currently paired with any devices. Please pair with a device first!"));
			add(M_SeperatorLine.newMaterialOne());
		}
		else
		{
			add(new M_InfoText("Please select the Citizenwater device that you wish to pair with."));
			add(new M_InfoText("You must currently be in range of the device and the device must be on."));
			
			add(M_SeperatorLine.newMaterialOne());
			
			for (BluetoothDevice device : devices)
			{
				M_Container temp = new M_Container();
				temp.add(new M_HalfHalf(new M_InfoText(device.getName()), new M_InfoText(device.getAddress())));
				temp.add(new ConnectButton("Connect", device));
				add(temp);
				add(M_SeperatorLine.newMaterialOne());	
			}
	
			add(M_SeperatorLine.newMaterialOne());
		}
		
		add(new M_BackButton("Back", container));

	}
}
