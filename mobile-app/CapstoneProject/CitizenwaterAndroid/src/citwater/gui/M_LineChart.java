/**
 *  M_LineChart
 *  A time series for displaying water-quality data
 */

package citwater.gui;

import java.io.File;
import java.util.ArrayList;
import java.util.Date;

import org.achartengine.ChartFactory;
import org.achartengine.GraphicalView;
import org.achartengine.model.XYMultipleSeriesDataset;
import org.achartengine.renderer.XYMultipleSeriesRenderer;
import org.achartengine.renderer.XYSeriesRenderer;

import simpleui.modifiers.ModifierInterface;
import simpleui.modifiers.v1.uiDecoration.UiDecoratable;
import simpleui.modifiers.v1.uiDecoration.UiDecorator;
import android.app.ProgressDialog;
import android.content.Context;
import android.graphics.Color;
import android.os.AsyncTask;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;
import android.widget.Toast;
import citwater.data.Cw_DataManipulation;

public class M_LineChart implements ModifierInterface, UiDecoratable {
	private String readingType;
    private GraphicalView mChart;
    private XYMultipleSeriesDataset mDataset;
    private XYMultipleSeriesRenderer mRenderer;
    private ArrayList<XYSeriesRenderer> rendererList;

    // One color per device on the graph. If there are more than 6 devices, some colors will be repeated
    private final int[] colors = {Color.BLUE, Color.RED, Color.GREEN, Color.YELLOW, Color.MAGENTA, Color.WHITE};
    
	private UiDecorator myDecorator;

	/**
	 * @param d                  Cw_DataManipulation object for reading from the database
	 * @param readingType        Type of reading that will be graphed
	 * @param initialDeviceList  Devices for which data should be graphed
	 * @param start   
	 * @param end
	 */
	public M_LineChart(String readingType, 
					   ArrayList<String> initialDeviceList, 
					   Date start, 
					   Date end,
					   XYMultipleSeriesDataset mDataset) {
		this.readingType = readingType;
		this.mDataset = mDataset;
		mRenderer = new XYMultipleSeriesRenderer();
		rendererList = new ArrayList<XYSeriesRenderer>();

		initChartData(readingType, initialDeviceList, start, end);
	}
	/*
	public void updateChart(ArrayList<String> deviceList, Date start, Date end) 
	{
		if (mChart != null)
		{
			initChartData(readingType, deviceList, start, end);
			mChart.repaint();
		}
	}
	 */
	
    /*
     *  Called whenever chart data is initialized or reinitialized by updateChart().
     */
    private void initChartData(String readingType, ArrayList<String> deviceList, Date start, Date end) {
    	for (int i = 0; i < deviceList.size(); i++)
    	{
    		XYSeriesRenderer newRenderer = new XYSeriesRenderer();
    		newRenderer.setLineWidth(7);
    		newRenderer.setColor(colors[i % colors.length]);
    		newRenderer.setChartValuesTextSize(18);

    		rendererList.add(newRenderer);
    		mRenderer.addSeriesRenderer(newRenderer);
    		mRenderer.setXAxisMin(start.getTime());
    		mRenderer.setXAxisMax(end.getTime());
    		mRenderer.setMargins(new int[] { 50, 50, 100, 22 });
    		mRenderer.setLabelsTextSize(40);
    		mRenderer.setLegendTextSize(40);
    		mRenderer.setChartTitle("Measurement: " + readingType);
    		mRenderer.setChartTitleTextSize(60);
    		mRenderer.setPanLimits(new double[] {start.getTime(), end.getTime(), 0, 1000} ); // TODO: change y limits
    		// mRenderer.setLegendHeight(200);
    	}
    }
    
    /*
     *  Called in getView()
     */
    private void initChartView(Context context, LinearLayout.LayoutParams p2)
    {
		mChart = ChartFactory.getTimeChartView(context, mDataset, mRenderer, "MMM d, yyyy");
	    mChart.setLayoutParams(p2);
    }
    
	@Override
	public boolean assignNewDecorator(UiDecorator decorator) {
		myDecorator = decorator;
		return true;
	}
	
	@Override
	public View getView(Context context) {
		LinearLayout l = new LinearLayout(context);
		l.setGravity(Gravity.CENTER_VERTICAL);
	
		LinearLayout.LayoutParams p2 = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, 1500,
				0);

		initChartView(context, p2);
		l.addView(mChart);
		
		l.setPadding(DEFAULT_PADDING, DEFAULT_PADDING, DEFAULT_PADDING,
				DEFAULT_PADDING);

		if (myDecorator != null) {
			int level = myDecorator.getCurrentLevel();
			myDecorator.decorate(context, mChart, level + 1,
					UiDecorator.TYPE_EDIT_TEXT);
		}
		return l;
}

	@Override
	public boolean save() {
		return true;
	}
}

