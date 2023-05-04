#include "decode.h"


string Decode::decode_urlencode(string param)
{
	string result = "";
	int i = 0;
	for (i = 0; i < param.length(); ++ i)
	{
		if (param[i] == '%') 
		{
			char x = param[i + 1];
			char y = param[i + 2];
			
			if ( (x-'0') > 9) 
			{
				x-= 'A';
				x+= 10;
			} else {
				x-='0';
				
			}
			
			if ( (y-'0') > 9) 
			{
				y-= 'A';
				y+= 10;
			} else {
				y-='0';
			}
			
			//cout << int(x * 16) << "," << int(y) << endl;
			result += (x * 16 + y);
			i += 2;
		}	
		else if (param[i] == '+')
		{
			result += " ";
		}
		else
			result += param[i];
		
		//cout << param[i] << endl;
	}
	return result;
}
