package Login;

import java.util.Random;

/*******************************************************************************
 * CLASS: PasswordEncoder
 *
 * FUNCTIONS: String encode(String password)
 *            String decode(String password)
 *            int getDigits(int n)
 *
 * DATE: 2013-01-18
 * 
 * REFACTORS: 2013-05-14
 *
 * DESIGNER: Team Cirno
 *
 * PROGRAMMER: Team Cirno
 *
 * NOTES: Encodes and Decodes a password
 *******************************************************************************/
public class PasswordEncoder
{
	/*************************************************************************** 
	 * FUNCTION: encode
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String encode(String password)
	 *            String password - the password to encode.
	 * 
	 * RETURN: The encoded password
	 * 
	 * NOTES: Encodes a password. Random a number from 1-8 as the key. Then 
	 *        multiply the byte of each letter of the password by the key. First
	 *        digit of the encoded string is the key. Then each letter of the
	 *        password is stored as a 3 digit number.
	 ***************************************************************************/
	public static String encode(String password)
	{
		if(password == null)
		{
			throw new IllegalArgumentException("password cannot be empty");
		}

		StringBuffer encodedString = new StringBuffer();
		Random random = new Random();
		
		//random key from 1 to 8
		int randNum = random.nextInt(8) + 1;
		
		//append the key
		encodedString.append(randNum);
		
		//transfrom password into bytes
		byte[] pwByte = password.getBytes();
		int[] processed = new int[pwByte.length];

		for(int i = 0; i < pwByte.length; ++i)
		{
			processed[i] = pwByte[i] * randNum;
			//if the processed number is not 3 digits.
			if(getDigits(processed[i]) == 2)
			{
				//append 0's
				encodedString.append('0');
			}
			
			//append the encoded letter to the string.
			encodedString.append(processed[i]);
		}
		
		return encodedString.toString();
	}

	/*************************************************************************** 
	 * FUNCTION: decode
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: String decode(String password)
	 *            String password - the password to decode.
	 * 
	 * RETURN: The decoded password
	 * 
	 * NOTES: Decode the encoded password. Get the first number as key, then 
	 *        divide each set of 3 digits by the key.
	 ***************************************************************************/
	public static String decode(String password)
	{
		if(password == null)
		{
			throw new IllegalArgumentException("password cannot be empty");
		}

		StringBuffer originalPassword = new StringBuffer();
		String passwordSub;
		
		//get the key.
		int key = Integer.parseInt(password.substring(0, 1));

		for(int i = 0; i < password.length() / 3; ++i)
		{
			//divide each set of 3 numbers by the key.
			passwordSub = password.substring(i * 3 + 1, i * 3 + 4);
			originalPassword.append((char)(Integer.parseInt(passwordSub) / key));
		}
		
		return originalPassword.toString();
	}

	/*************************************************************************** 
	 * FUNCTION: getDigits
	 * 
	 * DATE: 2013-01-30
	 * 
	 * DESIGNER: Team Cirno
	 * 
	 * PROGRAMMER: Team Cirno
	 * 
	 * INTERFACE: int getDigits(int n)
	 *            int n - The integer to parse
	 * 
	 * RETURN: the number of digits (up to 10)
	 * 
	 * NOTES: Returns the number of digits of a integer. This works upto 10 
	 *        digits. This method is faster than using a loop because it 
	 *        imitates binary search.
	 ***************************************************************************/
	/**
	 * Returns the number of digits of a integer. This works upto 10 digits.
	 * This method is faster than using a loop because it imitates binary search.
	 * @param n the integer to parse
	 *
	 * @return the number of digits the integer has
	 */
	public static int getDigits(int n)
	{
		if(n < 100000)
		{
			// 5 or less
			if(n < 100)
			{
				// 1 or 2
				if(n < 10)
				{
					return 1;
				} else
				{
					return 2;
				}
			} else
			{
				// 3 or 4 or 5
				if (n < 1000)
				{
					return 3;
				} else
				{
					// 4 or 5
					if (n < 10000)
					{
						return 4;
					} else
					{
						return 5;
					}
				}
			}
		} else
		{
			// 6 or more
			if (n < 10000000)
			{
				// 6 or 7
				if (n < 1000000)
				{
					return 6;
				} else
				{
					return 7;
				}
			} else
			{
				// 8 to 10
				if (n < 100000000)
				{
					return 8;
				} else
				{
					// 9 or 10
					if (n < 1000000000)
					{
						return 9;
					} else
					{
						return 10;
					}
				}
			}
		}
	}
}