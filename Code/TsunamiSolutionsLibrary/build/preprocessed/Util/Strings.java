package Util;




import java.io.IOException;

/**
 * Contains all the strings of English and French language used by this application. Needs Messages class for indexing.
 * @author Team Circno
 */
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
    
    /**
     * Sets up the Language to be French/English depending on the setting configuration.
     */
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
        }
        catch(IOException e)
        {
            throw new RuntimeException("FileReader error: " + e.getMessage());
        }
    }

    /**
     * All the messages/strings in this project will be stored in separate files by language.
     * Returns the specific message stored in the array.<br/>
     * Example: <br/>
     * <code>
     * String ok = Strings.getMessage(Messages.OK);
     * 
     * </code>
     *
     * @param i  The index of the message. Use the constants in Messages class. (eg. Messages.OK)
     * @return  The equivalent string of the message or null if there is an error.
     */
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
        }
        catch(IOException ex)
        {
            return null;
        }
        return messages[i];
    }
    
    /**
     * Returns all the strings in in the string array depending on what Language is currently set.
     * <br/>
     * Example: <br/>
     * <code>
     * String str[] = Strings.getAllMessages();<br/>
     * System.out.println(str[Messages.OK]);<br/>
     * 
     * </code>
     * @return The String array containing all the messages or null of there is an error.
     */
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
        }
        catch(IOException ex)
        {
            return null;
        }
        return messages;
    }
    
    /**
     * Gets the current language that is currently being used.
     * @return The language that is currently being used. "English" or "French".
     */
    public static String getLanguage(){
        return currentLanguage;
    }

}
