package Button2;

import Login.UI;
import Util.Base64;
import Util.Messages;
import Util.Strings;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;

/*******************************************************************************
 * CLASS: Sender
 *
 * FUNCTIONS: String send(String path, int messageFormat)
 *            boolean errorInResponse(char[] c)
 *            String getMsg()
 *
 * DATE: 2013-01-18
 * 
 * REFACTORS: 2013-05-14
 *            Aaron Lee
 *
 * REVISIONS: 2013-05-13
 *            Steve Lo
 * 
 * DESIGNER: Team Cirno
 *
 * PROGRAMMER: Team Cirno
 *
 * NOTES: This takes a path of UI information and sends it to the constructor.
 *******************************************************************************/
public class Sender
{
	static String returnMsg;
	
	/*************************************************************************** 
	 * FUNCTION: send
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
         * REVISIONS: 2013-05-13
         *            Steve Lo
         * 
	 * INTERFACE: String send(String path, int messageFormat)
	 *            String path - path of the MessageFormat files. If this was null, it uses the default location
	 *            int messageFormat - Network Message. Example: NetworkRequest.OK_MESSAGE
	 * 
	 * RETURN: Server connection response.
	 * 
	 * NOTES: Sender's sender method takes a path to the login information.
	 *        If path is null, default path will be used. 
	 *        If not null, that file should contain valid login information, 
	 *        that is encoded with PasswordEncoder from Configurator. 
	 ***************************************************************************/
	public static String send(String path, int messageFormat)
	{
		HttpConnection connection = null;
		String content, message, line;
		line = "NOTINIT";
		OutputStreamWriter writer = null;
		InputStreamReader reader = null;
		try
		{
			try
			{
				connection = (HttpConnection)Connector.open("http://devapi.slmonitor.com/safetylinexml.asmx/Cmd", Connector.READ);
				content = UI.getXMLMessage(path, null, messageFormat);
				content = Base64.encode(content);
				connection.setRequestMethod("POST");
				connection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
				connection.setRequestProperty("Content-Length", "" + Integer.toString(content.getBytes().length));
				writer = new OutputStreamWriter(connection.openOutputStream());

				message = "data=" + content;

				writer.write(message);
				writer.flush();
				reader = new InputStreamReader(connection.openInputStream());
			} catch(Exception ex)
			{   //This is hardcoded string because it might fail reading the Language files
				return "Resource for login information not found. \n" +
				       "Please run the Configurator.";
			} finally
			{
				if(writer != null)
				{
					writer.close();
				}
			}
			
			if(connection != null)
			{
				line = connection.getResponseMessage();

				// read in return message
				char[] c = new char[4000];
				int index = 0;
				while(reader.ready())
				{
					int result = reader.read(c, index, 4000 - index);
					if(result != -1)
					{
						index += result;
					}
				}

				// check return msg for error
				if(errorInResponse(c)) 
				{
					throw new SecurityException("bad credentials");
				}
			}
		} 
		
		catch(SecurityException exx)
		{
			line = Strings.getMessage(Messages.BAD_CREDENTIALS);
		}
		catch(IOException ex)
		{
			line = ex.getMessage();
		} finally
		{
			if(connection != null)
			{
				try
				{
					connection.close();
				} catch(IOException ex)
				{
					line = ex.getMessage();
				}
			}
			
			return line;
		}
		
		//return line;
	}

	/*************************************************************************** 
	 * FUNCTION: errorInResponse
	 * 
	 * DATE: 2013-05-14
	 * 
	 * DESIGNER: Aaron Lee
	 * 
	 * PROGRAMMER: Aaron Lee
	 * 
	 * INTERFACE: boolean errorInResponse(char[] c)
	 *            char[] c - the character array to be checked for an error
	 * 
	 * RETURN: true if an error has been found; otherwise false
	 * 
	 * NOTES: Looks for error in a a char array and reports the results.
	 ***************************************************************************/
	private static boolean errorInResponse(char[] c)
	{
		String s = new String(c);
		returnMsg = s;
		if(s.indexOf("error") > -1) 
		{
			return true;
		}

		return false;
	}

	/*************************************************************************** 
	 * FUNCTION: getMsg
	 * 
	 * DATE: 2013-05-13
	 * 
	 * DESIGNER: Steve Lo
	 * 
	 * PROGRAMMER: Steve Lo
	 * 
	 * INTERFACE: String getMsg()
	 * 
	 * RETURN: returns the message
	 * 
	 * NOTES: Returns the message stored in the returnMsg variable.
	 ***************************************************************************/
	public static String getMsg()
	{
		return returnMsg;
	}
}
