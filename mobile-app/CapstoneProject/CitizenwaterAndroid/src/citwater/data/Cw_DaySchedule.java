package citwater.data;

import java.util.Date;

import android.util.Log;

public class Cw_DaySchedule {

	private String day;
	private Date startTime = new Date();
	private Date endTime   = new Date();
	private Date intervalTime = new Date();

	public Cw_DaySchedule(String day)
	{
		setDay(day);
	}
	
	public Cw_DaySchedule(String day, String start, String end, String interval)
	{
		setDay(day);
		
		// Start Time
		String[] startHourMin = start.split(":");
		
		if (startHourMin.length != 2)
		{
			Log.w("Cw_DaySchedule", "Invalid start time string detected");
		}
		else
		{
			try
			{
				startTime.setHours(Integer.parseInt(startHourMin[0]));
				startTime.setMinutes(Integer.parseInt(startHourMin[1]));
			}
			catch (NumberFormatException e)
			{
				Log.w("Cw_DaySchedule", "Invalid start time string detected");
			}
		}
		
		// End Time
		String[] endHourMin = end.split(":");
		
		if (endHourMin.length != 2)
		{
			Log.w("Cw_DaySchedule", "Invalid end time string detected");
		}
		else
		{
			try
			{
				endTime.setHours(Integer.parseInt(endHourMin[0])); 
				endTime.setMinutes(Integer.parseInt(endHourMin[1]));
			}
			catch (NumberFormatException e)
			{
				Log.w("Cw_DaySchedule", "Invalid end time string detected");
			}
		}
		
		// Interval
		try 
		{
			int interval_time = Integer.parseInt(interval);
			intervalTime.setHours(interval_time / 60);
			intervalTime.setMinutes(interval_time % 60);
		}
		catch (NumberFormatException e)
		{
			Log.w("Cw_DaySchedule", "Invalid interval string detected");
		}
	}
	
	public String getDay()
	{
		return day;
	}
	
	public int getDayAsInt()
	{
		String[] days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
		for (int i = 0; i < days.length; i++) 
		{
			if (day.equals(days[i]))
				return i;  
		}
		return 0; // Invalid
	}
	public void setDay(String newDay)
	{
		day = newDay;
	}
	
	public int getEndTimeMinutes() {
		return endTime.getMinutes();
	}
	public void setEndTimeMinutes(int endTimeMinutes) {
		endTime.setMinutes(endTimeMinutes);
	}
	public int getStartTimeHours() {
		return startTime.getHours();
	}
	public void setStartTimeHours(int startTimeHours) {
		startTime.setHours(startTimeHours);
	}
	public int getIntervalHours() {
		return intervalTime.getHours();
	}
	public void setIntervalHours(int intervalHours) {
		intervalTime.setHours(intervalHours);
	}
	public int getStartTimeMinutes() {
		return startTime.getMinutes();
	}
	public void setStartTimeMinutes(int startTimeMinutes) {
		startTime.setMinutes(startTimeMinutes);
	}
	public int getEndTimeHours() {
		return endTime.getHours();
	}
	public void setEndTimeHours(int endTimeHours) {
		endTime.setHours(endTimeHours);
	}
	public int getIntervalMinutes() {
		return intervalTime.getMinutes();
	}
	public void setIntervalMinutes(int intervalMinutes) {
		intervalTime.setMinutes(intervalMinutes);
	}

	public Date getIntervalAsDate() {
		return intervalTime;
	}

	public Date getEndTimeAsDate() {
		return endTime;
	}

	public Date getStartTimeAsDate() {
		return startTime;
	}

	public boolean isScheduleValid()
	{
		if ((getStartTimeHours() > getEndTimeHours()))
		{
			return false;
		}
		else if ((getStartTimeHours() == getEndTimeHours()) && (getStartTimeMinutes() >= getEndTimeMinutes()))
		{
			return false;
		}
		else if (getIntervalMinutes() < 1 || getIntervalMinutes() > 59)
		{
			return false;
		}
		else if (getDayAsInt() == -1)
		{
			return false;
		}

		return true;
	}
}
