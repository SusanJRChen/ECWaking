#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdbool.h>
#include<ugpio/ugpio.h>
#include <time.h>

int getTime(char *&line) //function that will return time 
{

}
int getDate(char *&line) //function that will return date
{

}
int getWeather(char *&line) //function that will return weather
{

}
int getAlarm(char *&line, bool alarm) //function that will return the alarm
{

}
int main(int argc, char **argv, char **envp)
{
    bool alarm = false; //determines whether an alarm is active or not

    char line1[20];
    char line2[40]; //line 2 will control lines 2 and 4 hence 40 lines
    char line3[20];
    char command[200]; //command meant to sent to bash to control display


    while(1)
    {
        //insert logic for buttons here
        if(getTime(line1))
        {
            //insert any errors and logging
        }
        if(getDate(line2))
        {

        }
        if(getWeather(line3))
        {

        }
        if(getAlarm(line2)) //outputs if there is a current alarm on the fourth day
        {

        }

        //combine all the lines to output and send it out
        snprintf(command, 200 "\"python /FireOnion_I2C_LCD/src/lcd.py - a 0x27 --line1=\"%s\" --line2=\"%s\" --line3=\"%s\"\"", line1, line2, line3);
        system(command);

        usleep(500000); //sleep for 0.5 seconds
    }

    //below is sample code of how the time class works and outputs data
    time_t tm;
    tm = time(NULL);
    printf("Current Time: %s", ctime(&tm));

    time_t my_time;
    struct tm *timeinfo;
    time(&my_time);
    timeinfo = localtime(&my_time);
    printf("year-> %d \n", timeinfo->tm_year + 1900); //
    printf("month-> %d \n", timeinfo->tm_mon + 1);
    printf("date-> %d \n", timeinfo->tm_mday);
    printf("hour-> %d \n", timeinfo->tm_hour);
    printf("minutes-> %d \n", timeinfo->tm_min);
    printf("seconds-> %d \n", timeinfo->tm_sec);
    return 0;
}