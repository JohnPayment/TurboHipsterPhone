/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

import javax.microedition.midlet.*;
import com.sonimtech.loneworker.LoneWorker;
import javax.microedition.lcdui.*;
import javax.microedition.io.*;
import javax.microedition.io.file.*;
import java.io.*;

/**
 * @author anup.s
 */
public class LoneWorkerExampleMidlet extends MIDlet implements CommandListener {

    Form form = new Form("LoneWorker");
    Command exit = new Command("Exit", Command.EXIT, 1);
    Command getMode = new Command("GetMode", Command.SCREEN, 2);
    Command setMode = new Command("SetMode", Command.SCREEN, 2);
    Command change = new Command("Change", Command.SCREEN, 2);
    LoneWorker loneworker;
    Display display;
    ChoiceGroup cg = new ChoiceGroup("Select Mode", ChoiceGroup.POPUP);
    final String FILE_NAME="Config.txt";
    public LoneWorkerExampleMidlet() {
        loneworker = LoneWorker.getInstance();
        for (int i = 0; i < LoneWorker.MODE_STRINGS.length; i++) {
            cg.append(LoneWorker.MODE_STRINGS[i], null);
        }
        form.append(cg);
        form.addCommand(exit);
        form.addCommand(getMode);
        form.addCommand(setMode);
        form.addCommand(change);
        form.setCommandListener(this);

    }

    public void startApp() {
        display = Display.getDisplay(this);
        display.setCurrent(form);
    }

    public void pauseApp() {
    }

    public void destroyApp(boolean unconditional) {
    }

    public void commandAction(Command c, Displayable d) {
        if (c == exit) {
            notifyDestroyed();
        } else if (c == getMode) {
            int mode = loneworker.getMode();
            Alert alrt = new Alert("MODE", "Mode :" + mode + "    " + LoneWorker.MODE_STRINGS[mode], null, AlertType.INFO);
            display.setCurrent(alrt);
        } else if (c == setMode) {
            int result = loneworker.setMode(cg.getSelectedIndex());
            if (result == LoneWorker.SUCCESS) {
                Alert alrt = new Alert("SUCCESS", "Mode Changing Successfull", null, AlertType.INFO);
                display.setCurrent(alrt);
            } else {
                Alert alrt = new Alert("FAILED", "Mode Changing Failed with error code " + result, null, AlertType.ERROR);
                display.setCurrent(alrt);
            }
        } else if (c == change) {
            form.append("Setting Configuration....");
            new Thread(){
                public void run(){
                    writeToFile();
                    int result = loneworker.setConfiguration(FILE_NAME);
                    try {
                        FileConnection fc = (FileConnection) Connector.open("file:///other/" + FILE_NAME);
                        fc.delete();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    if (result == LoneWorker.SUCCESS) {
                        Alert alrt = new Alert("SUCCESS", "Mode Changing Successfull", null, AlertType.INFO);
                        display.setCurrent(alrt);
                    } else {
                        Alert alrt = new Alert("FAILED", "Mode Changing Failed with error code " + result, null, AlertType.ERROR);
                        display.setCurrent(alrt);
                    }
                     form.append("Configuration Set.");
                }
            }.start();
            

        }
    }
    private void writeToFile() {
        OutputStream os = null;
        FileConnection fc = null;
        try {
            fc = (FileConnection) Connector.open("file:///other/" + FILE_NAME);
            if(fc.exists())
                fc.delete();
            fc.create();
            os = fc.openOutputStream();
            /* BatteryCheck=0
            BatteryCheckInterval=600
            BatteryFailureReporting=1
            BatteryFailureThreshold=20
            AwakeCheck=0
            AwakeCheckInterval=3600
            AwakeFailureReporting=3
             */
            String keyValues = "BatteryCheck=0" + "\r\n"
                    + "BatteryCheckInterval=599" + "\r\n"
                    + "BatteryFailureReporting=1" + "\r\n"
                    + "BatteryFailureThreshold=19" + "\r\n"
                    + "AwakeCheck=0" + "\r\n"
                    + "AwakeCheckInterval=3550" + "\r\n"
                    + "AwakeFailureReporting=3"+ "\r\n";
            os.write(keyValues.getBytes());
            os.flush();

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                os.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
            try {
                fc.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
