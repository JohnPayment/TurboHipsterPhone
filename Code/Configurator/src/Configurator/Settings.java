package Configurator;

import Util.Messages;
import Util.Strings;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.List;

/**
 * Setting screen.
 * @author Team Cirno
 */
public class Settings extends List
{
	/**
	 * Identifier of the language
	 */
	public static final int ENGLISH = 0;

	/**
	 * Identifier of the language
	 */
	public static final int FRENCH = 1;

	/**
	 * Identifier of the current language that is selected.
	 */
	public static int CURRENT_LANGUAGE = ENGLISH;

	/**
	 * The index of mandown toggle item; 0
	 */

	public static final int MANDOWN = 0;
	/**
	 * The index of fall detection toggle item; 1
	 */
	public static final int FALL = 1;
	/**
         * The index of GPS toggle item; 2
         */
        public static final int GPS = 2;
        
        /**
	 * The index of login item; 3
	 */
	public static final int LOGIN = 3;
	/**
	 * The index of language item; 4
	 */
	public static final int LANG = 4;
	/**
	 * The MIDlet this screen is a part of.
	 */
	private Configurator m;
	/**
	 * The togleable item for mandown.
	 */
	private Toggleable mandown;
	/**
	 * The toggleable item for fall detection.
	 */
	private Toggleable fall;
        /**
	 * The togleable item for GPS.
	 */
	private Toggleable gps;
        
	/**
	 * Construct this settings class.
	 *
	 * @param title title of the screen
	 * @param items array of menu items
	 * @param m the Configurator
	 */
	public Settings(String title, String[] items, Configurator m)
	{
		super(title, List.IMPLICIT, items, null);
		this.m = m;
		mandown = new Toggleable(Messages.MANDOWN, false);
		fall = new Toggleable(Messages.FALL_DETECTION, false);
                gps = new Toggleable(Messages.GPS, false);

		addCommand(new Command("Exit", Command.EXIT, 1));
		addCommand(new Command("Select", Command.OK, 1));

		setCommandListener(new SettingListener());
	}

	/**
	 * Returns the state of the toggleable.
	 *
	 * @param toggleable the index of the toggleable.
	 *
	 * @return true if valid toggleable is on, false otherwise
	 */
	public boolean getToggleableState(int toggleable)
	{
		switch (toggleable)
		{
			case MANDOWN:
				return mandown.state;
			case FALL:
				return fall.state;
                        case GPS:
                                return gps.state;
			default:
				return false;
		}
	}

	/**
	 * Returns the state of the toggleable.
	 *
	 * @param toggleable the index of the toggleable.
	 * @param s State of the toggleable
	 * @return true if valid toggleable is on, false otherwise
	 */
	public void setToggleableState(int toggleable, boolean s)
	{
		switch (toggleable)
		{
			case MANDOWN:
				mandown.setState(s);
				break;
			case FALL:
				fall.setState(s);
				break;
                        case GPS:
                                gps.setState(s);
                                break;
			default:
				throw new IllegalArgumentException("use a real index");
		}
	}

	/**
	 * Updates the language to current selected language.
	 */
	public void updateLanguage()
	{
		String str[] = Strings.getAllMessages();
		setTitle(str[Messages.SETTINGS]);
		set(MANDOWN, mandown.toString(), null);
		set(FALL, fall.toString(), null);
                set(GPS, gps.toString(), null);
		set(LOGIN, str[Messages.LOGIN], null);
		set(LANG, str[Messages.LANGUAGES], null);
	}

	/**
	 * Command listener.
	 */
	private class SettingListener implements CommandListener
	{
		public void commandAction(Command c, Displayable d)
		{
			List l = (List)d;
			switch (c.getCommandType())
			{
				case Command.SCREEN:
				case Command.OK:
					switch (l.getSelectedIndex())
					{
						case Settings.LOGIN:
							Configurator.moveto(m.getLoginScreen());
							break;
						case Settings.LANG:
							Configurator.moveto(m.getLanguageScreen());
							break;
						case Settings.MANDOWN:
							mandown.toggle();
							l.set(MANDOWN, mandown.toString(), null);
							break;
						case Settings.FALL:
							fall.toggle();
							l.set(FALL, fall.toString(), null);
							break;
                                                case Settings.GPS:
                                                        gps.toggle();
                                                        l.set(GPS, gps.toString(), null);
                                                        break;
						default:
							break;
					}
					break;
				default:
					m.destroyApp(false);
					m.notifyDestroyed();
					break;
			}
		}
	}
}
