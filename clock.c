#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ugpio/ugpio.h>
#include <time.h>
#include <math.h>

struct Button
{
    int pin;
    int request;
    int receive;

    int prev_vals[5];
    int cur_val;

    bool pressed;
};

bool isPressed(int prev_vals[], int cur_val)
{
    if (cur_val == 0)
    {
        return false;
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            if (prev_vals[i])
                return false;
        }
    }	

    return true;
}

bool initialize(int prev_vals[])
{
    for (int i = 0; i < 5; i++)
    {
        prev_vals[i] = 0;
    }

    return true;
}

bool changeValues(int prev_vals[], int cur_val)
{
    for (int i = 5; i > 0; i--)
    {
        prev_vals[i] = prev_vals[i - 1];
    }
    prev_vals[0] = cur_val;

    return true;
}
int getLoggingTime(char **line) //function that will return time
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

int getLogLevel(char **line, int level)
{
    switch (level)
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

bool isNum(char in)
{
    if (in >= '0' && in <= '9')
    {
        return true;
    }

    return false;
}

bool checkChar(char in)
{
    if (in >= 48 && in <= 57)
        return true;
    else if (in == 'e' || in == 'E' || in == '.' || in == '+' || in == '-')
        return true;
    else
        return false;
}

float FahtoCel(float value)
{
    return (value - 32) / 1.8;
}

bool stringToFloat(const char input[], float *value)
{
    enum State
    {
        Start,
        Normal,
        Terminal
    };
    enum InputState
    {
        Whole,
        Decimal,
        ExponentSign,
        Exponent
    };

    enum State state = Start;
    enum InputState inState;
    char in = input[0];
    int inIndex = 0;
    bool Done = false;

    int sign = 1;
    int exponent = 0;
    int decimal = 0;
    int whole = 0;
    int exponentSign = 1;

    int decDigits = 0;

    while (!Done)
    {
        in = input[inIndex];
        inIndex++;
        switch (state)
        {
        case Start:
            if (in == '+' || in == '-')
            {
                if (!isNum(input[inIndex]))
                    return false;
                if (in == '-')
                    sign = -1;
                inState = Whole;
                state = Normal;
            }
            else if (in == '.')
            {
                if (!isNum(input[inIndex]))
                    return false;
                inState = Decimal;
                state = Normal;
            }
            else if (isNum(in))
            {
                state = Normal;
                inState = Whole;
                whole *= 10;
                whole += in - 48;
            }
            else
                return false;
            break;
        case Normal:
            switch (inState)
            {
            case Whole:
                if (isNum(in))
                {
                    whole *= 10;
                    whole += in - 48;
                }
                else if (in == '.')
                {
                    if (!isNum(input[inIndex]))
                        return false;
                    inState = Decimal;
                }
                else if (in == 'e' || in == 'E')
                {
                    if (!isNum(input[inIndex]) && !(input[inIndex] == '+' || input[inIndex] == '-'))
                        return false;
                    inState = ExponentSign;
                }
                else if (in == 0)
                    state = Terminal;
                else if (!checkChar(in) || in == '+' || in == '-')
                    return false;
                break;
            case Decimal:
                if (isNum(in))
                {
                    decimal *= 10;
                    decimal += in - 48;
                    decDigits++;
                }
                else if (in == 'e' || in == 'E')
                {
                    if (!isNum(input[inIndex]) && !(input[inIndex] == '+' || input[inIndex] == '-'))
                        return false;
                    inState = ExponentSign;
                }
                else if (in == 0)
                    state = Terminal;
                else if (!checkChar(in) || in == '.' || in == '+' || in == '-')
                    return false;
                break;
            case ExponentSign:
                if (isNum(in))
                {
                    inState = Exponent;
                    exponent *= 10;
                    exponent += in - 48;
                }
                else if (in == '+' || in == '-')
                {
                    if (!isNum(input[inIndex]))
                        return false;
                    if (in == '-')
                        exponentSign = -1;
                    inState = Exponent;
                }
                else if (!checkChar(in) || in == '.')
                    return false;
                break;
            case Exponent:
                if (isNum(in))
                {
                    exponent *= 10;
                    exponent += in - 48;
                }
                else if (in == 0)
                    state = Terminal;
                else if (!checkChar(in) || in == 'e' || in == 'E' || in == '+' || in == '-')
                    return false;
                break;
            }
            break;
        case Terminal:
            Done = true;
            exponent *= exponentSign;
            *value = (sign * (whole + decimal * pow(10, -decDigits)) * pow(10, exponent));
            return true;
            break;
        }
    }
}

char *substring(char *string, int position, int length)
{
    char *pointer;
    int c;

    pointer = malloc(length + 1);

    if (pointer == NULL)
    {
        printf("Unable to allocate memory.\n");
        exit(1);
    }

    for (c = 0; c < length; c++)
    {
        *(pointer + c) = *(string + position - 1);
        string++;
    }

    *(pointer + c) = '\0';

    return pointer;
}

bool getIP(char **ipaddress)
{
    FILE *fp = popen("curl ipinfo.io/ip", "r");

    fscanf(fp, "%s", *ipaddress);
    pclose(fp);

    return true;
}

bool getLongandLat(char *buff, char **latitude, char **longitude)
{
    char *result = strstr(buff, "latitude\":");

    int LatitudeIndex = result - buff + 11;
    result = strstr(buff, "longitude\":");
    int LongitudeIndex = result - buff + 12;
    int length = LongitudeIndex - LatitudeIndex - 13;

    *longitude = substring(buff, LongitudeIndex, length);
    *latitude = substring(buff, LatitudeIndex, length);

    return true;
}

bool getWoeid(char *buff, char **woeid)
{
    int woeidIndex = strstr(buff, "woeid\":") - buff + 9;
    int woeidEndIndex = strstr(buff, "\"}}") - buff + 1;

    *woeid = substring(buff, woeidIndex, woeidEndIndex - woeidIndex);

    return true;
}

bool getTemperature(char *buff, char **high, char **low)
{
    int highIndex = strstr(buff, "high\":\"") - buff + 8;
    int lowIndex = strstr(buff, "low\":\"") - buff + 7;
    int textIndex = strstr(buff, "text\":\"") - buff + 8;

    *high = substring(buff, highIndex, lowIndex - highIndex - 9);
    *low = substring(buff, lowIndex, textIndex - lowIndex - 10);

    return true;
}

bool getUnits(char *buff, char **temperatureUnits)
{
    int unitIndex = strstr(buff, "temperature\":") - buff + 15;

    *temperatureUnits = substring(buff, unitIndex, 1);
    if (*temperatureUnits != "C" && *temperatureUnits != "F")
    {
        return false;
    }

    return true;
}

bool getLineWithString(char *filename, char *string, char *buff)
{
    FILE *file;
    file = fopen(filename, "r");

    if (file == NULL)
        return false;

    while (fscanf(file, "%s", buff) != EOF)
    {
        if (strstr(buff, string))
        {
            return true;
        }
    }

    return false;
}
int getTime(char **line) //function that will return time
{
    time_t tm;
    tm = time(NULL);

    time_t my_time;
    struct tm *timeinfo;
    time(&my_time);
    timeinfo = localtime(&my_time);

    snprintf(*line, 20, "       %02d:%02d       ", timeinfo->tm_hour, timeinfo->tm_min);
    printf("%s", *line);
    return 1;
}
int getDate(char **line) //function that will return date
{
    char *day;
    char *month;
    day = (char *)malloc(5 * sizeof(char));
    month = (char *)malloc(4 * sizeof(char));
    time_t tm;
    tm = time(NULL);

    time_t my_time;
    struct tm *timeinfo;
    time(&my_time);
    timeinfo = localtime(&my_time);

    switch (timeinfo->tm_wday) //assign the necessary day to the string
    {
    case 0:
        snprintf(day, 5, "%s", "Sun"); //day = "Sunday";
        break;
    case 1:
        snprintf(day, 5, "%s", "Mon"); //day = "Monday";
        break;
    case 2:
        snprintf(day, 5, "%s", "Tues"); //day = "Tuesday";
        break;
    case 3:
        snprintf(day, 5, "%s", "Wed"); //day = "Wednesday";
        break;
    case 4:
        snprintf(day, 5, "%s", "Thurs"); //day = "Thursday";
        break;
    case 5:
        snprintf(day, 5, "%s", "Fri"); //day = "Friday";
        break;
    case 6:
        snprintf(day, 5, "%s", "Sat"); //day = "Saturday";
        break;
    }

    switch (timeinfo->tm_mon) //assign the necessary month to the string
    {
    case 0:
        snprintf(month, 4, "%s", "Jan");
        break;
    case 1:
        snprintf(month, 4, "%s", "Feb");
        break;
    case 2:
        snprintf(month, 4, "%s", "Mar");
        break;
    case 3:
        snprintf(month, 4, "%s", "Apr");
        break;
    case 4:
        snprintf(month, 4, "%s", "May");
        break;
    case 5:
        snprintf(month, 4, "%s", "June");
        break;
    case 6:
        snprintf(month, 4, "%s", "July");
        break;
    case 7:
        snprintf(month, 4, "%s", "Aug");
        break;
    case 8:
        snprintf(month, 4, "%s", "Sept");
        break;
    case 9:
        snprintf(month, 4, "%s", "Oct");
        break;
    case 10:
        snprintf(month, 4, "%s", "Nov");
        break;
    case 11:
        snprintf(month, 4, "%s", "Dec");
        break;
    }
    snprintf(*line, 20, "%s %s %d %d    ", day, month, timeinfo->tm_mday, timeinfo->tm_year + 1900);

    return 1;
}
bool getTemperatureInC(float *hightemp, float *lowtemp)
{
    char *ipaddress = (char *)malloc(20);
    char command[256];
    char buff[256];

    char *woeid = (char *)malloc(15);
    char *longitude = (char *)malloc(8);
    char *latitude = (char *)malloc(8);
    char *high = (char *)malloc(5);
    char *low = (char *)malloc(5);
    char *temperatureUnits = (char *)malloc(1);

    system("curl ipinfo.io/ip > content.txt");

    if (getLineWithString("content.txt", ".", buff))
    {
        strcpy(ipaddress, buff);
    }

    snprintf(command, sizeof command, "%s%s%s", "curl freegeoip.net/json/", ipaddress, " > content.txt");
    system(command);

    if (getLineWithString("content.txt", "longitude", buff))
        getLongandLat(buff, &longitude, &latitude);

    snprintf(command, sizeof command, "%s%s%s%s%s", "curl query.yahooapis.com/v1/public/yql -d q=\"select woeid from geo.places where text=\\\"(", longitude, ", ", latitude, ")\\\"\" -d format=json > content.txt");
    system(command);

    if (getLineWithString("content.txt", "woeid", buff))
        getWoeid(buff, &woeid);

    snprintf(command, sizeof command, "%s%s%s", "curl query.yahooapis.com/v1/public/yql -d q=\"select * from weather.forecast where woeid=\\\"", woeid, "\\\"\" -d format=json > content.txt");
    system(command);

    if (getLineWithString("content.txt", "high", buff))
        getTemperature(buff, &high, &low);

    if (getLineWithString("content.txt", "temperature", buff))
        getUnits(buff, &temperatureUnits);

    float highfloat = 0;
    float lowfloat = 0;

    stringToFloat(high, &highfloat);
    stringToFloat(low, &lowfloat);

    if (temperatureUnits == "F" || temperatureUnits[0] == 'F')
    {
        highfloat = FahtoCel(highfloat);
        lowfloat = FahtoCel(lowfloat);
    }

    float averageTemperature = (highfloat + lowfloat) / 2;

    // logging
    char *date;
    char *loggingTime;
    date = (char *)malloc(1 * sizeof(char));
    loggingTime = (char *)malloc(1 * sizeof(char));

    FILE *fptr;
    fptr = fopen("log.txt", "a");
    bool logging = true;
    if (fptr == NULL){
	logging = false;
    }
    if (logging){
    	getDate(&date);
    	getLoggingTime(&loggingTime);
    	fprintf(fptr, "%s - %s: TRACE - Public IP: %s\n", date, loggingTime, ipaddress);
    	fprintf(fptr, "%s - %s: TRACE - Coordinates: (%s, %s)\n", date, loggingTime, latitude, longitude);
    	fprintf(fptr, "%s - %s: TRACE - woeid: %s\n", date, loggingTime, woeid);
    	fprintf(fptr, "%s - %s: TRACE - Units were given in: %s\n", date, loggingTime, temperatureUnits);
    	fprintf(fptr, "%s - %s: TRACE - (in celsius) High: %f , Low: %f\n", date, loggingTime, highfloat, lowfloat);
    	fprintf(fptr, "%s - %s: TRACE - Average Temperature(C): %f\n", date, loggingTime, averageTemperature);
    }
    *hightemp = highfloat;
    *lowtemp = lowfloat;
    return true;
}

int getWeather(char **line)
{
    float lowtemp;
    float hightemp;
    if (!getTemperatureInC(&hightemp, &lowtemp))
        return -1;

    snprintf(*line, 20, "%s%.1f%s%.1f", "High:", hightemp, " Low:", lowtemp);
    return 0;
}
int getAlarm(bool alarm, int hours, int minutes, char **line) //function that will return the alarm
{
    if (!alarm) //if there is no alarm
    {
        snprintf(*line, 20, "%s", " Have a great day!");
    }
    else
    {
        snprintf(*line, 20, "   Alarm at %02d:%02d   ", hours, minutes);
    }

    return 1;
}

bool update(char *line1, char *line2, char *line3)
{
    char command[200]; //command meant to sent to bash to control display
    //combine all the lines to output and send it out
    snprintf(command, 200, "python /FireOnion_I2C_LCD/src/lcd.py -a 0x27 --line1=\"%s\" --line2=\"%s\" --line3=\"%s\" \n", line1, line2, line3);
    /*char * argv[8];
    argv[0] = "-a";
    argv[1] = "0x27";
    argv[2] =  "--line1=";
    argv[3] = line1;
    argv[4] = "--line2=";
    argv[5] = line2;
    argv[6] = "--line3=";
    argv[7] = line3;
    Py_SetProgramName("lcd.py");
    Py_Initialize();
    PySys_SetArgv(8, argv);
    FILE lcdFile = fopen("/FireOnion_I2C_LCD/src/lcd.py", "r");
    PyRun_SimpleFile(file, "lcd.py");
    Py_Finalize();
    fclose(lcdFile);*/


    // logging
    char *date;
    char *loggingTime;
    date = (char *)malloc(1 * sizeof(char));
    loggingTime = (char *)malloc(1 * sizeof(char));

    FILE *fptr;
    fptr = fopen("log.txt", "a");
    bool logging = true;
    if (fptr == NULL){
	logging = false;
    }
	
    if (logging){
    getDate(&date);
    getLoggingTime(&loggingTime);
    	fprintf(fptr, "%s - %s: TRACE - Screen Outputted Successfully\n", date, loggingTime);
    }
	
    system(command);
    return true;
}

enum clockState
{
    Normal,
    ChangeAlarm,
    AlarmON
};
enum LOGGING
{
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

int main(int argc, char **argv, char **envp)
{
    system("fast-gpio set 11 0");
    system("omega2-ctrl gpiomux set spi_cs1 gpio");
    system("ntpd -q -p 	ptbtime1.ptb.de");
    system("reboot");	
    bool alarm = false; //determines whether an alarm is active or not
    bool modMinutes = false;
    bool upPressed = false;
    bool downPressed = false;
    bool snooze = false;
    int aMinutes = 05;
    int aHours = 23;
    int tMinutes = -1;
    char *line1;
    char *line2; //line 2 will control lines 2 and 4 hence 40 lines
    char *line3;
    char *line4;
    char *line5;

    time_t tm;
    time_t my_time;
    struct tm *timeinfo;

    line1 = (char *)malloc(20 * sizeof(char));
    line2 = (char *)malloc(20 * sizeof(char));
    line3 = (char *)malloc(20 * sizeof(char));
    line4 = (char *)malloc(20 * sizeof(char));
    line5 = (char *)malloc(40 * sizeof(char));

    bool a = true;

    enum LOGGING logLevel = TRACE;
    char *date;
    char *loggingTime;
    char *loggingLevel;

    date = (char *)malloc(1 * sizeof(char));
    loggingTime = (char *)malloc(1 * sizeof(char));
    loggingLevel = (char *)malloc(1 * sizeof(char));

    FILE *fptr;
    fptr = fopen("log1.txt", "a");

    bool logging = true;
    if (fptr == NULL){
	logging = false;
    }
    
    /*
    to make a log
    logLevel = (put LOGGING level here);
    getDate(&date);
    getLoggingTime(&loggingTime);
    getLogLevel(&loggingLevel, logLevel);
    fprintf (fptr, "%s - %s: %s - log message here\n", date, loggingTime, loggingLevel);
    
    */

    // logging
    if (logging){
    	logLevel = TRACE;
    	getDate(&date);
    	getLoggingTime(&loggingTime);
    	getLogLevel(&loggingLevel, logLevel);
    	fprintf(fptr, "%s - %s: %s - Program Initialized\n", date, loggingTime, loggingLevel);
    }
	
    struct Button button1 = {1, 0, 0, {0, 0, 0, 0, 0}, 0, false};
    struct Button button2 = {2, 0, 0, {0, 0, 0, 0, 0}, 0, false};
    struct Button button3 = {3, 0, 0, {0, 0, 0, 0, 0}, 0, false};
    struct Button button4 = {6, 0, 0, {0, 0, 0, 0, 0}, 0, false};
    struct Button button5 = {18, 0, 0, {0, 0, 0, 0, 0}, 0, false};

    button1.request = gpio_is_requested(button1.pin);
    button2.request = gpio_is_requested(button2.pin);
    button3.request = gpio_is_requested(button3.pin);
    button4.request = gpio_is_requested(button4.pin);
    button5.request = gpio_is_requested(button5.pin);

    a = initialize(button1.prev_vals);
    a = initialize(button2.prev_vals);
    a = initialize(button3.prev_vals);
    a = initialize(button4.prev_vals);
    a = initialize(button5.prev_vals);

    button1.cur_val = 0;
    button2.cur_val = 0;
    button3.cur_val = 0;
    button4.cur_val = 0;
    button5.cur_val = 0;

    // logging
    if (logging){
    	logLevel = TRACE;
    	getDate(&date);
    	getLoggingTime(&loggingTime);
    	getLogLevel(&loggingLevel, logLevel);
    	fprintf(fptr, "%s - %s: %s - Buttons Initialized\n", date, loggingTime, loggingLevel);
    }
	
    if (button1.request < 0 || button2.request < 0 || button3.request < 0 || button4.request < 0 || button5.request < 0)
    {
        perror("gpio_is_requested");

	// logging
	if (Logging){
        	logLevel = FATAL;
        	getDate(&date);
        	getLoggingTime(&loggingTime);
        	getLogLevel(&loggingLevel, logLevel);
        	fprintf(fptr, "%s - %s: %s - gpio Requested Error\n", date, loggingTime, loggingLevel);
	}
        return EXIT_FAILURE;
    }

    button1.receive = gpio_request(button1.pin, NULL);
    button2.receive = gpio_request(button2.pin, NULL);
    button3.receive = gpio_request(button3.pin, NULL);
    button4.receive = gpio_request(button4.pin, NULL);
    button5.receive = gpio_request(button5.pin, NULL);

    if (!(button1.request || button2.request || button3.request || button4.request || button5.request))
    {
        if (button1.receive < 0 || button2.receive < 0 || button3.receive < 0 || button4.receive < 0 || button5.receive < 0)
        {
            // logging
	    if (logging){
            	logLevel = FATAL;
            	getDate(&date);
            	getLoggingTime(&loggingTime);
            	getLogLevel(&loggingLevel, logLevel);
            	fprintf(fptr, "%s - %s: %s - gpio Requested Error\n", date, loggingTime, loggingLevel);
	    }

            perror("gpio_request");
            return EXIT_FAILURE;
        }
    }

    button1.receive = gpio_direction_input(button1.pin);
    button2.receive = gpio_direction_input(button2.pin);
    button3.receive = gpio_direction_input(button3.pin);
    button4.receive = gpio_direction_input(button4.pin);
    button5.receive = gpio_direction_input(button5.pin);


    enum clockState state = Normal;
    fclose(fptr);
    while (1)
    {
	fptr = fopen("log.txt", "a");
        //insert logic for buttons here, based on the logic change the state of the machine
        //check if the time has been reached
        //check if the button has been pressed and change the bool of alarm

        button1.cur_val = gpio_get_value(button1.pin);
        button2.cur_val = gpio_get_value(button2.pin);
        button3.cur_val = gpio_get_value(button3.pin);
        button4.cur_val = gpio_get_value(button4.pin);
        button5.cur_val = gpio_get_value(button5.pin);

        button1.pressed = isPressed(button1.prev_vals, button1.cur_val);
        button2.pressed = isPressed(button2.prev_vals, button2.cur_val);
        button3.pressed = isPressed(button3.prev_vals, button3.cur_val);
        button4.pressed = isPressed(button4.prev_vals, button4.cur_val);
        button5.pressed = isPressed(button5.prev_vals, button5.cur_val);

        if (button1.pressed && state != ChangeAlarm)
        {
            // logging
	    if (logging){
            	logLevel = DEBUG;
            	getDate(&date);
            	getLoggingTime(&loggingTime);
            	getLogLevel(&loggingLevel, logLevel);
            	fprintf(fptr, "%s - %s: %s - Button 1 Pressed\n", date, loggingTime, loggingLevel);

            	logLevel = TRACE;
            	getDate(&date);
            	getLoggingTime(&loggingTime);
            	getLogLevel(&loggingLevel, logLevel);
            	fprintf(fptr, "%s - %s: %s - Change State to ChangeAlarm\n", date, loggingTime, loggingLevel);
	    }
		
            state = ChangeAlarm;
	    getTime(&line1); 
            getDate(&line2);
	    getWeather(&line3);
	    getAlarm(alarm, aHours, aMinutes, &line4);
            snprintf(line5, 40, "%s%s", line2, line4);
            snprintf(line1, 20, "Change %s %02d:%02d", "Hour", aHours, aMinutes);

            update(line1, line5, line3);
        }
        if (button4.pressed)
        {
            // logging
	    if (logging){
            	logLevel = DEBUG;
            	getDate(&date);
            	getLoggingTime(&loggingTime);
            	getLogLevel(&loggingLevel, logLevel);
            	fprintf(fptr, "%s - %s: %s - Button 4 Pressed\n", date, loggingTime, loggingLevel);
	    }

            alarm = !alarm;
            button4.pressed = false;
	    getTime(&line1); 
            getDate(&line2);
	    getWeather(&line3);
	    getAlarm(alarm, aHours, aMinutes, &line4);
            snprintf(line5, 40, "%s%s", line2, line4);

            update(line1, line5, line3);
        }

        a = changeValues(button1.prev_vals, button1.cur_val);
        a = changeValues(button2.prev_vals, button2.cur_val);
        a = changeValues(button3.prev_vals, button3.cur_val);
        a = changeValues(button4.prev_vals, button4.cur_val);
        a = changeValues(button5.prev_vals, button5.cur_val);

        button1.pressed = false;
        button2.pressed = false;
        button3.pressed = false;
        button4.pressed = false;
        button5.pressed = false;

        tm = time(NULL);
        

        time(&my_time);
        timeinfo = localtime(&my_time);

        if (aMinutes == timeinfo->tm_min && aHours == timeinfo->tm_hour && alarm)
        {
            // logging
	    if (Logging){
            	logLevel = TRACE;
            	getDate(&date);
            	getLoggingTime(&loggingTime);
            	getLogLevel(&loggingLevel, logLevel);
            	fprintf(fptr, "%s - %s: %s - Change State to AlarmON\n", date, loggingTime, loggingLevel);
	    }
		
            state = AlarmON;
        }
        switch (state)
        {
        case Normal:
	    if(tMinutes < 0)
		getWeather(&line3);	
            if (tMinutes != timeinfo->tm_min)
            {
		printf("Current Time: %s \n", ctime(&tm));
		    
                // logging
		if (logging){
	                logLevel = DEBUG;
        	        getDate(&date);
                	getLoggingTime(&loggingTime);
                	getLogLevel(&loggingLevel, logLevel);
                	fprintf(fptr, "%s - %s: %s - Case Normal\n", date, loggingTime, loggingLevel);
		}

                tMinutes = timeinfo->tm_min;
                getTime(&line1); 
                getDate(&line2);
                getAlarm(alarm, aHours, aMinutes, &line4);
                snprintf(line5, 40, "%s%s", line2, line4);
                if (timeinfo->tm_min == 30)
                    getWeather(&line3);

                update(line1, line5, line3);
            }
            break;
        case ChangeAlarm:


		while(!upPressed && !downPressed)
		{	
	            button1.cur_val = gpio_get_value(button1.pin);
        	    button2.cur_val = gpio_get_value(button2.pin);
                    button3.cur_val = gpio_get_value(button3.pin);		
		    
		    if (button1.cur_val)
		    {
			getWeather(&line3);
			printf("Button 1 Pressed");
			    
		        // logging
			if (logging){
		        	logLevel = DEBUG;
		        	getDate(&date);
		       		getLoggingTime(&loggingTime);
		        	getLogLevel(&loggingLevel, logLevel);
		        	fprintf(fptr, "%s - %s: %s - Button 1 Pressed\n", date, loggingTime, loggingLevel);
			}
			    
		        if (modMinutes)
		        {
		            state = Normal;
		            modMinutes = false;
                    	    
		            getTime(&line1); 
		            getDate(&line2);
		            getAlarm(alarm, aHours, aMinutes, &line4);
		            snprintf(line5, 40, "%s%s", line2, line4);
		            update(line1, line5, line3);	
		        }
		        else
		        {
		            modMinutes = true;
                    	    snprintf(line1, 20,"Change %s %02d:%02d", "Min", aHours, aMinutes);
                            update(line1, line5, line3);
		        }
		    }
		    else if (button2.cur_val)
		    {
			printf("Button 2 pressed");
			    
		        // logging
			if (logging){
		        	logLevel = DEBUG;
		        	getDate(&date);
		        	getLoggingTime(&loggingTime);
		        	getLogLevel(&loggingLevel, logLevel);
		       		fprintf(fptr, "%s - %s: %s - Button 2 Pressed\n", date, loggingTime, loggingLevel);
			}
			    
		        upPressed = true;
		    }
		    else if (button3.cur_val)
		    {
			printf("Button 3 pressed");
			    
		        // logging
			if (logging){
		        	logLevel = DEBUG;
			        getDate(&date);
			        getLoggingTime(&loggingTime);
			        getLogLevel(&loggingLevel, logLevel);
		        	fprintf(fptr, "%s - %s: %s - Button 3 Pressed\n", date, loggingTime, loggingLevel);
			}
			    
		        downPressed = true;
		    }
		   
		}
		button1.pressed = false;
		button2.pressed = false;
		button3.pressed = false;

            if (!modMinutes) //changing the hours
            {
                if (upPressed) //up button is pressed
                {
		    // logging
		    if (logging){
	                    logLevel = INFO;
        	            getDate(&date);
                	    getLoggingTime(&loggingTime);
                    	    getLogLevel(&loggingLevel, logLevel);
                    	    fprintf(fptr, "%s - %s: %s - Up Button Pressed\n", date, loggingTime, loggingLevel);
		    }
			
                    aHours += 1;
                    if (aHours > 23)
                        aHours = 0;

                    upPressed = false;

                    snprintf(line1, 20,"Change %s %02d:%02d", "Hour", aHours, aMinutes);
                    update(line1, line5, line3);
                    break;
                }
                if (downPressed) //down button is pressed
                {
		    // logging
		    if (logging){
	                    logLevel = INFO;
        	            getDate(&date);
                	    getLoggingTime(&loggingTime);
                    	    getLogLevel(&loggingLevel, logLevel);
                    	    fprintf(fptr, "%s - %s: %s - Down Button Pressed\n", date, loggingTime, loggingLevel);
		    }

                    aHours -= 1;
                    if (aHours < 0)
                        aHours = 23;

                    downPressed = false;

                    snprintf(line1, 20, "Change %s %02d:%02d", "Hour", aHours, aMinutes);
                    update(line1, line5, line3);
                    break;
                }
            }
            else
            {
                if (upPressed) //up button is pressed
                {

                    // logging
		    if (logging){
                    	logLevel = INFO;
                    	getDate(&date);
                    	getLoggingTime(&loggingTime);
                    	getLogLevel(&loggingLevel, logLevel);
                    	fprintf(fptr, "%s - %s: %s - Up Button Pressed\n", date, loggingTime, loggingLevel);
		    }
			
                    aMinutes += 1;
                    if (aMinutes > 59)
                        aMinutes = 0;

                    upPressed = false;
                    snprintf(line1, 20, "Change %s %02d:%02d", "Min", aHours, aMinutes);
		    update(line1, line5, line3);
                    break;
                }
                if (downPressed) //down button is pressed
                {
		    	
                    // logging
		    if (logging){
                    	logLevel = INFO;
                    	getDate(&date);
                    	getLoggingTime(&loggingTime);
                    	getLogLevel(&loggingLevel, logLevel);
                    	fprintf(fptr, "%s - %s: %s - Down Button Pressed\n", date, loggingTime, loggingLevel);
		    }

                    aMinutes -= 1;
                    if (aMinutes < 0)
                        aMinutes = 59;

                    downPressed = false;
                    snprintf(line1, 20, "Change %s %02d:%02d", "Min", aHours, aMinutes);
		    update(line1, line5, line3);
                    break;
                }
            }
            break;
        case AlarmON:
            system("fast-gpio set 11 1");

            // logging
	    if (Logging){
	        logLevel = TRACE;
                getDate(&date);
            	getLoggingTime(&loggingTime);
            	getLogLevel(&loggingLevel, logLevel);
            	fprintf(fptr, "%s - %s: %s - Case AlarmON\n", date, loggingTime, loggingLevel);
	    }
            int tempHours = aHours;
            int tempMin = aMinutes;
            getTime(&line1);

            //This will force it to wait until it receives a signal from the gpio
            while (!snooze && !alarm)
            {  	
        	button4.cur_val = gpio_get_value(button4.pin);
        	button5.cur_val = gpio_get_value(button5.pin);
        	button4.pressed = isPressed(button4.prev_vals, button4.cur_val);
        	button5.pressed = isPressed(button5.prev_vals, button5.cur_val);
                if (button4.pressed)
                {
                    // logging
		    if (logging){
	                    logLevel = DEBUG;
        	            getDate(&date);
                	    getLoggingTime(&loggingTime);
                  	    getLogLevel(&loggingLevel, logLevel);
                    	    fprintf(fptr, "%s - %s: %s - Button 4 Pressed\n", date, loggingTime, loggingLevel);
		    }

		    system("fast-gpio set 11 0");
                    alarm = !alarm;
                  
                }
                else if (button5.pressed)
                {
                    // logging
		    if (logging){
	                    logLevel = DEBUG;
        	            getDate(&date);
                	    getLoggingTime(&loggingTime);
                   	    getLogLevel(&loggingLevel, logLevel);
                    	    fprintf(fptr, "%s - %s: %s - Button 5 Pressed\n", date, loggingTime, loggingLevel);
		    }
			
		    system("fast-gpio set 11 0");
                    snooze = true;
                    
                }
		usleep(200);
		a = changeValues(button4.prev_vals, button4.cur_val);
       		a = changeValues(button5.prev_vals, button5.cur_val);
            }
	    button4.pressed = false;
	    button5.pressed = false;	
		
            if (snooze) //the snooze button is pressed
            {
                //stop the logic high

                // logging
		if (logging){
                	logLevel = TRACE;
                	getDate(&date);
                	getLoggingTime(&loggingTime);
                	getLogLevel(&loggingLevel, logLevel);
                	fprintf(fptr, "%s - %s: %s - Alarm Snoozed\n", date, loggingTime, loggingLevel);
		}
		    
                aMinutes = timeinfo->tm_min + 5;
                if (aMinutes > 59)
                {
                    aMinutes = aMinutes % 60;
                    aHours++;
                }
		
		tMinutes = timeinfo->tm_min;
                getTime(&line1); 
                getDate(&line2);
		getWeather(&line3);
                getAlarm(alarm, aHours, aMinutes, &line4);
                snprintf(line5, 40, "%s%s", line2, line4);
                update(line1, line5, line3);

                snooze = false;
                state = Normal;
                system("fast-gpio set 11 0");

		// logging
		if (logging){
	                logLevel = TRACE;
       	        	getDate(&date);
        	        getLoggingTime(&loggingTime);
              	  	getLogLevel(&loggingLevel, logLevel);
                	fprintf(fptr, "%s - %s: %s - Alarm Sound Off\n", date, loggingTime, loggingLevel);
		}
            }

            if (alarm) //alarm off is pressed
            {
                // logging
		if (logging){
	                logLevel = DEBUG;
        	        getDate(&date);
                	getLoggingTime(&loggingTime);
                	getLogLevel(&loggingLevel, logLevel);
                	fprintf(fptr, "%s - %s: %s - Alarm Truned\n", date, loggingTime, loggingLevel);
		}
		    
		tMinutes = timeinfo->tm_min;
                getTime(&line1); 
                getDate(&line2);
		getWeather(&line3);
                getAlarm(alarm, aHours, aMinutes, &line4);
                snprintf(line5, 40, "%s%s", line2, line4);
                update(line1, line5, line3);

                aHours = tempHours; //reset the alarm to its original value;
                aMinutes = tempMin;

                alarm = false; //This will actually turn off the alarm
                //stop the logic high

                state = Normal;
                system("fast-gpio set 11 0");
            }
            break;
        }
	fclose(fptr);
    }

    // logging
    if (logging){
	    logLevel = TRACE;
   	 getDate(&date);
    	getLoggingTime(&loggingTime);
    	getLogLevel(&loggingLevel, logLevel);
    	fprintf(fptr, "%s - %s: %s - Program Closed\n", date, loggingTime, loggingLevel);
    	fclose(fptr);
    }
	
    return 0;
}

