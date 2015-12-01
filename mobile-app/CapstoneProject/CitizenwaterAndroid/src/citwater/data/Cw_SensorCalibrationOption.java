package citwater.data;

public class Cw_SensorCalibrationOption {

	private String calibrationType; // ie., CAL_FACTORY_RESET
	private String description;
	private char  paramType;
	private int   integerLowerBound;
	private int   integerUpperBound;
	private double floatLowerBound;
	private double floatUpperBound;
	
	/*
	 *   Type == N (no parameters)
	 */
	public Cw_SensorCalibrationOption(String type, String description)
	{
		this.setParamType('N'); 
		this.setCalibrationType(type);
		this.setDescription(description);
	}
	
	/*
	 *   Type == I (integer parameters)
	 */
	public Cw_SensorCalibrationOption(String type, String description, int lowerBound, int upperBound)
	{
		this.setParamType('I');
		this.setCalibrationType(type);
		this.setDescription(description);
		this.setIntegerLowerBound(lowerBound);
		this.setIntegerUpperBound(upperBound);
	}
	
	/*
	 *   Type == D (double parameters)
	 */
	public Cw_SensorCalibrationOption(String type, String description, double lowerBound, double upperBound)
	{
		this.setParamType('D');
		this.setCalibrationType(type);
		this.setDescription(description);
		this.setFloatLowerBound(lowerBound);
		this.setFloatUpperBound(upperBound);
	}

	public String getCalibrationType() {
		return calibrationType;
	}

	public void setCalibrationType(String type) {
		this.calibrationType = type;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public int getIntegerUpperBound() {
		return integerUpperBound;
	}

	public void setIntegerUpperBound(int integerUpperBound) {
		this.integerUpperBound = integerUpperBound;
	}

	public int getIntegerLowerBound() {
		return integerLowerBound;
	}

	public void setIntegerLowerBound(int integerLowerBound) {
		this.integerLowerBound = integerLowerBound;
	}

	public double getFloatLowerBound() {
		return floatLowerBound;
	}

	public void setFloatLowerBound(double floatLowerBound) {
		this.floatLowerBound = floatLowerBound;
	}

	public double getFloatUpperBound() {
		return floatUpperBound;
	}

	public void setFloatUpperBound(double floatUpperBound) {
		this.floatUpperBound = floatUpperBound;
	}

	public char getParamType() {
		return paramType;
	}

	public void setParamType(char paramType) {
		this.paramType = paramType;
	}
}
