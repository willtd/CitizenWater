package citwater.data;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.database.sqlite.SQLiteOpenHelper;
import citwater.app.R;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.sql.SQLException;

public class Cw_DatabaseHelper extends SQLiteOpenHelper{

   // private static final int DATABASE_VERSION = 2;
   // private static final String DATABASE_CREATE = "create table MyEmployees( _id integer primary key,name text not null);";

    public static final String DATABASE_NAME = "Citizen.db";
    public static final String READINGTYPE_TABLE = "ReadingTypes";
    public static final String READING_TABLE = "Readings";
    public static final String DEVICE_TABLE = "Devices";
    public static final String READING_TYPE_ID_COL_1 = "ReadingTypeID";
    public static final String NAME_COL = "Name";
    public static final String UNIT_COL = "Unit";
    public static final String READING_ID_COL = "ReadingID";
    public static final String READING_TYPE_ID_COL_2 = "ReadingTypesID";
    public static final String TIME_COL = "Time";
    public static final String VALUE_COL = "Value";
    public static final String DEVICE_COL = "DeviceID";

    public static final String DEVICECLOUDID_COL = "DeviceCloudID";
    public static final String DEVICENAME = "DeviceName";
    public static final String DEVICECLOUDKEY = "DeviceCloudID";
    public static final String DEVIDEID_COL2 = "DeviceID";
    
    private static final boolean debugon = true;
    private static final int DATABASE_VERSION = 3;
    private static String dbName = "citizen";
    private String tmp;
    private String dbPath;
    private SQLiteDatabase crsDB;
    private final Context context;
    private int refs = 0;
    
    public Cw_DatabaseHelper(Context context) {
       // super(context, DATABASE_NAME, null, DATABASE_VERSION);
    	 super(context,dbName, null, DATABASE_VERSION);
         this.context = context;
         tmp = context.getFilesDir().getPath();
         dbPath = tmp.substring(0, (tmp.length()-5));
         dbPath += "databases/";
         android.util.Log.d(this.getClass().getSimpleName(),"dbpath: "+dbPath);
    }

    // Method is called during creation of the database
    @Override
    public void onCreate(SQLiteDatabase database) {
       // database.execSQL(DATABASE_CREATE);
    	 this.getReadableDatabase();
         try {
             copyDB();
         } catch (IOException e) {
             android.util.Log.w(this.getClass().getSimpleName(),
                     "createDB Error copying database " + e.getMessage());
         }
    }

    // Method is called during an upgrade of the database,
    @Override
    public void onUpgrade(SQLiteDatabase database,int oldVersion,int newVersion){
    	android.util.Log.w(Cw_DatabaseHelper.class.getName(),
                         "Upgrading database from version " + oldVersion + " to "
                         + newVersion + ", which will destroy all old data");
        database.execSQL("DROP TABLE IF EXISTS MyEmployees");
        onCreate(database);
    }
    
    private boolean checkDB(){    //does the database exist and is it initialized?
        SQLiteDatabase checkDB = null;
        boolean ret = false;
        try{
            String path = dbPath + dbName + ".db";
            debug("citizenDB --> checkDB: path to db is", path);
            File aFile = new File(path);
            if(aFile.exists()){
                checkDB = SQLiteDatabase.openDatabase(path, null, SQLiteDatabase.OPEN_READWRITE);
                if (checkDB!=null) {
                    debug("citizenDB --> checkDB","opened db at: "+checkDB.getPath());
                    Cursor tabChk = checkDB.rawQuery("SELECT name FROM sqlite_master where type='table' and name='course';", null);
                    boolean crsTabExists = false;
                    if(tabChk == null){
                        debug("citizenDB --> checkDB","check for course table result set is null");
                    }else{
                        tabChk.moveToNext();
                        debug("citizenDB --> checkDB","check for course table result set is: " +
                                ((tabChk.isAfterLast() ? "empty" : (String) tabChk.getString(0))));
                        crsTabExists = !tabChk.isAfterLast();
                    }
                    if(crsTabExists){
                        Cursor c= checkDB.rawQuery("SELECT * FROM course", null);
                        c.moveToFirst();
                        while(! c.isAfterLast()) {
                            String crsName = c.getString(0);
                            int crsid = c.getInt(1);
                            debug("citizenDB --> checkDB","Course table has CourseName: "+
                                    crsName+"\tCourseID: "+crsid);
                            c.moveToNext();
                        }
                        ret = true;
                    }
                }
            }
        }catch(SQLiteException e){
            android.util.Log.w("citizenDB->checkDB",e.getMessage());
        }
        if(checkDB != null){
            checkDB.close();
        }
        return ret;
    }
    
    public void copyDB() throws IOException{
        try {
            if(!checkDB()){
                // only copy the database if it doesn't already exist in my database directory
                debug("citizenDB --> copyDB", "checkDB returned false, starting copy");
                InputStream ip =  context.getResources().openRawResource(R.raw.citizen);
                // make sure the database path exists. if not, create it.
                File aFile = new File(dbPath);
                if(!aFile.exists()){
                    aFile.mkdirs();
                }
                String op=  dbPath  +  dbName +".db";
                OutputStream output = new FileOutputStream(op);
                byte[] buffer = new byte[1024];
                int length;
                while ((length = ip.read(buffer))>0){
                    output.write(buffer, 0, length);
                }
                output.flush();
                output.close();
                ip.close();
            }
        } catch (IOException e) {
            android.util.Log.w("citizenDB --> copyDB", "IOException: "+e.getMessage());
        }
    }

    public synchronized SQLiteDatabase openDB() throws SQLException {
    	refs++;
    	
    	if (refs == 1)
    	{
	        String myPath = dbPath + dbName + ".db";
	        crsDB = SQLiteDatabase.openDatabase(myPath, null, SQLiteDatabase.OPEN_READWRITE);
	        debug("citizenDB --> openDB", "opened db at path: " + crsDB.getPath());
    	}
        return crsDB;
    }

    @Override
    public synchronized void close() {
    	refs--;
    	
    	if (refs == 0)
    	{
    		crsDB.close();
    	}
        super.close();
    }

    private void debug(String hdr, String msg){
        if(debugon){
            android.util.Log.d(hdr,msg);
        }
    }
    
 

    public boolean insertIntoReadingTypes(int ReadingTypeID, String Name, String Unit) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues contentValues = new ContentValues();
        contentValues.put(READING_TYPE_ID_COL_1, ReadingTypeID);
        contentValues.put(NAME_COL, Name);
        contentValues.put(UNIT_COL, Unit);
        long result = db.insert(READINGTYPE_TABLE, null, contentValues);
        if (result == -1)
            return false;
        else
            return true;
    }

   	/*** IF YOU ARE GOING TO USE THIS FUNCTION, PLEASE REMOVE REFERENCES TO 'TYPE' ***/
    
    public boolean insertIntoReadings(int ReadingID, int Type, int ReadingTypesID, int Time, double Value, int deviceID) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues contentValues = new ContentValues();
        contentValues.put(READING_ID_COL, ReadingID);
        contentValues.put(READING_TYPE_ID_COL_2, ReadingTypesID);
        contentValues.put(TIME_COL, Time);
        contentValues.put(VALUE_COL, Value);
        contentValues.put(DEVICE_COL, deviceID);
        long result = db.insert(READING_TABLE, null, contentValues);
        if (result == -1)
            return false;
        else
            return true;
    }
    
    public boolean insertIntoDevices(String deviceCloudID, String deviceName, String deviceCloudKey, int deviceID) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues contentValues = new ContentValues();
        contentValues.put(DEVICECLOUDID_COL, deviceCloudID);
        contentValues.put(DEVICENAME, deviceName);
        contentValues.put(DEVICECLOUDKEY, deviceCloudKey);
        contentValues.put(DEVIDEID_COL2, deviceID);
        long result = db.insert(DEVICE_TABLE, null, contentValues);
        if (result == -1)
            return false;
        else
            return true;
    }
    
    public int getReadingTypesID(String str) {
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor res = db.rawQuery("select * from " + READING_TABLE + " where Name = '" + str + "'", null);
        if (res.getString(0) == null) {
            return -1;
        } else {
            return Integer.parseInt(res.getString(0));
        }

    }
    
    public int getDeviceID(String str) {
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor res = db.rawQuery("select DeviceID from " + DEVICE_TABLE + " where DeviceName = '" + str + "'", null);
        if (res.getString(0) == null) {
            return -1;
        } else {
            return res.getInt(0);
        }

    }
    
    public long getLastestTime(String deviceName){
    	SQLiteDatabase db = this.getWritableDatabase();
    	int deviceID = 0;
    	long max = 0;
    	Cursor res = db.rawQuery("select DeviceID from Devices where DeviceName = '" + deviceName + "'", null);
    	if(res.isNull(0)){
    		max = 0;
    	} else {
    		if(res.moveToNext()){
    			deviceID = res.getInt(0);
    		} 
    		Cursor cursor = db.rawQuery("select Time from Readings where DeviceID = " + deviceID, null);
    		while(cursor.moveToNext()){
    			if(cursor.getLong(0) > max){
    				max = cursor.getLong(0);
    			}
    		}
    	}
    	return max;
    }
    

    /*
    public Cursor getValueAfter(String str, int time) {
        SQLiteDatabase db = this.getWritableDatabase();
        int num = getReadingTypesID(str);
        Cursor res = db.rawQuery("select Value from " + READING_TABLE + " where ReadingTypesID = " + num + " and Time > " + time, null);
        return res;
    }
    */
/*
    public Cursor getAllDataFromReadings() {
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor res = db.rawQuery("select * from " + READING_TABLE, null);
        return res;
    }
*/
    /*
    public Cursor getAllDataFromReadingTypes() {
        SQLiteDatabase db = this.getWritableDatabase();
        Cursor res = db.rawQuery("select * from " + READINGTYPE_TABLE, null);
        return res;
    }
*/

    
}
