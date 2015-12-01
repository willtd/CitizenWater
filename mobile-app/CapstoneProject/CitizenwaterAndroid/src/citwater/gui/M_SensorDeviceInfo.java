package citwater.gui;

import java.util.ArrayList;
import java.util.List;

import citwater.app.Cw_ButtonTimer;
import citwater.bluetooth.Cw_Bluetooth;
import citwater.data.Cw_Device;
import citwater.data.Cw_SensorCalibrationOption;
import citwater.data.Cw_SensorDevice;
import android.content.Context;
import android.widget.Button;
import android.widget.Toast;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Caption;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_DoubleModifier;
import simpleui.modifiers.v3.M_HalfHalf;
import simpleui.modifiers.v3.M_InfoText;
import simpleui.modifiers.v3.M_IntModifier;
import simpleui.modifiers.v3.M_SeperatorLine;
import simpleui.modifiers.v3.M_Toolbar;

public class M_SensorDeviceInfo extends M_Container {

	private static final long serialVersionUID = -7507975447030655298L;
	
	private Cw_Device device = null;
	private Cw_SensorDevice sensor = null;
	private M_SensorDeviceInfo container = this;

	public M_SensorDeviceInfo(final Cw_Device device, final Cw_SensorDevice sensor)
	{
		this.device = device;
		this.sensor = sensor;
		add(new M_Toolbar(sensor.getSensorType()));

		add(M_SeperatorLine.newMaterialOne());
		add(new M_Caption("Information"));
		add(new M_InfoText("Brand: " + sensor.getSensorBrand()));
		add(new M_InfoText("Description: " + sensor.getSensorDescription()));
		
		add(M_SeperatorLine.newMaterialOne());
		add(new M_Caption("Calibration Options"));
		
		List<Cw_SensorCalibrationOption> calOptions = sensor.getSensorCalibrationOpts();
		
		for (Cw_SensorCalibrationOption calOpt : calOptions)
		{
			add(new Cw_CalContainer(device, sensor, calOpt));
			// add(M_SeperatorLine.newMaterialOne());
		}
		
		add(new M_BackButton("Back", container));
		
		add(M_SeperatorLine.newMaterialOne());
		
	}

}
