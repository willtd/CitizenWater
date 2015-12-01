package simpleui.modifiers.v3;

import simpleui.SimpleUI.OptionsMenuListener;
import simpleui.modifiers.ModifierInterface;
import simpleui.modifiers.v1.uiDecoration.ExampleDecorator;
import simpleui.util.ColorUtils;
import simpleui.util.ImageTransform;
import simpleui.util.Log;
import simpleui.util.MenuItemList;
import simpleui.util.SimpleUiApplication;
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.support.v7.widget.CardView;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;
import android.widget.ScrollView;

/**
 * (Adding {@link M_Container}s to an {@link M_Container} is possible to group
 * information in cards)
 *
 */
public class M_Container extends M_Collection implements OptionsMenuListener {

	private static final String LOG_TAG = M_Container.class.getSimpleName();
	public static final int DEFAULT_CHILDREN_PADDING_IN_DIP = 4;
	public static final int DEFAULT_SHADDOW_SIZE_IN_DIP = 5;
	private static final int OUTER_BACKGROUND_DIMMING_COLOR = Color.argb(200,
			255, 255, 255);
	private static Handler myHandler = new Handler(Looper.getMainLooper());

	private Context context;
	MenuItemList menuItemList;
	private Integer cardBackgroundColor = null;
	private boolean fillCompleteScreen = false;
	private Integer bgDimmingColor = OUTER_BACKGROUND_DIMMING_COLOR;
	private CardView card;
	private LinearLayout mostOuterBox;

	@Override
	public View getView(Context c) {
		this.context = c;
		mostOuterBox = new LinearLayout(context);
		mostOuterBox.setGravity(Gravity.CENTER);
		if (bgDimmingColor != null) {
			mostOuterBox.setBackgroundColor(bgDimmingColor);
		}
		rebuildUi();
		return mostOuterBox;
	}

	public boolean rebuildUi() {
		if (mostOuterBox == null) {
			return false;
		}
		if (Looper.getMainLooper().getThread() == Thread.currentThread()) {
			mostOuterBox.removeAllViews();
			createCardWithContent();
			mostOuterBox.addView(card);
		} else {
			myHandler.post(new Runnable() {
				@Override
				public void run() {
					rebuildUi(); // execute again on ui thread
				}
			});
		}
		return true;
	}

	private void createCardWithContent() {
		LinearLayout outerCardContainer = new LinearLayout(context);
		LinearLayout cardItems = new LinearLayout(context);
		boolean firstEntryIsToolbar = size() > 0 && get(0) instanceof M_Toolbar;
		boolean fillScreenHeight = firstEntryIsToolbar
				|| this.fillCompleteScreen;

		int shaddowSize = (int) ImageTransform.dipToPixels(context,
				DEFAULT_SHADDOW_SIZE_IN_DIP);
		if (fillScreenHeight) {
			shaddowSize = 0;
		}
		card = newCardViewWithContainers(context, outerCardContainer,
				cardItems, shaddowSize);
		if (cardBackgroundColor != null) {
			card.setCardBackgroundColor(cardBackgroundColor);
		}
		createViewsForAllModifiers(context, cardItems, firstEntryIsToolbar);
		if (fillScreenHeight) {
			card.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT,
					LayoutParams.MATCH_PARENT));
		}
		if (firstEntryIsToolbar) {
			outerCardContainer.addView(get(0).getView(context), 0);
		}
	}

	public static CardView newCardViewWithContainers(Context context,
			LinearLayout outerContainer, LinearLayout listItemContainer,
			int shaddowSizeInPxls) {
		CardView card = newCardView(context, shaddowSizeInPxls);
		outerContainer.setOrientation(LinearLayout.VERTICAL);
		ScrollView scrollContainer = new ScrollView(context);
		listItemContainer.setOrientation(LinearLayout.VERTICAL);
		int p = (int) ImageTransform.dipToPixels(context,
				DEFAULT_CHILDREN_PADDING_IN_DIP);
		listItemContainer.setPadding(p, p, p, p);
		scrollContainer.addView(listItemContainer);
		outerContainer.addView(scrollContainer);
		card.addView(outerContainer);
		return card;
	}

	public static CardView newCardView(Context context, int shaddowSizeInPxls) {
		CardView card = new CardView(context);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
		if (Build.VERSION_CODES.LOLLIPOP == Build.VERSION.SDK_INT) {
			params.setMargins(shaddowSizeInPxls, shaddowSizeInPxls,
					shaddowSizeInPxls, shaddowSizeInPxls);
		} else {
			card.setMaxCardElevation(shaddowSizeInPxls);
		}
		card.setCardElevation(shaddowSizeInPxls);
		card.setLayoutParams(params);
		card.setCardBackgroundColor(ColorUtils.getDefaultBackgroundColor(
				context, 0xFF00FF));
		return card;
	}

	public void setFillCompleteScreen(boolean fillCompleteScreen) {
		this.fillCompleteScreen = fillCompleteScreen;
	}

	public boolean closeParentActivity() {
		if (getContext() instanceof Activity) {
			((Activity) getContext()).finish();
			return true;
		}
		Log.w(LOG_TAG, "Could not close window, parent context=" + context
				+ " was not an activity");
		return false;
	}

	@Override
	public boolean add(ModifierInterface object) {
		if (object instanceof M_Container) {
			((M_Container) object).setBgDimmingColor(null);
		}
		return super.add(object);
	}

	/**
	 * @param bgDimmingColor
	 *            set it to null to not apply a background dimming, useful for
	 *            nested {@link M_Container}s e.g.
	 */
	public void setBgDimmingColor(Integer bgDimmingColor) {
		this.bgDimmingColor = bgDimmingColor;
	}

	@Override
	public boolean save() {
		boolean result = true;
		for (ModifierInterface m : this) {
			if (m != null && !m.save()) {
				Log.w(LOG_TAG, "save(): Modifier " + m
						+ " rejected save request");
				result = false;
			}
		}
		return result;
	}

	public void setBackgroundColor(Integer cardBackgroundColor) {
		setCardBackgroundColor(cardBackgroundColor);
	}

	public void setCardBackgroundColor(Integer cardBackgroundColor) {
		this.cardBackgroundColor = cardBackgroundColor;
		if (card != null) {
			myHandler.post(new Runnable() {

				@Override
				public void run() {
					card.setCardBackgroundColor(M_Container.this.cardBackgroundColor);
				}
			});
		}
	}

	public void setMenuItemList(MenuItemList menuItemList) {
		this.menuItemList = menuItemList;
	}

	@Override
	public boolean onCreateOptionsMenu(Activity a, Menu menu) {
		Log.i(LOG_TAG, "onCreateOptionsMenu menuItemList=" + menuItemList);
		if (menuItemList != null) {
			return menuItemList.onCreateOptionsMenu(a, menu);
		}
		return false;
	}

	@Override
	public boolean onPrepareOptionsMenu(Activity a, Menu menu) {
		Log.i(LOG_TAG, "onPrepareOptionsMenu menuItemList=" + menuItemList);
		if (menuItemList != null) {
			return menuItemList.onPrepareOptionsMenu(a, menu);
		}
		return false;
	}

	@Override
	public boolean onOptionsItemSelected(Activity a, MenuItem item) {
		Log.i(LOG_TAG, "onOptionsItemSelected menuItemList=" + menuItemList);
		if (menuItemList != null) {
			return menuItemList.onOptionsItemSelected(a, item);
		}
		return false;
	}

	@Override
	public void onOptionsMenuClosed(Activity a, Menu menu) {
		menuItemList.onOptionsMenuClosed(a, menu);
	}

	public Context getContext() {
		if (context == null) {
			context = SimpleUiApplication.getContext();
		}
		return context;
	}

	@Deprecated
	public void assignNewDecorator(ExampleDecorator exampleDecorator) {
		Log.e(LOG_TAG, "assignNewDecorator: Decorators are no longer supported");
	}
}
