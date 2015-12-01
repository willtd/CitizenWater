package citwater.app;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;

import android.util.Log;

public class Cw_SensorCloud {
	
	private String authKey;
	private String authToken;
	private String server;
	private String deviceId;
	private static final int VERSION = 1;
	private static final int MAX_DATAPOINTS = 100000;
	
	public Cw_SensorCloud(String deviceId, String authKey)
	{
		this.authKey = authKey;
		this.deviceId = deviceId;
	}
	
	private String readHttpResponse(HttpURLConnection connection) throws IOException
	{
		StringBuilder builder = new StringBuilder();
		BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));
		String line;
		while ((line = reader.readLine()) != null)
		{
			builder.append(line);
		}
		
		reader.close();
		
		return builder.toString();
	}
	
	private HttpURLConnection setupConnection(String urlString, String method) throws IOException
	{
		URL url = new URL(urlString);
		HttpURLConnection connection = (HttpURLConnection) url.openConnection();
		connection.setRequestMethod(method);

		connection.setUseCaches(false);
		
		if (method.equals("PUT") || method.equals("POST"))
		{
			connection.setDoOutput(true);
			connection.setRequestProperty("Content-Type", "application/xdr");
		}
		else
		{
			connection.setRequestProperty("Accept", "application/xdr");
		}
		
		return connection;
	}
	
	private int getVersion()
	{
		return VERSION;
	}
	
	private String getAuthenticationURL()
	{
		return "https://" + "sensorcloud.microstrain.com" + "/SensorCloud/devices/" + deviceId + "/";
	}
	
	private String getDeviceURL()
	{
		return "https://" + server + "/SensorCloud/devices/" + deviceId + "/";
	}
	private String getSensorURL(String sensorType)
	{
		return getDeviceURL() + "sensors/" + sensorType + "/";
	}
	
	private String getChannelURL(String sensorType, String channel)
	{
		return getSensorURL(sensorType) + "channels/" + channel + "/";
	}
	
	private String getTimeSeriesURL(String sensorType, String channel)
	{
		return getChannelURL(sensorType, channel) + "streams/timeseries/";
	}
	private String getAuthTokenAndVersionSuffix()
	{
		return "?version=" + getVersion() + "&auth_token=" + authToken; 
	}
	
	private String getAuthKeyAndVersionSuffix()
	{
		return "?version=" + getVersion() + "&key=" + authKey;
	}
	
	private boolean doesDatastreamExist(String sensorType) throws IOException
	{
		HttpURLConnection connection = null;
		
		String urlString = getTimeSeriesURL(sensorType, "citizenwater") + getAuthTokenAndVersionSuffix();
		
		boolean ret = true;
		
		connection = setupConnection(urlString, "GET");

		if (connection.getResponseCode() == 200)
		{
			ret = true;
		}
		else
		{
			ret = false;
		}

		if (connection != null)
		{
			connection.disconnect();
		}

		return ret;
	}
	
	public Long getLatestDatastreamTime(String sensorType)
	{
		Long ret = null;
		HttpURLConnection connection = null;

		String urlString = getTimeSeriesURL(sensorType, "citizenwater") + "data/latest/" + getAuthTokenAndVersionSuffix();
	
		try 
		{
			if (doesDatastreamExist(sensorType))
			{
				connection = setupConnection(urlString, "GET");

				if (connection.getResponseCode() == 200)
				{
					String response = readHttpResponse(connection);

					Xdr xdr = new Xdr(response.getBytes().length);

					xdr.xdr_raw(response.getBytes(), 0);
					xdr.xdr_offset(0);
					ret = Long.valueOf(xdr.xdr_hyper());
					
					Log.i("Cw_SensorCloud", "Latest datastream point (time/value): (" + ret + "," + xdr.xdr_float() +")");
				}
				else
				{
					ret = null;
				}
			}
			else
			{
				// Stream does not yet exist
				Log.i("Cw_SensorCloud", "Datastream point does not yet exist");
			}

		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
		finally
		{
			if (connection != null)
			{
				connection.disconnect();
			}
		}
		
		return ret;
	}
	
	public int addChannel(String sensorType)
	{
		int ret = 0;
		HttpURLConnection connection = null;
		String urlString = getChannelURL(sensorType, "citizenwater") + getAuthTokenAndVersionSuffix();
		
		Xdr xdr = new Xdr(1000 /* larger than needed */);
		xdr.xdr_int(1); // Version
		xdr.xdr_string("citizenwater");
		xdr.xdr_string("citizenwater");
		xdr.xdr_size(xdr.xdr_offset());
		xdr.xdr_offset(0);
		
		try 
		{
			connection = setupConnection(urlString, "PUT");

			DataOutputStream out = new DataOutputStream(connection.getOutputStream());
			out.write(xdr.xdr_raw(0, xdr.xdr_size()));

			ret = connection.getResponseCode();

			out.close();
		}
		catch (IOException e)
		{
			e.printStackTrace();
			ret = -1;
		}
		finally
		{
			if (connection != null)
			{
				connection.disconnect();
			}
		}
		
		return ret;
	}
	
	public int addSensor(String sensorType, String sensorLabel, String description)
	{
		int ret = 0;
		HttpURLConnection connection = null;
		String urlString = getSensorURL(sensorType) + getAuthTokenAndVersionSuffix();
		android.util.Log.d("sync:urlString", (urlString));
		
		Xdr xdr = new Xdr(1000 /* larger than needed */);
		xdr.xdr_int(1); // Version
		xdr.xdr_string(sensorType);
		xdr.xdr_string(sensorLabel);
		xdr.xdr_string(description);
		xdr.xdr_size(xdr.xdr_offset());
		xdr.xdr_offset(0);

		try 
		{
			connection = setupConnection(urlString, "PUT");

			DataOutputStream out = new DataOutputStream(connection.getOutputStream());
			out.write(xdr.xdr_raw(xdr.xdr_size()));

			ret = connection.getResponseCode();
			android.util.Log.d("sync:getResponseCode", String.valueOf(ret));
			out.close();
		}
		catch (IOException e)
		{
			e.printStackTrace();
			ret = -1;
		}
		finally
		{
			if (connection != null)
			{
				connection.disconnect();
			}
		}
		
		return ret;
	}
	public int authenticate()
	{
		int ret = 0;
		HttpURLConnection connection = null;

		String urlString = getAuthenticationURL() + "authenticate/" + getAuthKeyAndVersionSuffix();
		
		try 
		{
			connection = setupConnection(urlString, "GET");
			
			String response = readHttpResponse(connection); 

			if (connection.getResponseCode() == 200)
			{
				Xdr xdr = new Xdr(response.getBytes().length);
				xdr.xdr_raw(response.getBytes(), 0);
				xdr.xdr_offset(0);
				
				authToken = xdr.xdr_string();
				System.out.println(authToken);
				server = xdr.xdr_string();
				System.out.println(server);
				ret = 200;
			}
			else
			{
				ret = connection.getResponseCode();
			}
		}
		catch (IOException e)
		{
			e.printStackTrace();
			ret = -1;
		}
		finally
		{
			if (connection != null)
			{
				connection.disconnect();
			}
		}
		return ret;
	}
	
	public int addTimeSeriesData(String sensorType, ArrayList<Long> time, ArrayList<Float> data)
	{
		int ret = 0;
		HttpURLConnection connection = null;
		String urlString = getTimeSeriesURL(sensorType, "citizenwater") + "data/" + getAuthTokenAndVersionSuffix();
		
		if (time.size() != data.size())
		{
			return -1;
		}
		
		Xdr xdr = new Xdr(4*4 + 12 * time.size());
		xdr.xdr_int(1); // Version
		xdr.xdr_int(1); // sample rate is in seconds (0) or hertz (1). Doesn't matter for us
		xdr.xdr_int(100); // sample rate (doesn't matter for us)
		
		xdr.xdr_int(time.size()); // ARRAY IS PREFIXED BY LENGTH
		
		/* TODO: ARRAYS OF DATA. Up to MAX_DATAPOINTS a message */
		
		for(int i = 0; i < time.size(); i++)
		{
			xdr.xdr_hyper(time.get(i));
			xdr.xdr_float(data.get(i));
		}
		
		xdr.xdr_offset(0);

		try 
		{
			connection = setupConnection(urlString, "POST");

			DataOutputStream out = new DataOutputStream(connection.getOutputStream());
			out.write(xdr.xdr_raw(0, xdr.xdr_size()));
			
			ret = connection.getResponseCode();
			
			out.close();
		}
		catch (IOException e)
		{
			ret = -1;
		}
		finally
		{
			if (connection != null)
			{
				connection.disconnect();
			}
		}
		return ret;
	}
}