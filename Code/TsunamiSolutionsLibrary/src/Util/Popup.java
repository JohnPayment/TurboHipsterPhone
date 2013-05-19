package Util;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Graphics;
import javax.microedition.midlet.MIDlet;

/*******************************************************************************
 * CLASS: Popup
 *
 * FUNCTIONS: Popup(MIDlet owner, String message, boolean cancelable, Displayable display)
 *            void paint(Graphics g)
 *            void keyPressed(int keyCode)
 *
 * DATE: 2013-01-18
 *
 * DESIGNER: Team Cirno
 *
 * PROGRAMMER: Team Cirno
 *
 * NOTES: Creates a popup dialog box on the screen.
 *******************************************************************************/
public class Popup extends Canvas
{
	/**
	 * the owner of this popup
	 */
	private MIDlet owner;
	
	/**
	 * the message to be displayed
	 */
	private String message;
	
	/**
	 * flag indicating if this popup will be cancelable or not
	 */
	private boolean cancelable;

	/**
	 * The previous screen.
	 */
	private Displayable dis;
	
	/**
	 * dimensions
	 */
	private int x, y, width, height;

	/***************************************************************************
	 * FUNCTION: Toggleable
	 *
	 * DATE: 2013-01-30
	 *
	 * DESIGNER: Team Cirno
	 *
	 * PROGRAMMER: Team Cirno
	 *
	 * INTERFACE: Popup(MIDlet owner, String message, boolean cancelable, Displayable display)
	 *            MIDlet owner - the owner of this popup
	 *            String message - the message being displayed
	 *            boolean cancelable - if this popup is cancellable or not
	 *            Displayable display - ???
	 *
	 * NOTES: Constructor for the Popup class
	 ***************************************************************************/
	public Popup(MIDlet owner, String message, boolean cancelable, Displayable display)
	{
		this.owner = owner;
		this.message = message;
		this.cancelable = cancelable;
		this.dis = display;
		
		height = 85; //285 max size
		x = 10;
		width = display == null ? 220 : display.getWidth() - (2 * x); //240 max size
		y = display == null ? 190 : display.getHeight() - height - 10;
	}

	/*************************************************************************** 
	 * FUNCTION: paint
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: void paint(Graphics g)
	 *            Graphics g - graphics object
	 * 
	 * RETURNS: void
	 * 
	 * NOTES: Paint method. what will be painted
	 ***************************************************************************/
	protected void paint(Graphics g)
	{
		g.setColor(0xC0FFEE);
		g.fillRect(x, y, width, height);
		g.setColor(0);
		g.setStrokeStyle(Graphics.SOLID);
		g.drawString(message, x, y, Graphics.LEFT | Graphics.TOP);
	}

	/*************************************************************************** 
	 * FUNCTION: paint
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: void keyPressed(int keyCode)
	 *            (int keyCode - they key that was pressed
	 * 
	 * RETURNS: void
	 * 
	 * NOTES: kill the owner if cancelable
	 ***************************************************************************/
	protected void keyPressed(int keyCode)
	{
		if(cancelable)
		{
			Display disp = Display.getDisplay(owner);
			disp.setCurrent(this.dis);
			this.owner.notifyDestroyed();
		}
	}
}
