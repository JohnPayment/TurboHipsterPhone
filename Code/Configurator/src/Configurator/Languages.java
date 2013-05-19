package Configurator;

import Util.FileIO;
import Util.Messages;
import Util.Path;
import Util.Strings;
import java.io.IOException;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.List;

/*******************************************************************************
 * CLASS: Languages
 *
 * FUNCTIONS: Languages(String title, String[] items, Configurator m)
 *
 * DATE: 2013-01-18
 * 
 * REVISIONS: 2013-05-18
 *            Aaron Lee
 *            Luke Tao
 *            John Payment
 *
 * DESIGNER: Team Cirno
 *
 * PROGRAMMER: Team Cirno
 *
 * NOTES: The screen display for the language submenu
 *******************************************************************************/
/**
 * Screen for the Languages where you can switch preferred language.
 * @author Team Cirno
 */
public class Languages extends List
{
	/**
	 * index of english menu item;0
	 */
	public static final int ENGLISH = 0;

	/**
	 * index of french menu item;0
	 */
	public static final int FRENCH = 1;

	/**
	 * the configurator this screen is part of.
	 */
	private Configurator m;

	/*************************************************************************** 
	 * FUNCTION: Languages
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: Languages(String title, String[] items, Configurator m)
	 *            String title - the title of the screen
	 *            String[] items - the menu items of the screen
	 *            Configurator m - the Configurator this screen is part of
	 * 
	 * NOTES: Constructor for the language class
	 ***************************************************************************/
	public Languages(String title, String[] items, Configurator m)
	{
		super(title, List.IMPLICIT, items, null);

		this.m = m;
		addCommand(new Command("Back", Command.BACK, 1));
		addCommand(new Command("Select", Command.OK, 1));

		setCommandListener(new LanguageListener());
		this.setSelectedIndex((Strings.getLanguage().equals("English")) ? ENGLISH : FRENCH, true);
	}

	/**
	 * The command listener.
	 */
	private class LanguageListener implements CommandListener
	{
		/**
		 * Calls all other screen's changefrench methods
		 *
		 */
		private void updateLanguage()
		{
			String str[] = Strings.getAllMessages();

			setTitle(str[Messages.LANGUAGES]);
			set(ENGLISH, str[Messages.ENGLISH], null);
			set(FRENCH, str[Messages.FRENCH], null);
			m.getSettingScreen().updateLanguage();
			m.getLoginScreen().updateLanguage();
		}

		/**
		 * The command handler for the Screen.
		 * @param c The command
		 * @param d The displayable where the command originated.
		 */
		public void commandAction(Command c, Displayable d)
		{
			try
			{
				List l = (List)d;
				FileIO f = new FileIO();
				String current = (f.readFromAbsolutePath(Path.ROOT_PATH + Path.SETTINGS_FILENAME)[0]);
				switch(c.getCommandType())
				{
					case Command.BACK:
						Configurator.moveto(m.getSettingScreen());
						break;
					case Command.SCREEN:
					case Command.OK:
						if(l.isSelected(Languages.ENGLISH) && !current.equals("English"))
						{
							StringBuffer buff = new StringBuffer();
							buff.append("English")
								.append('\n')
								/*.append((m.getSettingScreen().getToggleableState(Settings.MANDOWN) ? "T" : "F"))
								.append('\n')
								.append((m.getSettingScreen().getToggleableState(Settings.FALL) ? "T" : "F"))
								.append('\n')
								.append((m.getSettingScreen().getToggleableState(Settings.GPS) ? "T" : "F"))*/
								.append('\n');
							f.writeTextFile(Path.SETTINGS_FILENAME, buff.toString());
							updateLanguage();
						} else
						{
							if(l.isSelected(Languages.FRENCH) && !current.equals("French"))
							{
								StringBuffer buff = new StringBuffer();
								buff.append("French")
									.append('\n')
									/*.append((m.getSettingScreen().getToggleableState(Settings.MANDOWN) ? 'T' : 'F'))
									.append('\n')
									.append((m.getSettingScreen().getToggleableState(Settings.FALL) ? 'T' : 'F'))
									.append('\n')
									.append((m.getSettingScreen().getToggleableState(Settings.GPS) ? "T" : "F"))*/
									.append('\n');
								f.writeTextFile(Path.SETTINGS_FILENAME, buff.toString());
								updateLanguage();
							}
						}
					default:
						break;
				}
			} catch(IOException e)
			{
				m.notifyDestroyed();
			}
		}
	}
}
