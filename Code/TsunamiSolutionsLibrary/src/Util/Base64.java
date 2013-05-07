package Util;

/**
 * Base64 encoder.
 * @author Team Cirno
 */
public final class Base64
{

    /**
     * dictionary of characters used in URL safe base64 encoding
     */
    private static String[] dictionary =
    {
        "A",//00
        "B",//01
        "C",//02
        "D",//03
        "E",//04
        "F",//05
        "G",//06
        "H",//07
        "I",//08
        "J",//09
        "K",//10
        "L",//11
        "M",//12
        "N",//13
        "O",//14
        "P",//15
        "Q",//16
        "R",//17
        "S",//18
        "T",//19
        "U",//20
        "V",//21
        "W",//22
        "X",//23
        "Y",//24
        "Z",//25
        "a",//26
        "b",//27
        "c",//28
        "d",//29
        "e",//30
        "f",//31
        "g",//32
        "h",//33
        "i",//34
        "j",//35
        "k",//36
        "l",//37
        "m",//38
        "n",//39
        "o",//40
        "p",//41
        "q",//42
        "r",//43
        "s",//44
        "t",//45
        "u",//46
        "v",//47
        "w",//48
        "x",//49
        "y",//50
        "z",//51
        "0",//52
        "1",//53
        "2",//54
        "3",//55
        "4",//56
        "5",//57
        "6",//58
        "7",//59
        "8",//60
        "9",//61
        "%2B",//62
        "%2F"//63
    };

    /**
     * encode the string in URL safe base64
     *
     * @param in the string to encode
     *
     * @return the encoded string
     */
    public static String encode(String in)
    {
        byte[] b = in.getBytes();
        int length = b.length;
        StringBuffer out = new StringBuffer();
        int i;

        //as long as the byte array contains bytes that can be grouped into 3
        for (i = 0; i + 2 < length; i += 3)
        {
            //encodes and adds the octets
            out.append(dictionary[((b[i] & 0xFC) >>> 2)]);
            out.append(dictionary[((b[i] & 0x3) << 4) | ((b[i + 1] & 0xF0) >>> 4)]);
            out.append(dictionary[((b[i + 1] & 0xF) << 2) | ((b[i + 2] & 0xC0) >>> 6)]);
            out.append(dictionary[(b[i + 2] & 0x3F)]);
        }
        //check to see if there are bytes that are not converted and pad if so
        if (length % 3 == 2)
        {
            out.append(dictionary[((b[i] & 0xFC) >>> 2)]);
            out.append(dictionary[((b[i] & 0x3) << 4 | (b[(i + 1)] & 0xF0) >>> 4)]);
            out.append(dictionary[((b[(i + 1)] & 0xF) << 2)]);
            out.append("%3D");

        }
        else if (length % 3 == 1)
        {
            out.append(dictionary[((b[i] & 0xFC) >>> 2)]);
            out.append(dictionary[((b[i] & 0x3) << 4)]);
            out.append("%3D%3D");
        }
        return new String(out);
    }
}
