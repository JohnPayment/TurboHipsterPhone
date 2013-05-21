package Login;

import Button2.NetworkRequest;
import Util.FileIO;
import Util.Path;
import java.io.IOException;

/*******************************************************************************
 * CLASS: UI
 *
 * FUNCTIONS: UI(final String name, final String pass, final int companyId)
 *            String getUserName()
 *            void setUserName(String userName)
 *            setUserName(String userName)
 *            String getPassword()
 *            String getUnDecodedPassword()
 *            void setPassword(String password)
 *            int getCompanyId()
 *            UI getUIFromFile()
 *            String getXMLMessage(final String path, final String msg, final int messageFormat)
 *            void setCompanyId(int companyId)
 *            String toString()
 *
 * DATE: 2013-01-18
 *
 * DESIGNER: Team Cirno
 *
 * PROGRAMMER: Team Cirno
 *
 * NOTES: Create objects that stores the user's username, password, and company 
 *        id.
 *******************************************************************************/
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

	/*************************************************************************** 
	 * FUNCTION: UI
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: UI(final String name, final String pass, final int companyId)
	 *            final String name - the username of the user
	 *            final String pass - the password if the user
	 *            final int companyId - the company id of the user
	 * 
	 * NOTES: Constructor for the UI class
	 ***************************************************************************/
	public UI(final String name, final String pass, final int companyId)
	{
		userName = name;
		password = PasswordEncoder.encode(pass);
		this.companyId = companyId;
	}

	/*************************************************************************** 
	 * FUNCTION: getUserName
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String getUserName()
	 * 
	 * RETURN: The stored username
	 * 
	 * NOTES: getter function for the userName
	 ***************************************************************************/
	public String getUserName()
	{
		return userName;
	}

	/*************************************************************************** 
	 * FUNCTION: setUserName
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String setUserName(String userName)
	 *            String username - The new username
	 * 
	 * RETURN: void
	 * 
	 * NOTES: setter function for the userName
	 ***************************************************************************/
	public void setUserName(String userName)
	{
		this.userName = userName;
	}

	/*************************************************************************** 
	 * FUNCTION: getPassword
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String getPassword()
	 * 
	 * RETURN: The stored Password
	 * 
	 * NOTES: getter function for the Password. Decodes it before printing.
	 ***************************************************************************/
	public String getPassword()
	{
		return PasswordEncoder.decode(password);
	}

	/*************************************************************************** 
	 * FUNCTION: getPassword
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String getPassword()
	 * 
	 * RETURN: The stored Password
	 * 
	 * NOTES: getter function for the Password.
	 ***************************************************************************/
	public String getUnDecodedPassword()
	{
		return password;
	}

	/*************************************************************************** 
	 * FUNCTION: setPassword
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String setPassword(String password)
	 *            String password - The new Password
	 * 
	 * RETURN: void
	 * 
	 * NOTES: setter function for the Password. Also encodes it.
	 ***************************************************************************/
	public void setPassword(String password)
	{
		this.password = PasswordEncoder.encode(password);
	}

	/*************************************************************************** 
	 * FUNCTION: getCompanyId
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: int getCompanyId()
	 * 
	 * RETURN: The stored ID
	 * 
	 * NOTES: getter function for the company ID.
	 ***************************************************************************/
	public int getCompanyId()
	{
		return companyId;
	}
	
	/*************************************************************************** 
	 * FUNCTION: setCompanyId
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String setCompanyId(int companyId)
	 *            int companyId - The id
	 * 
	 * RETURN: void
	 * 
	 * NOTES: setter function for the Company ID
	 ***************************************************************************/
	public void setCompanyId(int companyId)
	{
		this.companyId = companyId;
	}

	/*************************************************************************** 
	 * FUNCTION: getUIFromFile
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: UI getUIFromFile()
	 * 
	 * RETURN: the user information object
	 * 
	 * THROWS: IOException when file cannot be read.
	 * 
	 * NOTES: Opens the Login file and generates a UI object from it. 
	 ***************************************************************************/
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

	/*************************************************************************** 
	 * FUNCTION: getUIFromFile
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: UI getUIFromFile(final String path)
	 *            final String path - The pathname of the file to be read
	 * 
	 * RETURN: the user information object
	 * 
	 * THROWS: IOException when file cannot be read.
	 * 
	 * NOTES: Opens the Login file and generates a UI object from it. 
	 ***************************************************************************/
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

	/*************************************************************************** 
	 * FUNCTION: getUIFromFile
	 * 
	 * DATE: 2013-01-30
	 * 
         * REVISIONS: 2013-05-18
	 *            Luke Tao
         * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String getXMLMessage(final String path, final String msg, final int messageFormat)
	 *            final String path - The pathname of the file to be read
	 *            final String msg - additional message to be added with the message.
	 *            final int messageFormat - Enum case indicating the format of the message
	 * 
	 * RETURN: The XML message.
	 * 
	 * THROWS: IOException when file cannot be read.
	 * 
	 * NOTES: Build a XML message with the correct username. 
	 ***************************************************************************/
	public static String getXMLMessage(final String path, final String msg, final int messageFormat) throws IOException
	{
		UI userInfo;
		StringBuffer message = new StringBuffer();
		if(path == null)
		{
			//get from default path if path is null
			userInfo = getUIFromFile();
		} else
		{
			//get from user defined path.
			userInfo = getUIFromFile(path);
		}
		
		//read OKMessage template.
		switch(messageFormat)
		{
			case NetworkRequest.OK_MESSAGE:
				reader.readFileFromResource("OKMessageFormat");
				break;
			case NetworkRequest.END_MONITORING_MESSAGE:
				reader.readFileFromResource("END_MONITORINGMessageFormat");
				break;
			case NetworkRequest.DATA_MESSAGE:
				reader.readFileFromResource("DATAMessageFormat");
				break;
			case NetworkRequest.OTHER_MESSAGE:
				throw new UnsupportedOperationException();
		}
		
		String[] allTheLines = reader.getLines();
		String line;

		for (int i = 0; i < allTheLines.length; i++)
		{
			line = allTheLines[i].trim();
			message.append(allTheLines[i]);
			if(line.startsWith("<company_id>"))
			{
				message.append(userInfo.getCompanyId());
			}
			if(line.startsWith("<user_id>"))
			{
				message.append(userInfo.getUserName());
			}
			if(line.startsWith("<password>"))
			{
				message.append(userInfo.getPassword());
			}
			if(line.startsWith("<latitude>"))
			{
				message.append("-500");
			}
			if(line.startsWith("<longitude>"))
			{
				message.append("500");
			}
		}
		return message.toString();
	}

	/*************************************************************************** 
	 * FUNCTION: toString
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String toString()
	 * 
	 * RETURN: String containing the Username, password and Company ID
	 * 
	 * NOTES: Returns a string containing the Username, password and Company ID.
	 ***************************************************************************/
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
