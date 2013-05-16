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

/**
 * Sender takes a path of UI information, 
 * and sends it to the constructor.
 * @author Team Cirno
 */
public class Sender
{
    /**
     * Sender's sender method takes a path to the login information.
     * If path is null, default path will be used. 
     * If not null, that file should contain valid login information, 
     * that is encoded with PasswordEncoder from Configurator. 
     * @param path path of the MessageFormat files. If this was null, it uses the default location/
     * @param messageFormat Network Message. Example: NetworkRequest.OK_MESSAGE
     * @return Server connection response.
     */
    static String returnMsg;
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
            }
            catch (Exception ex)
            {   //This is hardcoded string because it might fail reading the Language files
                return "Resource for login information not found. \n" +
                        "Please run the Configurator.";
            }
            finally
            {
                if (writer != null)
                {
                    writer.close();
                }
            }
            if (connection != null)
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
                String s = new String(c);
                returnMsg = s;
                if ( s.indexOf( "error" ) > -1 ) {
                    throw new SecurityException("bad credentials");
                }
            }
        }
        catch (SecurityException exx)
        {
            line = Strings.getMessage(Messages.BAD_CREDENTIALS);
        }
        catch (IOException ex)
        {
            line = ex.getMessage();
        }
        finally
        {
            if (connection != null)
            {
                try
                {
                    connection.close();
                }
                catch (IOException ex)
                {
                    line = ex.getMessage();
                }
            }
            return line;
        }
        //return line;

    }
    
    public static String getMsg(){
        return returnMsg;
    }
}
