package Configurator;


import Util.FileIO;
import Util.Messages;
import Util.Path;
import Util.Strings;
import java.io.IOException;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.midlet.*;


/**
 * This is the interface that will allow the user to change the settings of
 * their phone.
 *
 * @author Team Cirno
 */
public class Configurator
        extends MIDlet
{
    /**
     * Number of lines in the Settings file. (language preference, mandown, fall detection)
     */
    public static final int SETTINGS_LINES = 3;

    /**
     * the display. (screen of the phone, for more information, please read J2ME
     * documentation)
     */
    private static Display disp;

    /**
     * the settings screen.
     */
    private Settings settings;

    /**
     * the languages screen.
     */
    private Languages languages;

    /**
     * the login screen.
     */
    private Login login;

    /**
     * Structure to hold the data of the Settings file
     */
    private SettingStruct settingStruct;

    /**
     * Constructs a configurator.
     * Reads from a file to set up the Settings (Language, mandown, fall detection)
     * If the file doesn't exist, it will load English as preferred language, mandown as off and fall detection as off.
     */
    public Configurator()
    {
        settingStruct = new SettingStruct();
        try
        {
            String path = Path.ROOT_PATH + Path.SETTINGS_FILENAME;
            FileIO f = new FileIO();
            String[] str = f.readFromAbsolutePath(path);
            if(str == null || str.length == 0)
            {
                f.writeTextFile(Path.SETTINGS_FILENAME,
                                "English\nF\nF\n");
            }
            else
            {
                if(str != null && str.length == SETTINGS_LINES)
                {
                    settingStruct.Language = str[0];
                    settingStruct.Mandown = str[1].equals("T");
                    settingStruct.Fall = str[2].equals("T");
                }
            }
        }
        catch(IOException ex)
        {
            notifyDestroyed();
        }
    }

    /**
     * Method to cause the screen to change to one of the screens.
     *
     * @param d the screen you want to change to
     *
     * @return true if successfully changed screens, false otherwise
     */
    public static boolean moveto(Displayable d)
    {
        if(d != null)
        {
            disp.setCurrent(d);
            return true;
        }
        return false;
    }

    /**
     * initialization preformed after this MIDlet has been constructed.
     * initializes the display and screens.
     */
    private void init()
    {
        disp = Display.getDisplay(this);
        String[] str = Strings.getAllMessages();
        String[] languageItems = new String[]{ str[Messages.ENGLISH], str[Messages.FRENCH] };
        
        String[] settingsItemgs = new String[]
        {
            str[Messages.TOGGLE] + " " + str[Messages.MANDOWN] + ": " + str[(settingStruct.Mandown) ?
                                                                            Messages.ON :
                                                                            Messages.OFF],
            str[Messages.TOGGLE] + " " + str[Messages.FALL_DETECTION] + ": " + str[(settingStruct.Fall ?
                                                                                    Messages.ON :
                                                                                    Messages.OFF)],
            str[Messages.LOGIN],
            str[Messages.LANGUAGES]
        };

        settings = new Settings(str[Messages.SETTINGS], settingsItemgs, this);
        settings.setToggleableState(Settings.FALL, settingStruct.Fall);
        settings.setToggleableState(Settings.MANDOWN, settingStruct.Mandown);

        login = new Login(str[Messages.LOGIN], this);

        languages = new Languages(str[Messages.LANGUAGES], languageItems, this);
    }

    /**
     * Main entry point of the MIDlet. starts the display as the settings
     * screen.
     */
    public void startApp()
    {
        init();
        disp.setCurrent(settings);
    }

    /**
     * When this MIDlet is moved to the background, it will destroy itself.
     */
    public void pauseApp()
    {
        notifyDestroyed();
    }

    /**
     * Structure that holds all the data related to Settings.
     */
    private class SettingStruct
    {
        public String Language;
        public boolean Mandown;
        public boolean Fall;
        public SettingStruct()
        {
            Language = "English";
        }
    }

    /**
     * Cleanup before destruction.
     *
     * @param unconditional if the destruction must happen or not
     */
    public void destroyApp(boolean unconditional)
    {
        try
        {
            String path = Path.ROOT_PATH + Path.SETTINGS_FILENAME;
            StringBuffer buff = new StringBuffer();
            FileIO f = new FileIO();
            String[] lines = f.readFromAbsolutePath(path);
            buff.append(lines[0])
                    .append("\n")
                    .append((settings.getToggleableState(Settings.MANDOWN) ? "T" : "F"))
                    .append("\n")
                    .append((settings.getToggleableState(Settings.FALL) ? "T" : "F"))
                    .append("\n");

            f.writeTextFile(Path.SETTINGS_FILENAME, buff.toString());
            notifyDestroyed();
        }
        catch(IOException ex)
        {
            notifyDestroyed();
        }


    }

    /**
     * Settings screen getter.
     *
     * @return settings screen.
     */
    public Settings getSettingScreen()
    {
        return settings;
    }

    /**
     * Languages screen getter.
     *
     * @return language screen.
     */
    public Languages getLanguageScreen()
    {
        return languages;
    }

    /**
     * Login screen getter.
     *
     * @return login screen.
     */
    public Login getLoginScreen()
    {
        return login;
    }

}
