/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

import javax.microedition.midlet.*;
import com.sonimtech.loneworker.*;
import javax.microedition.lcdui.*;

/**
 * @author anup.s
 */
public class ModeChangers extends MIDlet {

	boolean firstTime = true;
	Form frm;

	public void startApp() {
		if (firstTime) {

			frm = new Form("Mode Changer");

			final Command exit = new Command("Exit", Command.EXIT, 0);
			final Command yes = new Command("Yes", Command.OK, 1);
			final Command no = new Command("No", Command.EXIT, 2);

			try {
				//Getting Current Mode, if unsuccesfull dont set mode
				int mode = LoneWorker.getInstance().getMode();
				int nextMode = getNextMode();
				if(mode == -1) {
					frm.append("Error getting current mode");
					frm.addCommand(exit);
				} else if(nextMode == -1) {
					frm.append("Error getting next mode");
					frm.addCommand(exit);
				} else if(mode == nextMode) {
					frm.append("Current mode is \n\n\"" + getMode(mode) + "\"");
					frm.addCommand(exit);
				} else {
					String confirmation = getAppProperty("CONFIRMATION");
					if (confirmation.equalsIgnoreCase("Yes")) {
						frm.append("Current mode is \n\n\"" + getMode(mode) + "\".\n\nDo you want to change the mode to \n\n\"" +getMode(nextMode)+"\" ?");
						frm.addCommand(yes);
						frm.addCommand(no);
					} else {
						changeMode();
						frm.addCommand(exit);
					}
				}
			} catch (Exception e) {
				frm.append("Getting mode unsuccessfull");
				frm.addCommand(exit);
			}

			frm.setCommandListener(new CommandListener() {

					public void commandAction(Command c, Displayable d) {
					if (c == exit) {
					System.out.println("Coming in Exit");
					notifyDestroyed();
					}
					if (c == no) {
					System.out.println("Coming in no");
					notifyDestroyed();
					}
					if (c == yes) {
					System.out.println("Coming in Yes");

					frm.deleteAll();
					frm.removeCommand(yes);
					frm.removeCommand(no);

					changeMode();
					frm.addCommand(exit);
					}
					}
					});

			System.out.println("Coming here to set Midlet display");
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

			int ret = LoneWorker.getInstance().setMode(modeJad);
			System.out.println("Coming inside set mode : " + ret);
			if (ret == LoneWorker.SUCCESS) {
				frm.append("Setting mode successfull");
				makeCall();
			} else {
				frm.append("Setting mode unsuccessfull");
			}
		} catch (Throwable e) {
			frm.append("Exception in setting mode");
		}
	}

	private String getMode(int mode) {
		String modeStr = "";
		if (mode == 0) {
			modeStr = "Disabled";
		} else if (mode == 1) {
			modeStr = "Simple";
		} else if (mode == 2) {
			modeStr = "Tracking";
		} else if (mode == 3) {
			modeStr = "Mandown";
		} else {
			modeStr = "Unknown";
		}
		System.out.println("Mode String in getMode is : " + modeStr);
		return modeStr;
	}

	private void makeCall() {
		String phoneNum = getAppProperty("PHONE-NUM");
		if (phoneNum == null || phoneNum.trim().equals("")) {
			notifyDestroyed();
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

	private void changeMode() {
		int nextMode = getNextMode();
		if(nextMode >= 0)
			setMode(nextMode);
	}

	private int getNextMode() {
		try {
			//Getting Current Mode,if unsuccesfull dont set mode
			int mode = LoneWorker.getInstance().getMode();
			String modeString = getAppProperty("LW-MODE");

			if (modeString.length() == 1) {
				int modeToSet = Integer.parseInt(modeString);
				return modeToSet;
			} else {
				int firstMode = Integer.parseInt(modeString.substring(0, 1));
				int secondMode = Integer.parseInt(modeString.substring(modeString.length() - 1));
				if (mode == firstMode) {
					return secondMode;
				} else {
					return firstMode;
				}
			}

		} catch (Exception e) {
			frm.append("Getting mode unsuccessfull");
			return -1;
		}
	}
}
