import Button2.NetworkRequest;
import Button2.Sender;
import Util.Messages;
import Util.Popup;
import Util.Strings;
import java.util.Calendar;
import java.util.Date;
import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;


/**
 * Application for button2. Requests server to end monitoring for this specific
 * user.
 *
 * @author Team Cirno
 */
public class Button2 extends MIDlet
{
	/**
	 * Constant string representing the connection being OK.
	 */
	private static final String OK = "OK"; 

	/**
	 * When application is launched, startApp will be invoked.
	 */
	public void startApp()
	{
		String response;
		Popup p;
		Display disp = Display.getDisplay(this);
                Date d = new Date();
                Calendar c = Calendar.getInstance();
                int period;
		c.setTime(d);
                boolean ifOk = false;
                String time;
                
                
		try
		{
			String[] str = Strings.getAllMessages();

			p = new Popup(this, str[Messages.SENDING], false, null);
			disp.setCurrent(p);
			response = Sender.send(null, NetworkRequest.OK_MESSAGE);
                        ifOk = response.equals((OK));
                        p = new Popup(this, (response.equals(OK)) ? str[Messages.RECEIVED] : response, true, null);
                        str = Strings.getAllMessages();
                        response = Sender.send(null, NetworkRequest.DATA_MESSAGE);
                        period = extractTime();
                        if(ifOk){
                            int tempMin = c.get(Calendar.MINUTE)+period;
                            int tempHour = c.get(Calendar.HOUR_OF_DAY);
                            while(tempMin>59){
                                tempHour++;
                                if(tempMin==60){
                                    tempMin=0;
                                }
                                else{
                                    tempMin -= 60;
                                }
                            }
                            if(tempHour>23){
                                tempHour -= 24;
                            }
                            if(tempMin<10){
                                time = "Check-in completed.\nPlease check-in again at " + tempHour + ":0" + tempMin;
                            }
                            else {
                                time = "Check-in completed.\nPlease check-in again at " + tempHour + ":" + tempMin;
                            }
                            p = new Popup(this, time, true, null);
                            disp.setCurrent(p);
                        }
		}catch(Exception e)
		{
		   //most likely to happen if there is an error in Strings.getAllMessages()
		   Popup pop = new Popup(this, "Please login using the Configurator first.", true, null); 
		   disp.setCurrent(pop);
		}
	}

	public void pauseApp()
	{
	}

	public void destroyApp(boolean unconditional)
	{
	}
        
        public int extractTime(){
            String xmlMsg = Sender.getMsg();
            String open = "&lt;get_next&gt;&lt;command_result&gt;";
            int length = open.length();
            String msg = xmlMsg;
            int cmdStart = msg.indexOf(open);
            int timeStart = cmdStart + length;
            String subStr = msg.substring(timeStart);
            int timeEnd = subStr.indexOf("&");
            String time = subStr.substring(0, timeEnd);
            
            return (int) Math.ceil(Double.parseDouble(time)/60); 
        }
}
