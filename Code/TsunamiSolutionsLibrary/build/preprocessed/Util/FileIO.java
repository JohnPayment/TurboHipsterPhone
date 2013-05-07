package Util;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.Vector;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

/**
 * File IO that can read from resource and read/write to a local file.
 * @author Team Cirno
 */
public class FileIO
{

    private String filename;
    private Vector v;
    private String[] lines;

    /**
     * Construct a FileIO that can read file from resource or from an absolute path.
     */
    public FileIO()
    {
        v = new Vector();
    }

    /**
     * <p>
     * Reads the file that is passed in the parameter. The file should be inside
     * the folder where FileIO resides. <br/>
     * Empty lines will be ignored (ie. A line with only new line character).<br/>
     * A line with only spaces is not considered an empty line.<br/>
     * Sample usage: </p><br/>
     * <code>
     * FileIO f = new FileIO();<br/>
     * String str = f.readFileFromResource("myfile2.txt"); <br/>
     * </code>
     * <p>
     * Note that the file name is CASE-SENSITIVE.
     *</p>
     * @throws IOException  thrown when there is a problem reading the file.
     * @return  String array that holds each line of the file or null if the file is empty.
     */
    public final String[] readFileFromResource(String fileName)
        throws IOException
    {
        if (fileName == null)
        {
            throw new IllegalArgumentException("pathname is null");
        }

        v.removeAllElements();
        InputStreamReader reader = new InputStreamReader(getClass().getResourceAsStream(fileName));

        if (reader == null)
        {
            return null;
        }
        String line;

        // Read a single line from the file. null represents the EOF.
        while ((line = readLine(reader)) != null)
        {
            v.addElement(line);
        }
        reader.close();
        lines = new String[v.size()];

        for (int i = 0; i < lines.length; i++)
        {
            lines[i] = (String)v.elementAt(i);
        }

        filename = fileName;
        return lines;
    }

    /**
     * Reads from the file character by character stopping at the new line
     * character or EOF.
     *
     * @param reader - The reader to read the file.
     *
     * @return - The line that was read.
     *
     * @throws IOException - Thrown when there is a problem reading the file.
     */
    private String readLine(InputStreamReader reader)
        throws IOException
    {

        // Test whether the end of file has been reached. If so, return null.
        int readChar = reader.read();
        if (readChar == -1)
        {
            return null;
        }

        StringBuffer string = new StringBuffer("");

        // Read until end of file or new line
        while (readChar != -1 && readChar != '\n')
        {

            // Append the read character to the string. Some operating systems
            // such as Microsoft Windows prepend newline character ('\n') with
            // carriage return ('\r'). This is part of the newline character
            // and therefore an exception that should not be appended to the
            // string.
            if (readChar != '\r')
            {
                string.append((char)readChar);
            }

            // Read the next character
            readChar = reader.read();

        }
        return string.toString();
    }

    
    /**
     * Gets all the lines of previously read files.
     * <br/>Example: <br/>
     * <code>
     * FileIO r = new FileIO();<br/>
     * r.readFromResource("sample.txt");<br/>
     * String str[] = r.getLines();
     * </code>
     * @return The lines of the file that was read.
     */
    public String[] getLines()
    {
        return lines;
    }

    /**
     * Gets the line at nth element starting from 0 as the first line to n-1 as
     * the last line. May throw indexOutOfBounds if i is greater than or equal to the
     * number of lines that was stored.<br/>
     * Example: <br/>
     * <code>
     * FileIO r = new FileIO();<br/>
     * r.readFromAbsolutePath("sample");<br/>
     * int i = r.getNumberOfLines();<br/>
     * String lastLine = r.getLineAt(i - 1);<br/>
     * String firstLine = r.getLineAt(0);<br/>
     * 
     * </code>
     *
     * @param i  nth element to get. Example: getLineAt(0); will return the
     * first line.
     *
     * @return  the specified string.
     */
    public String getLineAt(int i)
    {
        return lines[i];
    }

    /**
     * Gets the filename the FileIO is currently holding.
     *
     * @return - The filename.
     */
    public String getFileName()
    {
        return filename;
    }

    /**
     * Gets the number of the lines of the file this current FileIO has.
     *
     * @return - number of lines of the file that was most recently read.
     */
    public int getNumberOfLines()
    {
        return v.size();
    }

    /**
     * Reads a file from an absolute file. String will be separated line by
     * line. <br/>
     * Empty lines will be ignored (ie. A line with only new line character).<br/>
     * A line with only spaces is not considered an empty line.<br/>
     * Example: <br/>
     * <code>
     * FileIO r = new FileIO();<br/>
     * String[] lines = r.readFromAbsolutePath(Path.ROOT_PATH + PATH.LOGIN_FILENAME);<br/>
     * </code>
     *
     * @param path  absolute path of the file.
     *
     * @return  String array that holds each line of the file or null if it failed to read the file.
     */
    public String[] readFromAbsolutePath(String path) throws IOException
    {
        InputStream is = null;
        FileConnection fc = null;
        String str;
        v = new Vector();
        lines = null;
        try
        {
            fc = (FileConnection)Connector.open(path,
                Connector.READ_WRITE);

            if (fc.exists())
            {
                int size = (int)fc.fileSize();
                is = fc.openInputStream();
                byte bytes[] = new byte[size];
                is.read(bytes,
                    0,
                    size);

                str = new String(bytes,
                    0,
                    size);
                StringBuffer buff = new StringBuffer();
                char c;

                for (int i = 0; i < str.length(); i++)
                {
                    c = str.charAt(i);
                    if (c != '\n' && c != '\r')
                    {
                        buff.append(c);
                    }
                    if (c == '\n')
                    {
                        if (buff.toString().length() > 0)
                        {
                            v.addElement(buff.toString());
                        }
                        buff.delete(0,
                            buff.length());
                    }
                }

                if (buff.length() > 0)
                {
                    v.addElement(buff.toString());
                }

                lines = new String[v.size()];
                for (int i = 0; i < v.size(); i++)
                {
                    lines[i] = (String)v.elementAt(i);
                }
            }
        }
        catch (IOException ioe)
        {
            throw new IOException();
        }
        finally
        {
            try
            {
                if (null != is)
                {
                    is.close();
                }
                if (null != fc)
                {
                    fc.close();
                }
            }
            catch (IOException e)
            {
                System.out.println(e.getMessage());
            }
        }
        return lines;
    }
    
    /**
     * Write contents to a file.
     *
     * @param fName - The file name along with the whole location. Example: Path.ROOT_NAME + Path.LOGIN_FILENAME
     * @param text - the content that will be saved in the file.
     *
     * @return value indicating if writing was a success or faliure
     */
    public boolean writeTextFile(String fName,
                                 String text)
    {
        
        fName = Path.ROOT_PATH + fName;

        //open the stream
        OutputStream os = null;
        FileConnection fc = null;
        try
        {
            fc = (FileConnection)Connector.open(fName, Connector.READ_WRITE);
            //if the file doesnt exist, create it
            if (fc.exists())
            {
                fc.delete();
            }
            fc.create();

            os = fc.openDataOutputStream();
            os.write(text.getBytes());
            fc.setHidden(false);
        }
        catch (IOException e)
        {
            return false;
        }
        finally
        {
            try
            {
                if (null != os)
                {
                    os.close();
                }
                if (null != fc)
                {
                    fc.close();
                }
            }
            catch (IOException e)
            {
                return false;
            }
        }
        return true;
    }

    
}
