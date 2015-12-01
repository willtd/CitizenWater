package citwater.bluetooth;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;

import org.json.JSONException;
import org.json.JSONObject;

import android.bluetooth.BluetoothSocket;
import android.util.Log;
import citwater.app.Cw_JSONMessageFactory;
import citwater.data.Cw_Device;
import citwater.data.Cw_ReplyMessage;

public class Cw_BluetoothSocket {
	
	private static final String LOG_TAG = "Cw_BluetoothSocket";
	
	private static int computeHeaderLength()
	{
		return 1 + 4; // +1 for type, +4 for length field
	}
	private static int computePacketLength(String str)
	{
		return str.length() + computeHeaderLength();
	} 
	
	private static void setupHeader(byte buff[], char type, String str)
	{
    	String lenAsString = Integer.toString(str.length()); 
    	Log.i(LOG_TAG, lenAsString);
    	Log.i(LOG_TAG, str);

    	buff[0] = (byte) type;
    	
    	for (int i = 0; i < lenAsString.length(); i++) 
    		buff[1+i] = (byte) lenAsString.charAt(i);
    	for (int i = lenAsString.length(); i < 4; i++)
    		buff[1+i] = ' ';
	}
	
	private static void setupPayload(byte buff[], String str)
	{
	    for(int i = 0; i < str.length(); i++) {
	    	buff[5+i] = (byte) str.charAt(i);
	    }
	}
	
	private static OutputStream setupOutputStream(BluetoothSocket mSocket)
	{
		OutputStream out;
		try 
		{
			out = mSocket.getOutputStream();
		} catch (IOException e1) {
			out = null;
		}
		
		return out;
	}
	
	private static void printBufferContents(byte[] buff)
	{
		String stringToSend;
		try 
		{
			stringToSend = new String(buff, "UTF-8");
		    Log.i(LOG_TAG, "Writing string: " + stringToSend);
		} catch (UnsupportedEncodingException e1) {
			Log.i(LOG_TAG, "UTF-8 not supported");
		}
	}
	
	private static boolean writeBytesToSocket(OutputStream out, byte[] buff)
	{
		// TODO: Make this similar to readBytesFromSocket so long messages can safely be sent
		try
		{
			out.write(buff);
			return true;
		}
		catch (IOException e)
		{
			return false;
		}
	}
	
	@SuppressWarnings("deprecation")
	public static void writeToSocket(BluetoothSocket mSocket, char type, String str)
	{
    	if (mSocket == null)
    	{
    		Log.i(LOG_TAG, "BluetoothSocket cannot be null");
    	}
    	else
    	{
    		byte buff[] = new byte[computePacketLength(str)]; 

    		OutputStream out = setupOutputStream(mSocket);

    		if (out == null)
    		{
    			Log.i(LOG_TAG, "setupOutputStream() failed");
    		}
    		else
    		{
			    if (str.length() >= 10000 || str.length() < 0)
			    {
			    	Log.i(LOG_TAG, "String too large");
			    	return;
			    }
			    else
			    {
			    	setupHeader(buff, type, str);
			    }
			    
			    setupPayload(buff, str);
			    
			    printBufferContents(buff);

			    writeBytesToSocket(out, buff);
	
    		}
    	}
	}
	
	public static boolean readBytesFromSocket(InputStream sock, byte[] buff, int len)
	{
		try
		{
			int bytesRead = 0;
			int ret = 0;
			
			while (bytesRead < len)
			{
				ret = sock.read(buff, bytesRead, len - bytesRead);
				
				if (ret == -1)
				{
					Log.i(LOG_TAG, "sock.read() returned -1");
					return false;
				}
				bytesRead += ret;
			}
			return true;
		}
		catch (IOException e)
		{
			return false;
		}
	}
	
	private static InputStream setupInputStream(BluetoothSocket mSocket)
	{
		InputStream in;
		try 
		{
			in = mSocket.getInputStream();
		} 
		catch (IOException e) {
			in = null;
		}
		
		return in;
	}
	
	private static String translateBytesToString(byte[] buff)
	{
		String str = null;
		try {
			str = new String(buff, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			Log.i(LOG_TAG, "Failed to print contents");
		}
		return str;
	}
	
	private static void printHeaderContents(byte[] headerBuffer)
	{
		String header = translateBytesToString(headerBuffer);
		Log.i(LOG_TAG, "Header: "+ header);
		Log.i(LOG_TAG, "Type:" + header.charAt(0));
		Log.i(LOG_TAG, "Length: "+ Integer.parseInt(header.substring(1,5).trim()));
	}
	
	private static char getTypeFromHeader(byte[] headerBuffer)
	{
		String header = translateBytesToString(headerBuffer);
		if (header != null)
		{
			return header.charAt(0);
		}
		else
		{
			return '\0';
		}
	}
	
	private static int getLengthFromHeader(byte[] headerBuffer)
	{
		String header = translateBytesToString(headerBuffer);
		if (header != null)
		{
			return Integer.parseInt(header.substring(1,5).trim());
		}
		else
		{
			return -1;
		}
	}
	
	public static Cw_ReplyMessage readFromSocket(BluetoothSocket mSocket)
	{	
		byte headerBuffer[] = new byte[computeHeaderLength()]; 
		byte dataBuffer[] = null;
		InputStream in;
		Cw_ReplyMessage reply = null;
		
		if (mSocket == null)
    	{
    		Log.i(LOG_TAG, "Cw_Device not initialized");
    	}
    	else
    	{	
    		in = setupInputStream(mSocket);
    		
    		if (in == null)
    		{
    			Log.i(LOG_TAG, "setupInputStream() failed");
    		}
    		else
    		{
    			readBytesFromSocket(in, headerBuffer, computeHeaderLength());
    			printHeaderContents(headerBuffer);
    			dataBuffer = new byte[getLengthFromHeader(headerBuffer)];
    			readBytesFromSocket(in, dataBuffer, getLengthFromHeader(headerBuffer));
    			
				reply = new Cw_ReplyMessage(getTypeFromHeader(headerBuffer), translateBytesToString(dataBuffer));
    		}
    	}
		
		return reply;
	}	
	
	
	public static Cw_ReplyMessage sendJSONMessage(Cw_Device citwDevice, JSONObject jsonObj)
	{
		Cw_ReplyMessage replyMessage;
		try {
			Cw_BluetoothSocket.writeToSocket(citwDevice.getSocket(), 'C', jsonObj.toString(3));
			replyMessage = Cw_BluetoothSocket.readFromSocket(citwDevice.getSocket());
			Log.i(LOG_TAG, replyMessage.getMessage());
		} catch (JSONException e) {
			Log.i(LOG_TAG,  "takeMeasurementsFromDevice() failed");
			return null;
		}
		
		return replyMessage;
	}

}
