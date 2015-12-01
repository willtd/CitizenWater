package citwater.gui;

import citwater.app.Cw_ButtonTimer;
import citwater.data.Cw_Device;
import citwater.data.Cw_SensorDevice;
import android.content.Context;
import android.widget.Button;
import simpleui.SimpleUI;
import simpleui.modifiers.v3.M_Button;

public class M_SensorDeviceButton extends M_Button {

	Cw_SensorDevice sensor;
	Cw_Device device;
	
	public M_SensorDeviceButton(String buttonText) {
		super(buttonText);
	}

	public M_SensorDeviceButton(String buttonText, Cw_Device device, Cw_SensorDevice sensor)
	{
		this(buttonText);
		this.sensor = sensor;
		this.device = device;
	}
	
	@Override
	public void onClick(Context c, Button b) {
		if (Cw_ButtonTimer.isClickAllowed())
		{
			SimpleUI.showUi(c, new M_SensorDeviceInfo(device, sensor));
		}
	}
}
