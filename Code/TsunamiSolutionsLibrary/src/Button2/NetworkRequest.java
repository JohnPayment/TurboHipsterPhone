package Button2;

/*******************************************************************************
 * CLASS: NetworkRequest
 *
 * FUNCTIONS: N/A
 *
 * DATE: 2013-01-18
 *
 * DESIGNER: Team Cirno
 *
 * PROGRAMMER: Team Cirno
 *
 * NOTES: Enums for network messages format. This will be used as a parameter 
 *        for Sender.
 * 
 *        EX. Sender.send(null, NetworkRequest.OK_MESSAGE);
 *******************************************************************************/
public class NetworkRequest 
{
	public static final int OK_MESSAGE = 0;
	public static final int END_MONITORING_MESSAGE = 1;
	public static final int DATA_MESSAGE = 2;
	public static final int OTHER_MESSAGE = 3;
}
