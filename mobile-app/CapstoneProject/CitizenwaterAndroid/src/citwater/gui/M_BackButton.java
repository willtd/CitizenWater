package citwater.gui;

import citwater.app.Cw_ButtonTimer;
import android.app.Activity;
import android.content.Context;
import android.widget.Button;
import simpleui.modifiers.v3.M_Button;
import simpleui.modifiers.v3.M_Container;

public class M_BackButton extends M_Button {
	
	M_Container container;
	public M_BackButton(String buttonText, M_Container buttonContainer)
	{
		super(buttonText);
		container = buttonContainer;
	}
	
	@Override
	public void onClick(Context c, Button b) {
		if (Cw_ButtonTimer.isClickAllowed())
		{
			if (container.save()) {
				if (c instanceof Activity) {
					((Activity) c).finish(); // close the window showing this container:
				}
			} else {
				// at least one modifier did not accept the save request
			}
		}
	}
}
