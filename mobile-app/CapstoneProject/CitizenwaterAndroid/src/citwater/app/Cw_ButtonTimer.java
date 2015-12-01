package citwater.app;

import android.os.SystemClock;

public class Cw_ButtonTimer {
	
	private static long lastClickTime;
	private static final int MILLISECONDS_BETWEEN_CLICK = 500;
	
	public static synchronized boolean isClickAllowed()
	{
	    if (Math.abs(SystemClock.elapsedRealtime() - lastClickTime) < MILLISECONDS_BETWEEN_CLICK) {
	        return false;
	    }
	    
	    lastClickTime = SystemClock.elapsedRealtime();

	    return true;
	}
}
