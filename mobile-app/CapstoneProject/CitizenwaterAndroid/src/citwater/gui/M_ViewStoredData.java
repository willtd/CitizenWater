package citwater.gui;

import java.sql.SQLException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;

import org.achartengine.model.XYMultipleSeriesDataset;

import citwater.app.Cw_ButtonTimer;
import citwater.app.MainActivity;
import citwater.data.Cw_DataManipulation;
import citwater.data.Cw_DatabaseHelper;
//import citwater.data.Cw_DatabaseManager;
import simpleui.SimpleUI;
import simpleui.modifiers.ModifierInterface;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Checkbox;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_DateModifier;
import simpleui.modifiers.v3.M_DurationInput;
import simpleui.modifiers.v3.M_HalfHalf;
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

public class M_ViewStoredData extends M_Container  {
	private M_ViewStoredData container = this;

	private Cw_ListWrapperV2<String> chartsList = null;
	
	private ArrayList<String> devicesList = new ArrayList<String>();
	private ArrayList<String> devicesSelectedList = new ArrayList<String>();
	
	private ArrayList<String> readingTypesList = new ArrayList<String>();
	private ArrayList<String> readingTypesSelectedList = new ArrayList<String>();

	private Date start = new Date(115, 1, 1); 
	private Date end   = new Date(115, 11, 1);
	
	private static final String LOG_TAG = "M_ViewStoredData";
	
	@Override
	public boolean save() {
		return true;
	}

	private Cw_DateModifier startDate;
	private Cw_DateModifier endDate;
	private Cw_DurationInput startTime;
	private Cw_DurationInput endTime;

	private void setupDateAndTimeModifiers()
	{
		startDate = new Cw_DateModifier() {
			@Override
			public java.lang.String getTextFor(Date d, String dateString) {
				DateFormat df = new SimpleDateFormat("MMM d, yyyy");
				String date = df.format(d);
				return date;
			}

			@Override
			public Date loadDate() {
				return start;
			}

			@Override
			public boolean save(Date arg0) {
				return true;
			}
			
		};
		
		startTime = new Cw_DurationInput("hh:mm a") {
			@Override
			public Integer loadDurationInMinutes() {
				return start.getHours() * 60 + start.getMinutes();
			}

			@Override
			public boolean saveDurationInMinutes(int arg0) {
				return true;
			}
			
		};
		
		endDate = new Cw_DateModifier() {
			@Override
			public java.lang.String getTextFor(Date d, String dateString) {
				DateFormat df = new SimpleDateFormat("MMM d, yyyy");
				String date = df.format(d);
				return date;
			}

			@Override
			public Date loadDate() {
				return end;
			}

			@Override
			public boolean save(Date arg0) {
				return true;
			}
			
		};
		
		endTime = new Cw_DurationInput("hh:mm a") {
			@Override
			public Integer loadDurationInMinutes() {
				return end.getHours() * 60 + end.getMinutes();
			}

			@Override
			public boolean saveDurationInMinutes(int arg0) {
				return true;
			}
			
		};
		
	}
	
	private void updateStartEndDates()
	{
		Date newStart = startDate.getCurrentlyDisplayedDate();
		start.setDate(newStart.getDate());
		start.setMonth(newStart.getMonth());
		start.setYear(newStart.getYear());
		
		int startTimeInMinutes = startTime.getDurationInMinutes();
		start.setHours(startTimeInMinutes / 60);
		start.setMinutes(startTimeInMinutes % 60);
		
		Date newEnd = endDate.getCurrentlyDisplayedDate();
		end.setDate(newEnd.getDate());
		end.setMonth(newEnd.getMonth());
		end.setYear(newEnd.getYear());
		
		int endTimeInMinutes = endTime.getDurationInMinutes();
		end.setHours(endTimeInMinutes / 60);
		end.setMinutes(endTimeInMinutes % 60);
	}
	
	private class CheckBoxList<String> extends Cw_ListWrapperV2<String> {

		private Collection<String> fullList;
		private Collection<String> selectedList;
		public CheckBoxList(Collection<String> fullList,
							Collection<String> selectedList) {
			super(fullList, null);
			// TODO Auto-generated constructor stub
			this.fullList = fullList;
			this.selectedList = selectedList;
		}

		@Override
		public ModifierInterface getModifierForItem(Context c, final String item, int posInList) {
			M_Checkbox checkbox = null;
			checkbox = new M_Checkbox((java.lang.String) item) {
					@Override
					public boolean loadVar() {
						return selectedList.contains(item);
					}
	
					@Override
					public boolean save(boolean arg0) {
						return true;
					}

					@Override
					public boolean onCheckedChanged(Context context, CheckBox e, boolean isChecked) {
						if (isChecked)
						{
							selectedList.add(item); // TODO: Duplicate checks?
						}
						else
						{
							selectedList.remove(item);
						}
						return true;
					}
			};
			return checkbox;
		}
		
		@Override
		public String getNewItemInstance(Context c, int posOfNewItemInList) {
			return null;
		}
	}

	private class SetupChartDataTask extends AsyncTask<Void, Void, Boolean>
	{
		private ArrayList<String> deviceList;
		private ArrayList<String> readingTypes;
		private ArrayList<XYMultipleSeriesDataset> datasets;
		private Date start;
		private Date end;
		private Context context;
		private ProgressDialog progressDialog;
		
		public SetupChartDataTask(final Context context,
								  final ArrayList<String> deviceList, 
								  final ArrayList<String> readingTypes,
								  final Date start, 
								  final Date end)
		{
			this.context = context;
			this.readingTypes = readingTypes;
			this.deviceList = deviceList;
			this.start = start;
			this.end = end;
			datasets = new ArrayList<XYMultipleSeriesDataset>();
		}
		
		@Override
		protected void onPreExecute()
		{
			super.onPreExecute();
			
			progressDialog = new ProgressDialog(context);
			progressDialog.setIndeterminate(true);
			progressDialog.setMessage("Loading chart data. Please wait...");
	        progressDialog.setCancelable(false);
	        progressDialog.setCanceledOnTouchOutside(false);
	        progressDialog.show();
		}
		
		@Override
		protected Boolean doInBackground(Void... params) {
			Cw_DatabaseHelper db = MainActivity.getInstance();
			for (String readingType : readingTypes)
			{
				XYMultipleSeriesDataset dataset = new XYMultipleSeriesDataset();
				
				Cw_DataManipulation.setupChartSeries(deviceList, readingType, dataset, start, end);	
				datasets.add(dataset);
				
			}
			return Boolean.valueOf(true);
		}
	
		@Override
		protected void onPostExecute(Boolean result) // Run in UI thread
		{
			super.onPostExecute(result);
			progressDialog.dismiss();
			
			if (start.getTime() < end.getTime())
			{
				SimpleUI.showUi(context, 
								new M_LineChartListContainer(readingTypes, 
														 	 deviceList, 
														 	 datasets,
														 	 start, 
														  end));
			}
		}
	}

	public M_ViewStoredData()
	{
		super();

		setFillCompleteScreen(true);
		
		add(new M_Toolbar("View data"));

		devicesList = Cw_DataManipulation.getListOfDevicesInDb();
		readingTypesList = Cw_DataManipulation.getListOfReadingTypesInDb();

		devicesSelectedList.addAll(devicesList);
		readingTypesSelectedList.addAll(readingTypesList);
		
		add(M_SeperatorLine.newMaterialOne(null));

		add(new M_InfoText(R.drawable.ic_dialog_info,
				"Select the devices for which data should be displayed."));
		add(new CheckBoxList<String> (devicesList, devicesSelectedList));
		
		add(M_SeperatorLine.newMaterialOne(null));

		add(new M_InfoText(R.drawable.ic_dialog_info,
				"Select the types of measurements which should be displayed."));
		add(new CheckBoxList<String> (readingTypesList, readingTypesSelectedList));

		add(M_SeperatorLine.newMaterialOne(null));

		add(new M_InfoText(R.drawable.ic_dialog_info,
				"Select a time frame. All recorded data within this time frame will be displayed."));

		add(M_SeperatorLine.newMaterialOne(null));
		
		setupDateAndTimeModifiers();
		
		add(new M_InfoText("Starting Date"));
		add(new M_HalfHalf(startDate, startTime));
		
		add(M_SeperatorLine.newMaterialOne(null));
		
		add(new M_InfoText("Ending Date"));
		add(new M_HalfHalf(endDate, endTime));

		add(M_SeperatorLine.newMaterialOne(null));

		add(new M_Button("Show Data!") {
			@Override
			public void onClick(Context context, Button arg1) {
				if (Cw_ButtonTimer.isClickAllowed())
				{
					if (!devicesSelectedList.isEmpty() && !readingTypesSelectedList.isEmpty())
					{
						updateStartEndDates();
						
						if (start.getTime() < end.getTime())
						{
							SetupChartDataTask setupChartTask = new SetupChartDataTask(context, 
																					   devicesSelectedList, 
																					   readingTypesSelectedList, 
																					   start, 
																					   end);
							setupChartTask.execute();
						}
						else
						{
							Toast.makeText(context, "The starting date should be before the ending date.", Toast.LENGTH_LONG).show();	
						}
					}
					else
					{
						Toast.makeText(context, "You can't graph nothing! At least one device and one measurement type must be selected.", Toast.LENGTH_LONG).show();
					}
				}
			}
		});

		add(new M_Button("Export Data!") {
			@Override
			public void onClick(Context context, Button arg1) {
				if (Cw_ButtonTimer.isClickAllowed())
				{
					if (!devicesSelectedList.isEmpty() && !readingTypesSelectedList.isEmpty())
					{
						updateStartEndDates();
						if (start.getTime() < end.getTime())
						{
							SimpleUI.showUi(context, 
									new M_ExportData(readingTypesSelectedList, 
													 devicesSelectedList, 
													 start, 
													 end));
						}
						else
						{
							Toast.makeText(context, "The starting date should be before the ending date.", Toast.LENGTH_LONG).show();
						}
					}
					else
					{
						Toast.makeText(context, "You can't export nothing! At least one device and one measurement type must be selected.", Toast.LENGTH_LONG).show();
					}
				}
			}
		});
		
		add(M_SeperatorLine.newMaterialOne(null));

		add(new M_BackButton("Back", container));
	}
}