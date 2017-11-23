#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<stdbool.h>

/*
Woooooo it’s a ghost
Spooky
fuck your ghost

How to curl:

Other things to note:
You can view content of variable content with:
echo $content
You can view content of file content.txt with:
cat /content.txt

From the onion omega, run:

To get longitude and latitude from IP address, run:

curl freegeoip.net/json/[IPADDRESS] > coordinates.txt

Parse through coordinates.txt to get longitude and latitude

Given any longitude and latitude, we can get the woeid(specifies where the place is) with:

curl query.yahooapis.com/v1/public/yql -d q="select woeid from geo.places where text=\"([LONGITUDE], [LATITUDE])\"" -d format=json > woeid.txt

which stores the woeid in weather.txt

Using the woeid, we can get information on the location’s weather with:
curl query.yahooapis.com/v1/public/yql -d q="select units,items.forecast from weather.forecast where woeid= [WOEID] and item.forecast.date" -d format=json > weather.txt

This stores the information in weather.txt
*/

char * substring(char * string, int position, int length){
    char *pointer;
    int c;
    
    pointer = malloc(length+1);
    
    if (pointer == NULL)
    {
        printf("Unable to allocate memory.\n");
        exit(1);
    }
    
    for (c = 0 ; c < length ; c++)
    {
        *(pointer+c) = *(string+position-1);
        string++;
    }
    
    *(pointer+c) = '\0';
    
    return pointer;
}

bool getLongandLat(char * buff, char ** latitude, char ** longitude){
    char * result= strstr(buff, "latitude\":");
    int LatitudeIndex = result - buff + 11;
    result= strstr(buff, "longitude\":");
    int LongitudeIndex = result - buff + 12;
    int length = LongitudeIndex - LatitudeIndex - 13;
    
    *longitude = substring(buff, LongitudeIndex, length);    
    *latitude = substring(buff, LatitudeIndex, length);
    
    return true;
}

bool getWoeid(char * buff, char ** woeid){
    int woeidIndex = strstr(buff, "woeid\":") - buff + 9;
    int woeidEndIndex = strstr(buff, "\"}}") - buff + 1;
    *woeid = substring(buff, woeidIndex, woeidEndIndex - woeidIndex);
    
    return true;
}

bool getTemperature(char * buff, char ** high, char ** low){
	int highIndex = strstr(buff, "high\":\"") - buff + 8;
	int lowIndex = strstr(buff, "low\":\"") - buff + 7;
	int textIndex = strstr(buff, "text\":\"") - buff + 8;
	
	*high = substring(buff, highIndex, lowIndex - highIndex - 9);
	*low = substring(buff, lowIndex, textIndex - lowIndex - 10);
		
	return true;	
}

bool getLineWithString(char * filename, char * string, char * buff){
	FILE * file;
	file = fopen(filename, "r");
	
	if (file == NULL)
		return false;
	
	while (fscanf(file, "%s", buff) != EOF){
		if (strstr(buff, string))
			return true;
	}
	return false;
}

int main(int argc, char** argv)
{
    char * IP;
    FILE * content;
    char buff[255];
	
	char * woeid;
	char * longitude;
	char * latitude;
	char * high;
	char * low;
	
    
    IP = "129.97.124.0";
	
	getLineWithString("content.txt", "high", buff);
	getTemperature(buff, &high, &low);
	
	printf("High: %s , Low: %s ", high, low);
    
    return 0;
}
