#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*#include<unistd.h>
#include<fcntl.h>
#include<stdbool.h>
#include<ugpio/ugpio.h>*/
#include <time.h>

enum LOGGING {TRACE, DEBUG, INFO, WARNING, ERROR, FATAL};


int getLoggingTime(char ** line) //function that will return time
{
    time_t tm;
    tm = time(NULL);
    
    time_t my_time;
    struct tm *timeinfo;
    time(&my_time);
    timeinfo = localtime(&my_time);
    
    snprintf(*line, 20, "%d:%d:%d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
    return 1;
}
int getDate(char ** line) //function that will return date
{
    char * day;
    char * month;
    day = (char*)malloc(9 * sizeof(char));
    month = (char*)malloc(4 * sizeof(char));
    time_t tm;
    tm = time(NULL);
    
    time_t my_time;
    struct tm *timeinfo;
    time(&my_time);
    timeinfo = localtime(&my_time);
    
    /*
     switch(timeinfo->tm_mday) //assign the necessary day to the string
     {
     case 0:
     snprintf(day, 9, "%s", "Sunday");//day = "Sunday";
     break;
     case 1:
     snprintf(day, 9, "%s", "Monday");//day = "Monday";
     break;
     case 2:
     snprintf(day, 9, "%s", "Tuesday");//day = "Tuesday";
     break;
     case 3:
     snprintf(day, 9, "%s", "Wednesday");//day = "Wednesday";
     break;
     case 4:
     snprintf(day, 9, "%s", "Thursday");//day = "Thursday";
     break;
     case 5:
     snprintf(day, 9, "%s", "Friday");//day = "Friday";
     break;
     case 6:
     snprintf(day, 9, "%s", "Saturday");//day = "Saturday";
     break;
     }*/
    
    switch(timeinfo->tm_mon) //assign the necessary month to the string
    {
        case 0:
            snprintf(month,4, "%s", "Jan");
            break;
        case 1:
            snprintf(month,4, "%s", "Feb");
            break;
        case 2:
            snprintf(month,4, "%s", "Mar");
            break;
        case 3:
            snprintf(month,4, "%s", "Apr");
            break;
        case 4:
            snprintf(month,4, "%s", "May");
            break;
        case 5:
            snprintf(month,4, "%s", "June");
            break;
        case 6:
            snprintf(month,4, "%s", "July");
            break;
        case 7:
            snprintf(month,4, "%s", "Aug");
            break;
        case 8:
            snprintf(month,4, "%s", "Sept");
            break;
        case 9:
            snprintf(month,4, "%s", "Oct");
            break;
        case 10:
            snprintf(month,4, "%s", "Nov");
            break;
        case 11:
            snprintf(month,4, "%s", "Dec");
            break;
    }
    snprintf(*line, 20, "%s %d %d", month, timeinfo->tm_mday, timeinfo->tm_year+1900);
    return 1;
}

int getLogLevel (char ** line, int level){
    switch(level)
    {
        case 0:
            snprintf(*line, 20, "TRACE");
            break;
        case 1:
            snprintf(*line, 20, "DEBUG");
            break;
        case 2:
            snprintf(*line, 20, "INFO");
            break;
        case 3:
            snprintf(*line, 20, "WARNING");
            break;
        case 4:
            snprintf(*line, 20, "ERROR");
            break;
        case 5:
            snprintf(*line, 20, "FATAL");
            break;
    }
    
    return 1;
}


int main()
{
    enum LOGGING logLevel = TRACE;
    char *date;
    char *time;
    char *loggingLevel;
    
    date = (char*)malloc(1 * sizeof(char));
    time = (char*)malloc(1 * sizeof(char));
    loggingLevel = (char*)malloc( 1 * sizeof(char));

    FILE *fptr;
    fptr = fopen("log.txt","a");
    
    getDate(&date);
    getLoggingTime(&time);
    getLogLevel(&loggingLevel, logLevel);
    fprintf (fptr, "%s - %s: %s\n", date, time, loggingLevel);
    fclose(fptr);

    return 0;
    
    
}
