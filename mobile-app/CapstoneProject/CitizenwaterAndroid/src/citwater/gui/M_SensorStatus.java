package citwater.gui;

import citwater.data.Cw_Device;
import citwater.data.Cw_SensorDevice;
import android.content.Context;
import simpleui.modifiers.ModifierInterface;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_HalfHalf;
import simpleui.modifiers.v3.M_InfoText;
import simpleui.modifiers.v3.M_SeperatorLine;
import simpleui.modifiers.v3.M_Toolbar;

public class M_SensorStatus extends M_Container {
	private static final long serialVersionUID = -4167143444236963585L;
	
	private Cw_Device device = null;
	private M_SensorStatus container = this;

	public M_SensorStatus(Cw_Device e)
	{
		device = e;
		add(new M_Toolbar("Sensor Status/Calibration"));

		add(new M_InfoText("Information about the sensors below was found on your device. If this information is inaccurate, the sensors.json file on your device must be corrected."));
		
		for (Cw_SensorDevice sensor : device.getSensorList())
		{
			M_Container temporarySensorContainer = new M_Container();
			temporarySensorContainer.add(new M_HalfHalf(new M_InfoText(sensor.getSensorType()), new M_SensorDeviceButton("Info", device, sensor)));
			add(temporarySensorContainer);
		
		}

		add(M_SeperatorLine.newMaterialOne());
		
		add(new M_BackButton("Back", container));

	}
}
