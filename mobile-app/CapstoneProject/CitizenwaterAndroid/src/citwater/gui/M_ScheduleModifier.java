package citwater.gui;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import citwater.data.Cw_DaySchedule;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_HalfHalf;
import simpleui.modifiers.v3.M_InfoText;
import simpleui.modifiers.v3.M_IntModifier;
import simpleui.modifiers.v3.M_Spinner;

@SuppressWarnings("serial")
public class M_ScheduleModifier extends M_Container {

	Cw_DaySchedule schedule;

	M_Spinner listOfDaysSpinner;
	Cw_DurationInput startTimeInput;
	Cw_DurationInput endTimeInput;
	M_IntModifier    intervalInput;
	 
	public M_ScheduleModifier(final Cw_DaySchedule sched)
	{
		super();
		schedule = sched;
		
		listOfDaysSpinner = new M_Spinner() {
			SpinnerItem[] spinnerItems = {new SpinnerItem(0, "Monday"),
										  new SpinnerItem(1, "Tuesday"),
										  new SpinnerItem(2, "Wednesday"),
										  new SpinnerItem(3, "Thursday"),
										  new SpinnerItem(4, "Friday"),
										  new SpinnerItem(5, "Saturday"),
										  new SpinnerItem(6, "Sunday")};
			
			List<SpinnerItem> list = new ArrayList<SpinnerItem>();
			
			@Override
			public void onUserSelectedNewItem(Context context, long selectedItemId,
											  SpinnerItem selectedItem)
			{
				schedule.setDay(selectedItem.getText());
			}
					
			@Override
			public String getVarName() {
				return "";
			}

			@Override
			public List<SpinnerItem> loadListToDisplay() {
				list.clear();
				for (int i = 0; i < 7; i++)
				{
					list.add(spinnerItems[i]);
				}
				return list; 
			}

			@Override
			public int loadSelectedItemId() {
				return schedule.getDayAsInt();
			}

			@Override
			public boolean save(SpinnerItem item) {
				schedule.setDay(item.getText());
				return true;
			}
			
		};
		
		listOfDaysSpinner.setWeightOfDescription(999999);
		
		startTimeInput = new Cw_DurationInput("hh:mm a") {
			@Override
			public Integer loadDurationInMinutes() {
				return schedule.getStartTimeHours() * 60 + schedule.getStartTimeMinutes();
			}

			@Override
			public boolean saveDurationInMinutes(int durationInMinutes) {
				schedule.setStartTimeHours(durationInMinutes / 60);
				schedule.setStartTimeMinutes(durationInMinutes % 60);
				return true;
			}
		};
		
		endTimeInput = new Cw_DurationInput("hh:mm a") {
			@Override
			public Integer loadDurationInMinutes() {
				return schedule.getEndTimeHours() * 60 + schedule.getEndTimeMinutes();
			}

			@Override
			public boolean saveDurationInMinutes(int durationInMinutes) {
				schedule.setEndTimeHours(durationInMinutes / 60);
				schedule.setEndTimeMinutes(durationInMinutes % 60);
				return true;
			}
		};
		
		intervalInput = new M_IntModifier() {
			@Override
			public int loadInt() { return schedule.getIntervalMinutes(); }

			@Override
			public boolean saveInt(int val) {
				schedule.setIntervalHours(0);
				schedule.setIntervalMinutes(val);
				return true; 
			}

			@Override
			public String getVarName() { return ""; }
			
		};
		
		intervalInput.setMinimumAndMaximumValue(0, 59);

		M_Container startTimeEndTimeAndDayContainer = new M_Container();

        startTimeEndTimeAndDayContainer.add(listOfDaysSpinner);
        startTimeEndTimeAndDayContainer.add(startTimeInput);
        startTimeEndTimeAndDayContainer.add(endTimeInput);
        
        M_Container intervalContainer = new M_Container();
        intervalContainer.add(new M_InfoText("Interval (min)"));
        intervalContainer.add(intervalInput);
        
		M_HalfHalf h2 = new M_HalfHalf(startTimeEndTimeAndDayContainer, intervalContainer, (float) .45,  (float) .55);

		add(h2);
	}
}
