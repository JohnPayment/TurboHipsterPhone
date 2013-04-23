/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

import javax.microedition.midlet.*;
import com.sonimtech.j2me.loneworker.LoneWorkerNative;
import javax.microedition.lcdui.*;

/**
 * @author anup.s
 */
public class ModeChanger2Midlet extends MIDlet {

    boolean firstTime = true;
    Form frm;

    public void startApp() {
        if (firstTime) {
            frm = new Form("Mode Changer");
            final Command exit = new Command("Exit", Command.EXIT, 1);
            frm.addCommand(exit);
            frm.setCommandListener(new CommandListener() {

                public void commandAction(Command c, Displayable d) {
                    if (c == exit) {
                        notifyDestroyed();
                    }
                }
            });
            try {
                //Getting Current Mode,if unsuccesfull dont set mode
                int mode = LoneWorkerNative.getMode();
                String modeString = getAppProperty("LW-MODE");
                if (modeString.length() == 1) {
                    int modeToSet = Integer.parseInt(modeString);
                    setMode(modeToSet);
                } else {
                    int firstMode = Integer.parseInt(modeString.substring(0, 1));
                    int secondMode = Integer.parseInt(modeString.substring(modeString.length() - 1));
                    if (mode == firstMode) {
                        setMode(secondMode);
                    } else {
                        setMode(firstMode);
                    }
                }

            } catch (Exception e) {
                frm.append("Getting mode unsuccessfull");
            }
            Display.getDisplay(this).setCurrent(frm);
            firstTime = false;
        }
    }

    public void pauseApp() {
    }

    public void destroyApp(boolean unconditional) {
    }

    private void setMode(int modeJad) {
        try {

            //Setting mode value
            int ret = LoneWorkerNative.setMode(modeJad);
            if (ret == LoneWorkerNative.SUCCESS) {
                frm.append("Setting mode successfull");
            }
            makeCall();
        } catch (Throwable e) {
            frm.append("Setting mode unsuccessfull");
        }
    }

    private void makeCall() {
        String phoneNum = getAppProperty("PHONE-NUM");
        if (phoneNum == null || phoneNum.trim().equals("")) {
            return;
        }
        try {
            platformRequest("tel:" + phoneNum);
        } catch (SecurityException se) {
            frm.append("Please give permission to make a call");
        } catch (javax.microedition.io.ConnectionNotFoundException cnfe) {
            frm.append("Error while making call");
        }
    }
}
