package citwater.data;

import java.util.List;

public class Cw_SensorDevice {

	private String sensorType = "Empty";
	
	private String sensorBrand = "Empty";
	
	private String sensorDescription = "Empty";
	
	private List<Cw_SensorCalibrationOption> sensorCalibrationOpts;

	public Cw_SensorDevice(String type, String brand, String desc, List<Cw_SensorCalibrationOption> calOpts)
	{
		sensorType = type;
		sensorBrand = brand;
		sensorDescription = desc;
		setSensorCalibrationOpts(calOpts);
	}
	public String getSensorType()
	{
		return sensorType;
	}
	
	public String getSensorBrand()
	{
		return sensorBrand;
	}
	
	public String getSensorDescription()
	{
		return sensorDescription;
	}
	
	public void setSensorType(String newSensorType)
	{
		sensorType = newSensorType;
	}
	
	public void setSensorBrand(String newSensorBrand)
	{
		sensorBrand = newSensorBrand;
	}
	
	public List<Cw_SensorCalibrationOption> getSensorCalibrationOpts() {
		return sensorCalibrationOpts;
	}
	public void setSensorCalibrationOpts(List<Cw_SensorCalibrationOption> sensorCalibrationOpts) {
		this.sensorCalibrationOpts = sensorCalibrationOpts;
	}
}
