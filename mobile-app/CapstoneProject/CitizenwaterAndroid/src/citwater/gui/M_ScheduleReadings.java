package citwater.gui;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import citwater.app.Cw_ButtonTimer;
import citwater.bluetooth.Cw_Bluetooth;
import citwater.data.Cw_DaySchedule;
import citwater.data.Cw_DayScheduleComparator;
import citwater.data.Cw_Device;
import citwater.data.Cw_ReplyMessage;
import citwater.gui.Cw_ListWrapperV2.WrapperItem;
import simpleui.modifiers.ModifierInterface;
import simpleui.modifiers.v1.M_ListWrapperV2;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_HalfHalf;
import simpleui.modifiers.v3.M_SeperatorLine;
import simpleui.modifiers.v3.M_Toolbar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;

public class M_ScheduleReadings extends M_Container {

	private Cw_Device device = null;
	private M_ScheduleReadings container = this;
	private List<Cw_DaySchedule> schedule = null;
	
	private DialogInterface.OnClickListener backButtonListener = new DialogInterface.OnClickListener() {
	    @Override
	    public void onClick(DialogInterface dialog, int type) {
	    	if (type == DialogInterface.BUTTON_POSITIVE)
	    	{
				Collections.sort(schedule, new Cw_DayScheduleComparator());
				((Activity) container.getContext()).finish(); // close the window showing this container:

	    	}
	    	else if (type == DialogInterface.BUTTON_NEGATIVE)
	    	{
	    		// Do nothing except close dialog.
	    		dialog.dismiss();
	    	}
	    }
	};
	
	public M_ScheduleReadings(Cw_Device e)
	{
		super();
		device = e;

		add(new M_Toolbar("Schedule readings"));

		schedule = e.getDayList();
		
		add(new Cw_ModifiableListWrapperV2<Cw_DaySchedule>(schedule, "Add New") {
			@Override
			public ModifierInterface getModifierForItem(Context c, Cw_DaySchedule item, int posInList) {
				return new M_ScheduleModifier(item);
			}

			@Override
			public boolean onRemoveRequest(Cw_DaySchedule item) {
				Log.i("M_Schedule", "Removing..");
				schedule.remove(item);
				return true;
			}

			@Override
			public boolean onAddRequest(Cw_DaySchedule item) {
				Log.i("M_Schedule", "Adding..");
				schedule.add(item);
				return true;
			}

			@Override
			public Cw_DaySchedule getNewItemInstance(Context c,
					int posOfNewItemInList) {
				return new Cw_DaySchedule("Monday", "10:00", "10:00", "5" /* minutes */);
			}
		});


		add(M_SeperatorLine.newMaterialOne());
		
		add(new M_HalfHalf(
				new M_Button("Update") {
							@Override
							public void onClick(Context context, Button button) {
								boolean errorInSchedule = false;
								if (Cw_ButtonTimer.isClickAllowed())
								{
									container.save(); // Updates 'schedule' with most up-to-date values from the UI.

									for (Cw_DaySchedule sched : schedule)
									{
										if (!sched.isScheduleValid())
										{		
											errorInSchedule = true;
											break;
										}
									}
									if (!errorInSchedule)
									{
										 Cw_Bluetooth.handleScheduleUpdate(context, device, device.getDayList());
									}
									else
									{
										Toast.makeText(context, "The reading schedule contains an error!", Toast.LENGTH_LONG).show();
									}
								}
							}
				},			
				new M_Button("Back") {
					public void onClick(Context context, Button button)
					{
						if (Cw_ButtonTimer.isClickAllowed())
						{
							AlertDialog.Builder builder = new AlertDialog.Builder(context);
							builder.setMessage("Are you sure? Any changes since your last successful update will be lost.");
							builder.setPositiveButton("Yes", backButtonListener);
							builder.setNegativeButton("No", backButtonListener);
							builder.show();
						}

					}
				}));
	}
}
							
