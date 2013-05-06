package Util;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Graphics;
import javax.microedition.midlet.MIDlet;

/**
 * Creates a popup dialog box on the screen.
 * @author Team Cirno
 */
public class Popup
    extends Canvas
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

    /**
     * Construct a popup.
     *
     * @param owner the owner of this popup
     * @param message the message being displayed
     * @param cancelable if this popup is cancellable or not
     */
    public Popup(MIDlet owner,
                 String message,
                 boolean cancelable,
                 Displayable display)
    {
        this.owner = owner;
        this.message = message;
        this.cancelable = cancelable;
        this.dis = display;
        height = 85;//285 max size
        x = 10;
        width = display == null ? 220 : display.getWidth() - (2 * x);//240 max size
        y = display == null ? 190 : display.getHeight() - height - 10;
    }

    /**
     * Paint method. what will be painted
     *
     * @param g graphics object
     */
    protected void paint(Graphics g)
    {
        g.setColor(0xC0FFEE);
        g.fillRect(x, y, width, height);
        g.setColor(0);
        g.setStrokeStyle(Graphics.SOLID);
        g.drawString(message, x, y, Graphics.LEFT | Graphics.TOP);
    }

    /**
     * kill the owner if cancelable
     *
     * @param keyCode they key that was pressed
     */
    protected void keyPressed(int keyCode)
    {
        if (cancelable)
        {
            Display disp = Display.getDisplay(owner);
            disp.setCurrent(this.dis);
            this.owner.notifyDestroyed();
        }
    }
}
