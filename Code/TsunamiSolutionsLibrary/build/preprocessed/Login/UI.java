package Login;

import Button2.NetworkRequest;
import Util.FileIO;
import Util.Path;
import java.io.IOException;

/**
 * Create objects that stores the user's username, password, and company id.
 *
 * @author Team Cirno
 * @version 1.0
 */
public class UI
{

    /**
     * user name of the user.
     */
    private String userName;
    /**
     * password of the user.
     */
    private String password;
    /**
     * company id of the user.
     */
    private int companyId;
    private static FileIO reader;

    /**
     * Constructor that creates a UI object.
     *
     * @param name the username of the user
     * @param pass the password if the user
     * @param companyId the company id of the user
     */
    public UI(final String name,
              final String pass,
              final int companyId)
    {
        userName = name;
        password = PasswordEncoder.encode(pass);
        this.companyId = companyId;
    }

    /**
     * Get the user name.
     *
     * @return the userName
     */
    public String getUserName()
    {
        return userName;
    }

    /**
     * Set the user name to the specific value.
     *
     * @param userName the userName to set
     */
    public void setUserName(String userName)
    {
        this.userName = userName;
    }

    /**
     * Get the password.
     *
     * @return the password, or empty string if password is null.
     */
    public String getPassword()
    {
        return PasswordEncoder.decode(password);
    }

    public String getUnDecodedPassword()
    {
        return password;
    }

    /**
     * Set the password to the specific value.
     *
     * @param password the password to set
     */
    public void setPassword(String password)
    {
        this.password = PasswordEncoder.encode(password);
    }

    /**
     * Get the company id.
     *
     * @return the companyId
     */
    public int getCompanyId()
    {
        return companyId;
    }

    /**
     * get user information from a file.
     *
     * @return the user information object
     * @throws IOException when file cannot be read.
     */
    public static UI getUIFromFile() throws IOException
    {
        reader = new FileIO();
        reader.readFromAbsolutePath(Path.ROOT_PATH + Path.LOGIN_FILENAME);
        String uname = reader.getLineAt(0);
        String passw = PasswordEncoder.decode(reader.getLineAt(1));
        int company = Integer.parseInt(reader.getLineAt(2));

        UI returnUI = new UI(uname, passw, company);

        return returnUI;
    }

    /**
     * get user information from a file.
     *
     * @param path The whole pathname to the file. Example: Path.ROOT_PATH + Path.LOGIN_FILENAME;
     * @return the user information object
     * @throws IOException when file cannot be read.
     */
    public static UI getUIFromFile(final String path) throws IOException
    {
        reader = new FileIO();
        reader.readFromAbsolutePath(path);
        String uname = reader.getLineAt(0);
        String passw = PasswordEncoder.decode(reader.getLineAt(1));
        int company = Integer.parseInt(reader.getLineAt(2));

        UI returnUI = new UI(uname, passw, company);

        return returnUI;
    }

    
    /**
     * Build a XML message with the correct username.
     *
     * @param path path to a user information file. Can be null. If null, read
     * user information from the default location. Default location:
     * file:///root1/Login.txt
     * @param msg additional message to be added with the message.
     * @return the XML message.
     * @throws IOException if file cannot be read.
     */
    public static String getXMLMessage(final String path,
                                       final String msg,
                                       final int messageFormat)
            throws IOException
    {
        UI userInfo;
        StringBuffer message = new StringBuffer();
        if (path == null)
        {
            //get from default path if path is null
            userInfo = getUIFromFile();
        }
        else
        {
            //get from user defined path.
            userInfo = getUIFromFile(path);
        }
        //read OKMessage template.
        switch (messageFormat)
        {
            case NetworkRequest.OK_MESSAGE:
                reader.readFileFromResource("OKMessageFormat");
                break;
            case NetworkRequest.END_MONITORING_MESSAGE:
                reader.readFileFromResource("END_MONITORINGMessageFormat");
                break;
            case NetworkRequest.DATA_MESSAGE:
            case NetworkRequest.OTHER_MESSAGE:
                throw new UnsupportedOperationException();
        }
        String[] allTheLines = reader.getLines();
        String line;

        for (int i = 0; i < allTheLines.length; i++)
        {
            line = allTheLines[i].trim();
            message.append(allTheLines[i]);
            if (line.startsWith("<company_id>"))
            {
                message.append(userInfo.getCompanyId());
            }
            if (line.startsWith("<user_id>"))
            {
                message.append(userInfo.getUserName());
            }
            if (line.startsWith("<password>"))
            {
                message.append(userInfo.getPassword());
            }
        }
        return message.toString();
    }

    /**
     * Set the company id to the specific value.
     *
     * @param companyId the companyId to set
     */
    public void setCompanyId(int companyId)
    {
        this.companyId = companyId;
    }

    /**
     * Returns a string representation of the object.
     *
     * @return a string representation of this object.
     */
    public String toString()
    {
        StringBuffer temp = new StringBuffer();
        temp.append(userName);
        temp.append("\n");
        temp.append(password);
        temp.append("\n");
        temp.append(companyId);
        return temp.toString();
    }
}
