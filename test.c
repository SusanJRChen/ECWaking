#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

echo $(curl freegeoip.net/json/[IPADDRESS]) > content.txt

Parse through content.txt to get longitude and latitude

Given any longitude and latitude, we can get the woeid(specifies where the place is) with:

echo $(curl query.yahooapis.com/v1/public/yql -d q="select woeid from geo.places where text=\"([LONGITUDE], [LATITUDE])\"" -d format=json) > $content.txt

which stores the woeid in content.txt

Using the woeid, we can get information on the location’s weather with:

echo $(curl query.yahooapis.com/v1/public/yql -d q="select wind from weather.forecast where woeid= [WOEID]" -d format=json) > $content.txt

This stores the information in content.txt

I have not test this.
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

char * getLongitude(char * buff){
    char * latitude;
    char * longitude;
    char * result= strstr(buff, "latitude\":");
    int LatitudeIndex = result - buff + 11;
    result= strstr(buff, "longitude\":");
    int LongitudeIndex = result - buff + 12;
    int length = LongitudeIndex - LatitudeIndex - 13;
    
    longitude = substring(buff, LongitudeIndex, length);
    
    return longitude;
}

char * getLatitude(char * buff){
    char * latitude;
    char * longitude;
    char * result= strstr(buff, "latitude\":");
    int LatitudeIndex = result - buff + 11;
    result= strstr(buff, "longitude\":");
    int LongitudeIndex = result - buff + 12;
    int length = LongitudeIndex - LatitudeIndex - 13;
    
    latitude = substring(buff, LatitudeIndex, length);
    
    return latitude;
}

char * getWoeid(char * buff){
    char * woeid;
    int woeidIndex = strstr(buff, "woeid\":") - buff + 9;
    int woeidEndIndex = strstr(buff, "\"}}") - buff + 1;
    woeid = substring(buff, woeidIndex, woeidEndIndex - woeidIndex);
    
    return woeid;
}

int main(int argc, char** argv)
{
    char * IP;
    FILE * content;
    char buff[255];
    
    IP = "129.97.124.0";
    content = fopen("content.txt", "r");
    
    if (content == NULL)
        return -1;
    
    int i = 1;
    while (fscanf(content, "%s", buff) != EOF) {
    }
    
    printf("%s\n", woeid);
    
    return 0;
}
