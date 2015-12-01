/**
 *   M_GraphDataContainer
 *   Displays a list of graphs
 * 
 * 
 */

package citwater.gui;

import java.util.ArrayList;
import java.util.Date;

import org.achartengine.model.XYMultipleSeriesDataset;

import citwater.data.Cw_DataManipulation;
import simpleui.modifiers.ModifierInterface;
import simpleui.modifiers.v3.M_Container;
import simpleui.modifiers.v3.M_Toolbar;
import android.content.Context;

public class M_LineChartListContainer extends M_Container {
	private static final long serialVersionUID = -1728264691334134958L;

	public M_LineChartListContainer(final ArrayList<String> readingTypesList, 
								    final ArrayList<String> devicesList, 
								    final ArrayList<XYMultipleSeriesDataset> datasets,
								    final Date start, 
								    final Date end)
	{
		add(new M_Toolbar("Graphs"));
		
		add(new Cw_ListWrapperV2<String>(readingTypesList, null /* No 'add to list' button */) {	
			@Override
			public ModifierInterface getModifierForItem(Context c, String item,
					int posInList) {
				int datasetIndex = readingTypesList.indexOf(item);
				
				return new M_LineChart(item, devicesList, start, end, datasets.get(datasetIndex));
			}	
			@Override
			public String getNewItemInstance(Context c, int posOfNewItemInList) {
					return null; /* No 'add to list' feature required */
			}
		});
		
		add(new M_BackButton("Back", this));
	}
}
