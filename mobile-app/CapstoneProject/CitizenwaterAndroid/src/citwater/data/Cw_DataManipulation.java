package citwater.data;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.Random;

import org.achartengine.model.TimeSeries;
import org.achartengine.model.XYMultipleSeriesDataset;

import citwater.app.MainActivity;
import simpleui.SimpleUI;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

public class Cw_DataManipulation {

	private static final String LOG_TAG = "Cw_DataManipulation";
	
	/* Checks if external storage is available for read and write */
	public static boolean isExternalStorageWritable() {
	    String state = Environment.getExternalStorageState();
	    if (Environment.MEDIA_MOUNTED.equals(state)) {
	        return true;
	    }
	    return false;
	}

	/* Checks if external storage is available to at least read */
	public static boolean isExternalStorageReadable() {
	    String state = Environment.getExternalStorageState();
	    if (Environment.MEDIA_MOUNTED.equals(state) ||
	        Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
	        return true;
	    }
	    return false;
	}
	
	/*
	 * Creates a file on external storage. Returns null on failure.
	 * The function may fail if an IO Exception occurs or the file already existed. 
	 */
	public static File createExternalFile(String fileName)
	{
		File file = null;
		if (isExternalStorageWritable())
		{
			// Use external storage
			file = new File(Environment.getExternalStoragePublicDirectory(
		            Environment.DIRECTORY_PICTURES), fileName);
		    try {
				if (!file.createNewFile())
					file = null;
			}
			catch (IOException e) {
				Log.w("Cw_DataManipulation", "Error occurred in writing file!");
				file = null;
			}
		}
		else
		{
			Log.w("Cw_DataManipulation", "External storage is not writeable!");
		}
		return file;
	}
	
	/*
	 * Creates a file on internal storage. 
	 */
	/*
	public static File createInternalFile(Context context, String fileName)
	{
        File file = new File(context.getFilesDir(), "spreadsheet.csv");
        try {
			if (!file.createNewFile()) {
				
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	*/
	
	/*
	 *  Returns a list of every type of measurement (conductivity, pH, etc..) found in the database.
	 * 
	 */
	public static ArrayList<String> getListOfReadingTypesInDb()
	{
		ArrayList<String> listOfReadingTypes = new ArrayList<String>();
		Cw_DatabaseHelper dbHelper = MainActivity.getInstance();
		SQLiteDatabase database = null;
		
		try 
		{
			database = dbHelper.openDB();
		}
		catch (SQLException e)
		{
			database = null;
		}
		
		if (database == null)
		{
			return listOfReadingTypes;
		}
		

		Cursor res = database.rawQuery("select Name from ReadingTypes", null);
		while(res.moveToNext()){
			listOfReadingTypes.add(res.getString(0));
		}
		
		dbHelper.close();
		return listOfReadingTypes;
	}
	

	/*
	 *  Returns a list of every device found in the database. 
	 */
	public static ArrayList<String> getListOfDevicesInDb()
	{
		ArrayList<String> devices = new ArrayList<String>();
		
		Cw_DatabaseHelper dbHelper = MainActivity.getInstance();
		SQLiteDatabase database = null;
		
		try 
		{
			database = dbHelper.openDB();
		}
		catch (SQLException e)
		{
			database = null;
		}
		
		if (database == null)
		{
			return devices;
		}
		
		Cursor res = database.rawQuery("select DeviceName from Devices", null);
		while(res.moveToNext()){
			devices.add(res.getString(0));
		}
		
		dbHelper.close();
		return devices;
	}
	
	public static String getDeviceKeyInDb( String deviceName)
	{
		Cw_DatabaseHelper dbHelper = MainActivity.getInstance();
		SQLiteDatabase database = null;
		
		try 
		{
			database = dbHelper.openDB();
		}
		catch (SQLException e)
		{
			database = null;
		}
		
		if (database == null)
		{
			return "No ID";
		}
		
		String str = "";
		Cursor res = database.rawQuery("select DeviceCloudKey from Devices where DeviceName = '" + deviceName + "'", null);
		if (res.moveToNext()){
			str += res.getString(0);
		}
		android.util.Log.d("getDeviceKeyInDb:",str);
		
		dbHelper.close();
		return str;
	}
	
	public static String getDeviceIdInDb(String deviceName)
	{
		Cw_DatabaseHelper dbHelper = MainActivity.getInstance();
		SQLiteDatabase database = null;
		
		try 
		{
			database = dbHelper.openDB();
		}
		catch (SQLException e)
		{
			database = null;
		}
		
		if (database == null)
		{
			return "No ID";
		}
		
		String str = "";
		String query = "select * from Devices where DeviceName = '" + deviceName + "'";
		Cursor res = database.rawQuery(query, null);
		if (res.moveToNext()){
			str += res.getString(0);
		}
		
	    dbHelper.close();
	     
		return str;
	}
	
	public static void setDeviceKeyInDb(String deviceName, String key)
	{
		Cw_DatabaseHelper dbHelper = MainActivity.getInstance();
		SQLiteDatabase database = null;
		
		try 
		{
			database = dbHelper.openDB();
		}
		catch (SQLException e)
		{
			database = null;
		}
		
		if (database == null)
		{
			return;
		}
		
		android.util.Log.d("setDeviceKeyInDb1",key);
		ContentValues cv = new ContentValues();
		cv.put("DeviceCloudKey",key);
		database.update("Devices", cv, "DeviceName = '" + deviceName + "'", null);
	    android.util.Log.d("setDeviceKeyInDb2",key);
	     
	    dbHelper.close();
	}
	
	public static void setDeviceIdInDb(String deviceName, String id)
	{
		Cw_DatabaseHelper dbHelper = MainActivity.getInstance();
		SQLiteDatabase database = null;
		
		try 
		{
			database = dbHelper.openDB();
		}
		catch (SQLException e)
		{
			database = null;
		}
		
		if (database == null)
		{
			return;
		}
		
		android.util.Log.d("setDeviceIdInDb1",id);
		ContentValues cv = new ContentValues();
		cv.put("DeviceCloudID",id);
		database.update("Devices", cv, "DeviceName = '" + deviceName + "'", null);
	   //  database.rawQuery("UPDATE Devices SET DeviceCloudID = '"+ id + "' WHERE DeviceName = '" + deviceName + "'", null);
	     android.util.Log.d("setDeviceIdInDb2",id);
	     
	     dbHelper.close();
	}
	
	/*
	 *  Returns a cursor which can be used in a while loop:
	 *  		while (cursor.moveToNext()) { ... }
	 *  Each row should contain the following information:
	 *   	[(YEAR) (MONTH) (DAY OF MONTH)] [(HOURS) (MINS) (SECS)] (VALUE)
	 *  The rows should be in chronological order.
	 */
	public static Cursor getCursorWithData(SQLiteDatabase database,
										   ArrayList<String> devices, 
										   String readingTypes, 
										   Date start, 
										   Date end)
	{
		Cursor cursor = null;
		int type = 0;
		long start1 = start.getTime();
		long end1 = end.getTime();
		Cursor res = database.rawQuery("select ReadingTypeID from ReadingTypes where Name = '" + readingTypes + "'", null);
		while (res.moveToNext()) {
			type = res.getInt(0);
		}
		 android.util.Log.d("Database",readingTypes + "  = " +type);
		 //String query = "select Time, DeviceID, Value from Readings where Time > " + start1 + " AND Time < " + end1 + " AND ReadingTypeID = " + type;
		 String query = "select Time, DeviceID, Value from Readings where ReadingTypeID = " + type;
		 cursor = database.rawQuery(query, null);

		 return cursor;
	}
	
	/*
	 *  Returns a cursor which can be used in a while loop:
	 *  		while (cursor.moveToNext()) { ... }
	 *  Each row should contain the following information:
 	 *  [DEVICE NAME] [DATE] [TIME] [READING TYPE 1] [READING TYPE 2] [READING TYPE 3] ... [READING TYPE N]
	 *  The rows should be sorted, in order of precedence, by (1) the device name and (2) chronological order.
	 */
	private static Cursor getCursorWithData(SQLiteDatabase database,
											ArrayList<String> devices, 
										    Date start, 
										    Date end)
	{
		Cursor cursor = null;
		int type = 0;
		long start1 = start.getTime();
		long end1 = end.getTime();
		// String query = "select DeviceID, Time, ReadingTypeID from Readings where Time > " + start1 + " AND Time < " + end1;
		String query = "select DeviceID, Time, ReadingTypeID, Value from Readings";
		cursor = database.rawQuery(query, null);
	    android.util.Log.d("Column Names",cursor.getColumnName(0));
	    android.util.Log.d("Column Names",cursor.getColumnName(1));
	    android.util.Log.d("Column Names",cursor.getColumnName(2));

		return cursor;
	}
	
	/* 
	 * Time returned in Unix time nanoseconds
	 */
	public static void getDataSinceTime(Long lastTime, String readingType, ArrayList<Long> times, ArrayList<Float> values) 
	{
		/*
		if (lastTime == null)
		{
			lastTime = System.currentTimeMillis() * 1000 * 1000;
		}
		for (int i = 0; i < 10; i++)
		{
			times.add(lastTime + i*1000*1000*1000);
			values.add((float) 5.0);
		}
		*/
		
		Cw_DatabaseHelper dbHelper = MainActivity.getInstance();
		SQLiteDatabase database = null;
		
		try 
		{
			database = dbHelper.openDB();
		}
		catch (SQLException e)
		{
			database = null;
		}
		
		if (database == null)
		{
			return;
		}
		
		// TODO: this is ugly, rewrite...
		Cursor cursor = database.rawQuery("SELECT Time, Value FROM Readings WHERE Time > " + 
										  lastTime/(1000*1000*1000) + 
										  " AND ReadingTypeID IN (SELECT ReadingTypeID from ReadingTypes Where Name = '" + readingType + "')", null);
		
		Log.i(LOG_TAG, "DATAPOINT START");
		
		while ( cursor.moveToNext() ) 
		{
			times.add(cursor.getLong(0) * 1000 * 1000 * 1000 /* nanoseconds */);
    		values.add(cursor.getFloat(1));
    		Log.i(LOG_TAG, "Datapoint: " + cursor.getLong(0) * 1000 * 1000 * 1000 + " " + cursor.getFloat(1));
    	}
		
		dbHelper.close();
		
	}
	
	
	/*
	 *  Sets up the Dataset for a TimeChart.
	 *  One 'series' of data is required per device.
	 *  
	 *  Readings are graphed if they satisfy the following conditions:
	 *   (1) The reading was taken in the given time interval
	 *   (2) The reading was taken by some device in the given list.
	 *   (3) The reading was of type 'readingType'.
	 */
	public static void setupChartSeries(ArrayList<String> devices, 
			                            String readingType, 
			                            XYMultipleSeriesDataset mSeries, 
			                            Date start, 
			                            Date end) {
		Cw_DatabaseHelper dbHelper = MainActivity.getInstance();
		SQLiteDatabase database = null;
		
		try 
		{
			database = dbHelper.openDB();
		}
		catch (SQLException e)
		{
			database = null;
		}
		
		if (database == null)
		{
			return;
		}
		
		Cursor cursor = getCursorWithData(database, devices, readingType, start, end);
		// TODO: Implement this
		for(int i = 0; i < devices.size();i++){
			TimeSeries series = new TimeSeries(devices.get(i));
			Cursor res = database.rawQuery("select DeviceID from Devices where DeviceName = '" + devices.get(i) + "'", null);
			int id = 0;
			if(res.moveToNext()){
				id = res.getInt(0);
			}
			mSeries.addSeries(series);
			while (cursor.moveToNext()) {
				android.util.Log.d("Cursor",String.valueOf(cursor.getInt(1)));
				if(cursor.getInt(1) == id){
					Date time = new Date(cursor.getLong(0)*1000);
					Double value = Double.parseDouble(cursor.getString(2));
					android.util.Log.d("Device "+id,"Value = "+ value);
					series.add(time, value);
				}
			}
			if(cursor.moveToFirst()){
				android.util.Log.d("Move Cursor","true");
			}
		}
		Cursor res = database.rawQuery("select DeviceID from Devices where DeviceName = 'Device 2'", null);
		if (res.moveToNext()){
			android.util.Log.d("getDeviceIdInDb:",res.getString(0));
		}
     	
		dbHelper.close();
	}

	/*
	 * Saves data to the given file.
	 * 
	 *  Readings are saved to the file if they satisfy the following conditions:
	 *   (1) The reading was taken in the given time interval
	 *   (2) The reading was taken by some device in the given list.
	 *   (3) The reading was of some type on the ArrayList readingTypes.
	 */
	@SuppressWarnings("deprecation")
	public static boolean saveDataToSpreadsheet(File file, 
												ArrayList<String> devices, 
												ArrayList<String> readingTypes, 
												Date start, 
												Date end)
	{
		// Modified from : http://stackoverflow.com/questions/14509026/export-sqlite-into-csv
		Cw_DatabaseHelper dbHelper = MainActivity.getInstance();
		SQLiteDatabase database = null;
		
		try 
		{
			database = dbHelper.openDB();
		}
		catch (SQLException e)
		{
			database = null;
		}
		
		if (database == null)
		{
			return false;
		}
		
		boolean ret = false;
		int i = 0;
		String csvHeader = "";
		//
		//String csvValues = "";
		String tmp ="";

	    
	    try { // Just for testing.
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	    
		// Columns in spreadsheet:
		// [DEVICE NAME] [DATE] [TIME] [READING TYPE 1] [READING TYPE 2] [READING TYPE 3] ... [READING TYPE N]
		
		csvHeader += "Device Name";
		csvHeader += ", ReadingTypes";
		csvHeader += ", Date, Time";
		csvHeader += ", Value";

	    csvHeader += "\n";
	    try {
	        FileWriter fileWriter = new FileWriter(file);
	        BufferedWriter out = new BufferedWriter(fileWriter);
	        Cursor cursor = getCursorWithData(database, devices, start, end);
	        if (cursor != null) {
	            out.write(csvHeader);
	            for (int j = 0; j < devices.size(); j++) {
	            	Cursor res = database.rawQuery("select DeviceID from Devices where DeviceName = '" + devices.get(i) + "'", null);
	            	int id = 0;
	    			String type = "";
	    			if(res.moveToNext()){
	    				id = res.getInt(0);
	    			}
	            	while (cursor.moveToNext()) {
	            		// DeviceID, Time, ReadingTypeID, Value
	 	            	if(id == cursor.getInt(0)) {
	 		    			Date time = new Date(cursor.getLong(1)*1000);
	 	            		Cursor res1 = database.rawQuery("select Name from ReadingTypes where ReadingTypeID = " + cursor.getInt(2), null);
	 	            		android.util.Log.d("Reading Types ID = ",String.valueOf(cursor.getInt(2)));
	 	            		if(res1.moveToNext()){
	 		    				type = res1.getString(0);
	 		    			}
	 	            		
	 	            		String csvValues = devices.get(i);
	 	            		
	 	            		csvValues += ", ";
	 		    			csvValues += type;
	 		    			csvValues += ", ";

		 	            	tmp = "" + (time.getMonth() + 1) + "/" + time.getDate() + "/" + (time.getYear() + 1900) +", ";
		 	            	csvValues += tmp; // Date should be column 1
		 	            	tmp = "" + time.getHours() +":" + time.getMinutes() + ":" + time.getSeconds() +", ";
		 	            	csvValues += tmp; // Time should be column 2
		 	            	csvValues += cursor.getString(3);
		 	            	csvValues += "\n";
		 	            	out.write(csvValues);
	 	            	}
	 	            }
	            }
	            cursor.close();
	        }
	        out.close();
	        ret = true;
	    } catch (IOException e) {
	        ret = false;
	    }

	    dbHelper.close();
	    
	    return ret;
	}
	
	/*
	 *  Prompts the user to select an email application for sending the given file.
	 */
	public static boolean sendFileViaEmail(Context context, File file)
	{
		// Send email 
		Intent i = new Intent(Intent.ACTION_SEND);
		i.setType("message/rfc822");
		i.putExtra(Intent.EXTRA_EMAIL  , new String[]{"recipient@example.com"});
		i.putExtra(Intent.EXTRA_SUBJECT, "subject of email");
		i.putExtra(Intent.EXTRA_TEXT   , "body of email");
	    i.putExtra(Intent.EXTRA_STREAM,  Uri.fromFile(file));

		try {
		    SimpleUI.startActivity(context, Intent.createChooser(i, "Send mail..."));
		} catch (android.content.ActivityNotFoundException ex) {
			Toast.makeText(context, "There are no email clients installed.",
					Toast.LENGTH_SHORT).show();
		}

		return true;
	}
}
