package citwater.data;

public class Cw_ReplyMessage{
	private char inputType;
	private String inputMessage;

	public Cw_ReplyMessage(char type, String message) {
		inputType = type;
		inputMessage = message;	
	}
	
	public char getInputType()
	{
		return inputType;
	}
	
	public String getMessage()
	{
		return inputMessage;
	}
	
	public void setInputType( char type)
	{
		inputType = type;
	}
	
	public void setMessage(String message)
	{
		inputMessage = message;
	}	
	
	public String toString()
	{
		return inputType+" "+inputMessage;
	}
}
