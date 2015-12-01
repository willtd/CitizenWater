package citwater.gui;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.achartengine.model.XYMultipleSeriesDataset;

import simpleui.SimpleUI;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_HalfHalf;
import simpleui.modifiers.v3.M_InfoText;
import simpleui.modifiers.v3.M_Toolbar;
import citwater.app.Cw_SensorCloud;
import citwater.app.MainActivity;
import citwater.data.Cw_DataManipulation;
import citwater.data.Cw_DatabaseHelper;
//import citwater.data.Cw_DatabaseManager;
import android.app.ProgressDialog;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;

public class Cw_SyncToCloud extends M_Container {

	private List<String> devicesList;
	private ArrayList<String> readingTypesList;

	private final static String LOG_TAG = "Cw_SyncToCloud";
	
	private class SyncWithCloudTask extends AsyncTask<Void, Void, Boolean>
	{
		private String  deviceKey;
		private String  deviceId;
		private Context context;
		private ProgressDialog progressDialog;

		
		public SyncWithCloudTask( final Context context,
								  final String  deviceId, 
								  final String  deviceKey,
								  final ArrayList<String> readingTypesList)
		{
			this.context = context;
			this.deviceKey = deviceKey;
			this.deviceId  = deviceId;
		}

		@Override
		protected void onPreExecute()
		{
			super.onPreExecute();
			
			Toast.makeText(context, "ID: " + deviceId + "\nKey: " + deviceKey, Toast.LENGTH_LONG).show();
			
			progressDialog = new ProgressDialog(context);
			progressDialog.setIndeterminate(true);
			progressDialog.setMessage("Synchronizing with cloud. Please wait...");
	        progressDialog.setCancelable(false);
	        progressDialog.setCanceledOnTouchOutside(false);
	        progressDialog.show();
		}

		
		private boolean uploadNewDataForReadingType(Cw_SensorCloud sensorCloud, String readingType)
		{
			int ret = 0;
			String truncatedReadingType = readingType.replaceAll("\\s+",""); // TODO: This is ugly, but sensorcloud does not allow spaces... Fix

			Log.i(LOG_TAG, "Now updating " + truncatedReadingType);
			
			ret = sensorCloud.addSensor(truncatedReadingType, "", "");
			Log.i(LOG_TAG, "AddSensor: " + ret);
			if (ret < 0){
				return Boolean.valueOf(false);
			}

			ret = sensorCloud.addChannel(truncatedReadingType);
			Log.i("Cw_SyncToCloud", "AddChannel: " + ret);
			if (ret < 0)
			{
				return Boolean.valueOf(false);
			}

			Long time = sensorCloud.getLatestDatastreamTime(truncatedReadingType);
			if (time == null)
			{
				time = Long.valueOf(0);
			}

			ArrayList<Long> times = new ArrayList<Long>();
			ArrayList<Float> values = new ArrayList<Float>();
			Cw_DataManipulation.getDataSinceTime(time, readingType, times, values);

			ret = sensorCloud.addTimeSeriesData(truncatedReadingType, times, values);
			Log.i("Cw_SyncToCloud", "AddTimeSeriesData: " + ret);
			
			if (ret != 201)
			{
				return Boolean.valueOf(false);
			}
			
			return Boolean.valueOf(true);
		}
		
		private Boolean authenticateCloud(Cw_SensorCloud sensorCloud)
		{
			int ret = sensorCloud.authenticate();
			Log.i(LOG_TAG, "Authenticate: " + ret);
			
			if (ret != 200)
			{
				return Boolean.valueOf(false);
			}
			return Boolean.valueOf(true);
		}
		
		@Override
		protected Boolean doInBackground(Void... params) {
			Cw_SensorCloud sensorCloud = new Cw_SensorCloud(deviceId, deviceKey);
			//new Cw_SensorCloud("OAPI00GF4V0YWSEP", "4a98ec590fbe3f6d1f655fda3ff4f4dc48cc5136ba67f3cf5297327c2df3fd20"); //

			if (authenticateCloud(sensorCloud).booleanValue() == false)
				return Boolean.valueOf(false);
			for (String readingType : readingTypesList)
			{
				if (uploadNewDataForReadingType(sensorCloud, readingType) == false)
					return Boolean.valueOf(false);
			}

			return Boolean.valueOf(true);
		}
	
		@Override
		protected void onPostExecute(Boolean result) // Run in UI thread
		{
			super.onPostExecute(result);
			progressDialog.dismiss();

			if (result.booleanValue() == true)
			{
				Toast.makeText(context, 
						       "Sync with cloud successful!", 
						       Toast.LENGTH_LONG).show();
			}
			else
			{
				Toast.makeText(context, 
						       "Sync with cloud unsuccessful. Perhaps your device ID or Key is wrong?", 
						       Toast.LENGTH_LONG).show();
			}
		}
	}
	
	public Cw_SyncToCloud()
	{
		devicesList = Cw_DataManipulation.getListOfDevicesInDb();
		readingTypesList = Cw_DataManipulation.getListOfReadingTypesInDb();
		
		add(new M_Toolbar("Sync with Cloud"));
		add(new M_InfoText("You must obtain a device ID and key from sensorcloud.com"));
		
		for (final String dev : devicesList)
		{
			M_Container tempDeviceContainer = new M_Container();
			
			tempDeviceContainer.add(new M_InfoText(dev));
			tempDeviceContainer.add(
					new M_HalfHalf(
							new M_Button("Set device ID/key") {
								@Override
								public void onClick(Context context, Button button) {
									SimpleUI.showUi(context, new Cw_DeviceKeyModifier(dev));
								}
							}, 
							new M_Button("Sync") {
								@Override
								public void onClick(Context context, Button button) {
									SyncWithCloudTask syncWithCloudTask = new SyncWithCloudTask(context, 
											Cw_DataManipulation.getDeviceIdInDb(dev),
											Cw_DataManipulation.getDeviceKeyInDb(dev),
											readingTypesList); 
									syncWithCloudTask.execute();

								}
								
							}));
			
			add(tempDeviceContainer);
		}
	}
}
