package Util;

import java.io.IOException;

/*******************************************************************************
 * CLASS: Strings
 *
 * FUNCTIONS: void setupMessage()
 *            String getMessage(int i)
 *            String[] getAllMessages()
 *            String getLanguage()
 *
 * DATE: 2013-01-18
 *
 * DESIGNER: Team Cirno
 *
 * PROGRAMMER: Team Cirno
 *
 * NOTES: Contains all the strings of English and French language used by this 
 *        application. Needs Messages class for indexing.
 *******************************************************************************/
public class Strings
{
	/**
	 * Collection of messages.
	 */
	private static String[] messages;

	/**
	 * The current language selected
	 */
	private static String currentLanguage = "English";

	/*************************************************************************** 
	 * FUNCTION: setupMessage
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: void setupMessage()
	 * 
	 * RETURNS: void
	 * 
	 * NOTES: Sets up the Language to be French/English depending on the setting
	 *        configuration.
	 ***************************************************************************/
	private static void setupMessage()
	{
		//reads from the Setting file that the configurator will make. The first line of that file should contain "English" or "French"
		try
		{
			String path = Path.ROOT_PATH + Path.SETTINGS_FILENAME;
			FileIO f = new FileIO();
			String selectedLanguage = f.readFromAbsolutePath(path)[0]; //gets the first line of the file.
			messages = f.readFileFromResource(selectedLanguage);
			currentLanguage = selectedLanguage;
		} catch(IOException e)
		{
			throw new RuntimeException("FileReader error: " + e.getMessage());
		}
	}

	/*************************************************************************** 
	 * FUNCTION: getMessage
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String getMessage(int i)
	 *            int i - The index of the message. Use the constants in Messages class. (eg. Messages.OK)
	 * 
	 * RETURNS: The equivalent string of the message or null if there is an 
	 *          error.
	 * 
	 * NOTES: All the messages/strings in this project will be stored in 
	 *        separate files by language. Returns the specific message stored in
	 *        the array.
	 * 
	 *        EX.
	 *        <code>
	 *        String ok = Strings.getMessage(Messages.OK); 
	 *        </code>
	 ***************************************************************************/
	public static String getMessage(int i)
	{
		String path = Path.ROOT_PATH + Path.SETTINGS_FILENAME;
		FileIO f = new FileIO();
		try
		{
			if(messages == null || messages.length == 0 || !currentLanguage.equals(f.readFromAbsolutePath(path)[0]))
			{
				setupMessage();
			}
		} catch(IOException ex)
		{
			return null;
		}
		
		return messages[i];
	}

	/*************************************************************************** 
	 * FUNCTION: getAllMessages
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String[] getAllMessages()
	 * 
	 * RETURNS: The String array containing all the messages or null of there is
	 *          an error.
	 * 
	 * NOTES: Returns all the strings in in the string array depending on what 
	 *        Language is currently set.
	 * 
	 *        EX.
	 *        <code>
	 *        String str[] = Strings.getAllMessages();
	 *        System.out.println(str[Messages.OK]);
	 *        </code>
	 ***************************************************************************/
	public static String[] getAllMessages()
	{
		String path = Path.ROOT_PATH + Path.SETTINGS_FILENAME;
		FileIO f = new FileIO();
		try
		{
			if(messages == null || messages.length == 0 || !currentLanguage.equals(f.readFromAbsolutePath(path)[0]))
			{
				setupMessage();
			}
		} catch(IOException ex)
		{
			return null;
		}
		
		return messages;
	}

	/*************************************************************************** 
	 * FUNCTION: getLanguage
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String getLanguage()
	 * 
	 * RETURNS: The language that is currently being used. "English" or "French"
	 * 
	 * NOTES: Gets the current language that is currently being used.
	 ***************************************************************************/
	public static String getLanguage()
	{
		return currentLanguage;
	}
}
