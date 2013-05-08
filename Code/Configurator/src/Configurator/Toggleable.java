package Configurator;

import Util.Messages;
import Util.Strings;

/**
 * Represents a toggleable item
 * @author Team Cirno
 */
public class Toggleable
{
	/**
	 * The state of the toggleable.
	 */
	public boolean state = false;

	/**
	 * The message in the toggleable.
	 */
	public int message = 0;

	public Toggleable(int msg, boolean state)
	{
		this.state = state;
		message = msg;
	}

	/**
	 * set the state of the toggleable
	 *
	 * @param s the state
	 */
	public void setState(boolean s)
	{
		state = s;
	}

	/**
	 * toggle the state
	 */
	public void toggle()
	{
		state = !state;
	}

	/**
	 * Returns a string formatted as <br/>
	 * "Toggle " [message] " - ON" OR
	 * "Toggle " [message] " - OFF"
	 *
	 * @return formatted string
	 */
	public String toString()
	{
		String str[] = Strings.getAllMessages();
		String retVal = str[Messages.TOGGLE] + " " + Strings.getMessage(message) + ": ";

		retVal += ((state)? str[Messages.ON] : str[Messages.OFF]);
		return retVal;
	}
}
