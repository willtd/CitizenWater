package citwater.data;

import java.util.Comparator;

public class Cw_DayScheduleComparator implements Comparator<Cw_DaySchedule> {

	@Override
	public int compare(Cw_DaySchedule day1, Cw_DaySchedule day2) {
		if (day1.getDayAsInt() < day2.getDayAsInt())
		{
			return -1; 
		}
		else if (day1.getDayAsInt() > day2.getDayAsInt())
		{  
			return 1;
		}
		else
		{
			int startTime1 = day1.getStartTimeHours() * 60 + day1.getStartTimeMinutes();
			int startTime2 = day2.getStartTimeHours() * 60 + day2.getStartTimeMinutes();
			if (startTime1 < startTime2)
			{
				return -1;
			}
			else if (startTime1 > startTime2)
			{
				return 1;
			}
			else
			{
				int endTime1 = day1.getEndTimeHours() * 60 + day1.getEndTimeMinutes();
				int endTime2 = day2.getEndTimeHours() * 60 + day2.getEndTimeMinutes();
				
				if (endTime1 < endTime2)
				{
					return -1;
				}
				else if (endTime1 > endTime2)
				{
					return 1;
				}
				else
				{
					int intervalTime1 = day1.getIntervalHours() * 60 + day1.getIntervalMinutes();
					int intervalTime2 = day2.getIntervalHours() * 60 + day2.getIntervalMinutes();
					
					if (intervalTime1 < intervalTime2)
					{
						return -1;
					}
					else if (intervalTime1 > intervalTime2)
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}
			}
		}
	}

}
