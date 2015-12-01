package citwater.gui;

import citwater.bluetooth.Cw_Bluetooth;
import citwater.data.Cw_Device;
import citwater.data.Cw_SensorCalibrationOption;
import citwater.data.Cw_SensorDevice;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_DoubleModifier;
import simpleui.modifiers.v3.M_InfoText;
import simpleui.modifiers.v3.M_IntModifier;
import android.content.Context;
import android.util.Log;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

public class Cw_CalContainer extends M_Container {

	Cw_Device device;
	Cw_SensorDevice sensor;

	public Cw_CalContainer(Cw_Device dev, Cw_SensorDevice sens, final Cw_SensorCalibrationOption calOpt)
	{
		final String name = calOpt.getCalibrationType();
		final String desc = calOpt.getDescription();
		final char type = calOpt.getParamType();
		final Cw_DoubleModifier floatInput;
		final M_IntModifier integerInput;
		
		this.device = dev;
		this.sensor = sens;

		Log.i("Cw_CalContainer", "Creating new cal container" + name + " " + type);
		add(new M_InfoText(desc));

		switch (type)
		{
		case 'N':
			add(new M_Button("Calibrate") {
				@Override
				public void onClick(Context context, Button arg1) {
					Toast.makeText(context, "Calibration request: " + name + " No params", Toast.LENGTH_LONG).show();;
					Cw_Bluetooth.sendCalibrationMessage(context, device, sensor, name, type, null);
				}
				
			});
			
			break;
		case 'D':
			final double floatLowerBound = calOpt.getFloatLowerBound();
			floatInput = new Cw_DoubleModifier() {
				@Override
				public double loadDouble() { return floatLowerBound; }

				@Override
				public boolean saveDouble(double arg0) { return true; }

				@Override
				public String getVarName() { return "";	}
				
				// We have no need to save anything that is typed in the double field
				@Override
				public boolean save(String newValue) { return true; }
			};

			add(floatInput);
			
			add(new M_Button("Calibrate") {
				@Override
				public void onClick(Context context, Button arg1) {
					Toast.makeText(context, "Calibration request: " + name + " Calibration type: " + type + " Calibration param: " +  floatInput.getCurrentTextValue(), Toast.LENGTH_LONG).show();;
					double currentValue = Double.parseDouble(floatInput.getCurrentTextValue());
					if (currentValue >= calOpt.getFloatLowerBound() && currentValue <= calOpt.getFloatUpperBound())
					{
						Cw_Bluetooth.sendCalibrationMessage(context, device, sensor, name, type, floatInput.getCurrentTextValue());
					}
					else
					{
						Toast.makeText(context,  "Invalid parameter! Please make sure value is in the specified range.", Toast.LENGTH_LONG).show();
					}
				}
			});
				
			break;
		case 'I':
			final int intLowerBound = calOpt.getIntegerLowerBound();
			integerInput = new M_IntModifier() {

				@Override
				public int loadInt() { return intLowerBound; }

				@Override
				public boolean saveInt(int arg0) { return true; }

				@Override
				public String getVarName() { return ""; }
				
				// We have no need to save anything that is typed in the integer field
				@Override
				public boolean save(String newValue) { return true; }
			};
			
			add(integerInput);
			
			add(new M_Button("Calibrate") {
				@Override
				public void onClick(Context context, Button arg1) {
					Toast.makeText(context, "Calibration request: " + name + " Calibration param: " + integerInput.getCurrentTextValue(), Toast.LENGTH_LONG).show();
					int currentValue = Integer.parseInt(integerInput.getCurrentTextValue());
					if (currentValue >= calOpt.getIntegerLowerBound() && currentValue <= calOpt.getIntegerUpperBound())
					{
						Cw_Bluetooth.sendCalibrationMessage(context, device, sensor, name, type, integerInput.getCurrentTextValue());
					}
					else
					{
						Toast.makeText(context,  "Invalid parameter! Please make sure value is in the specified range.", Toast.LENGTH_LONG).show();
					}
				}
			});
			break;
		}

	}
}
