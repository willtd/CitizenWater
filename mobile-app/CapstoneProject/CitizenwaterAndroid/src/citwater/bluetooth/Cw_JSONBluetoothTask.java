package citwater.bluetooth;

import org.json.JSONObject;

import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.widget.Toast;
import citwater.data.Cw_Device;
import citwater.data.Cw_ReplyMessage;

public class Cw_JSONBluetoothTask extends  AsyncTask<Void, Void, Boolean> {

	private Cw_Device citwDevice;
	private JSONObject jsonObj;
	private Context context;
	private ProgressDialog progressDialog;
	private Cw_ReplyMessage replyMessage; // TODO: DO SOMETHING WITH THIS
		
	public Cw_JSONBluetoothTask(final Context context,
			    				final Cw_Device citwDevice, 
							    final JSONObject jsonObj)
	{
		this.context = context;
		this.citwDevice = citwDevice;
		this.jsonObj = jsonObj;
	}
		
	@Override
	protected void onPreExecute()
	{
		super.onPreExecute();
		
		progressDialog = new ProgressDialog(context);
		progressDialog.setIndeterminate(true);
		progressDialog.setMessage("Message transfer in progress...");
	    progressDialog.setCancelable(false);
	    progressDialog.setCanceledOnTouchOutside(false);
	    progressDialog.show();
	}
	
	@SuppressLint("NewApi")
	@Override
	protected Boolean doInBackground(Void... params) {
		replyMessage = Cw_BluetoothSocket.sendJSONMessage(citwDevice, jsonObj);
		return (replyMessage == null) ? false : true;
	}
	
	@Override
	protected void onPostExecute(Boolean result) // Run in UI thread
	{
		super.onPostExecute(result);
		progressDialog.dismiss();
		
		if (result == Boolean.valueOf(true))
		{
			Toast.makeText(context,  "Success!\n" + replyMessage.toString(), Toast.LENGTH_LONG).show();
		}
		else
		{
			Toast.makeText(context, "Connection failure", Toast.LENGTH_LONG).show();
		}
	}

}
