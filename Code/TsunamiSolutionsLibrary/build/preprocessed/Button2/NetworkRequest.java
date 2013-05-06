package Button2;

/**
 * Enums for network messages format. This will be used as a parameter for Sender. <br/>
 * Example: <br/>
 * <code>
 * Sender.send(null, NetworkRequest.OK_MESSAGE);
 * </code>
 * @author Team Cirno
 */
public class NetworkRequest {
    public static final int OK_MESSAGE = 0;
    public static final int END_MONITORING_MESSAGE = 1;
    public static final int DATA_MESSAGE = 2;
    public static final int OTHER_MESSAGE = 3;
}
