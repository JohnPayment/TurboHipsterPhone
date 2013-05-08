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

	/**
	 * Construct the language screen
	 *
	 * @param title the title of the screen
	 * @param items the menu items of the screen
	 * @param m     the Configurator this screen is part of
	 */
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
								.append((m.getSettingScreen().getToggleableState(Settings.MANDOWN) ? "T" : "F"))
								.append('\n')
								.append((m.getSettingScreen().getToggleableState(Settings.FALL) ? "T" : "F"))
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
									.append((m.getSettingScreen().getToggleableState(Settings.MANDOWN) ? 'T' : 'F'))
									.append('\n')
									.append((m.getSettingScreen().getToggleableState(Settings.FALL) ? 'T' : 'F'))
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
