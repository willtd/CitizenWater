/**
 *  M_ExportData
 *  
 *  
 */

package citwater.gui;

import java.io.File;
import java.sql.SQLException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.concurrent.ExecutionException;

import citwater.app.MainActivity;
import citwater.data.Cw_DataManipulation;
import citwater.data.Cw_DatabaseHelper;
//import citwater.data.Cw_DatabaseManager;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Checkbox;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_InfoText;
import simpleui.modifiers.v3.M_SeperatorLine;
import simpleui.modifiers.v3.M_Toolbar;
import android.R;
import android.app.ProgressDialog;
import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.Toast;

public class M_ExportData extends M_Container {
	private static final String LOG_TAG = M_ExportData.class.getSimpleName();
	
	private static final long serialVersionUID = -2348451646148817018L;
	
	private M_ExportData container = this;
	private boolean emailFlag = false;
	
	private M_Checkbox emailFlagCheckbox = new M_Checkbox("Email") {
		@Override
		public boolean loadVar() { return emailFlag; }

		@Override
		public boolean save(boolean arg0) { return true; }

		@Override
		public boolean onCheckedChanged(Context context, CheckBox e, boolean isChecked) {
			if (isChecked) 
				{ emailFlag = true; }
			else 
				{ emailFlag = false; }
			return true;
		}
	};

	private File createSpreadsheetFile()
	{
		Calendar c = Calendar.getInstance(); 
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss");
		format.format(c.getTime());
		String fileName = ("cw-export-test" + format.format(c.getTime()) + ".csv");
		File file = Cw_DataManipulation.createExternalFile(fileName);
		return file;
	}

	private class SaveToSpreadsheetTask extends AsyncTask<Void, Void, Boolean>
	{
		private File file;
		private ArrayList<String> readingTypesList;
		private ArrayList<String> devicesList;
		private Date start;
		private Date end;
		private Context context;
		private ProgressDialog progressDialog;
		
		public SaveToSpreadsheetTask(final Context context,
									 final File file,
									 final ArrayList<String> readingTypesList, 
									 final ArrayList<String> devicesList, 
									 final Date start, 
									 final Date end)
		{
			this.context = context;
			this.file = file;
			this.readingTypesList = readingTypesList;
			this.devicesList = devicesList;
			this.start = start;
			this.end = end;
		}
		
		@Override
		protected void onPreExecute()
		{
			super.onPreExecute();
			progressDialog = new ProgressDialog(context);
			progressDialog.setIndeterminate(true);
			progressDialog.setMessage("Saving. Please wait...");
	        progressDialog.setCancelable(false);
	        progressDialog.setCanceledOnTouchOutside(false);
	        progressDialog.show();
		}
		
		@Override
		protected Boolean doInBackground(Void... params) {
			if (file != null)
			{
				if (Cw_DataManipulation.saveDataToSpreadsheet(file, devicesList, readingTypesList, start, end))
				{	
					return Boolean.valueOf(true);
				}
			}

			return Boolean.valueOf(false);
		}
	
		@Override
		protected void onPostExecute(Boolean result) // Run in UI thread
		{
			super.onPostExecute(result);
			progressDialog.dismiss();
			
			if (result == true)
			{
				if (emailFlag)
				{
					if (Cw_DataManipulation.sendFileViaEmail(context, file))
					{
						Toast.makeText(context, "Success!", Toast.LENGTH_LONG).show();
						return;
					}
					else
					{
						Log.i(LOG_TAG, "sendFileViaEmail() failed");
						Toast.makeText(context, "Failed to send email\n", Toast.LENGTH_LONG).show();
						return;
					}
				}
				else
				{
					Toast.makeText(context, "Successfully saved file\n", Toast.LENGTH_LONG).show();
				}
			}
		}
	}
	
	public M_ExportData(final ArrayList<String> readingTypesList, 
						final ArrayList<String> devicesList, 
						final Date start, 
						final Date end) {

		add(new M_Toolbar("Export stored data"));

		add(new M_InfoText(R.drawable.ic_dialog_info,
				"Data will be saved to local storage as a CSV file. " + 
				"If the checkbox below is marked, after saving the file another application will open allowing"
				+ " you to email it."));

		add(emailFlagCheckbox);
		
		add(new M_Button("Save") {
			@Override
			public void onClick(Context context, Button button) {
				File file = createSpreadsheetFile();
				
				if (file == null)
				{
					Log.i(LOG_TAG, "createSpreadsheetFile() failed");
					Toast.makeText(context, "Error occurred in creating a file. There may be no external storage " +
											"available on your device.", Toast.LENGTH_LONG).show();
					return;
				}
				
				SaveToSpreadsheetTask spreadsheetTask = new SaveToSpreadsheetTask(context,
																			      file, 
																			      readingTypesList,
																			      devicesList,
																			      start,
																			      end);
				spreadsheetTask.execute();
		}});
		
		add(M_SeperatorLine.newMaterialOne(null));
		add(M_SeperatorLine.newMaterialOne(null));

		add(new M_BackButton("Back", container));
		
	}
}