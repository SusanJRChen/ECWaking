#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#include <ugpio/ugpio.h>

struct Button {
	int pin;
	int request;
	int receive;
	
	int prev_vals[5];
	int cur_val;

	bool pressed;	
};

bool isPressed(int prev_vals[], int cur_val){
	if(!cur_val)
		return false;
	else {
		for (int i = 0; i < 5; i++){
			if(prev_vals[i])
				return false;	
		}		
	}
	return true;
}

bool changeValues (int& prev_vals[], cur_val){
    for (int i = 0; i < 4; i++){
        prevals [i+1] = prevals [i];
    }
    pre_vals[0] = cur_val;
    return true;
}

int main (int argc, char ** argv, char ** envp)
{	
	struct Button button1 = {0};
	struct Button button2 = {0};
	struct Button button3 = {0};
	struct Button button4 = {0};
    	struct Button button5 = {0};
    
	button1.pin = 1;
	button2.pin = 2;
    	button3.pin = 3;
    	button4.pin = 4;
    	button5.pin = 5;
	
	button1.request = gpio_is_requested(button1.pin);
	button2.request = gpio_is_requested(button2.pin);
	button3.request = gpio_is_requested(button3.pin);
	button4.request = gpio_is_requested(button4.pin);
	button5.request = gpio_is_requested(button5.pin);
	
	if (button1.request < 0 || button2.request < 0 || button3.request < 0 || button4.request < 0 || button5.request < 0)
	{
		perror("gpio_is_requested");
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
			perror("gpio_request");
			return EXIT_FAILURE;
		}
	}
	
	button1.receive = gpio_direction_input(button1.pin);
	button2.receive = gpio_direction_input(button2.pin);
	button3.receive = gpio_direction_input(button3.pin);
	button4.receive = gpio_direction_input(button4.pin);
	button5.receive = gpio_direction_input(button5.pin);
	
	bool run = true;
	
	while(run){
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
        
        
		if (button1.pressed){
			printf("button 1 is pressed\n");
            
		}
		if (button2.pressed){
			printf("button 2 is pressed\n");
		}
		if (button3.pressed){
			printf("button 3 is pressed\n");
		}
		if (button4.pressed){
			printf("button 4 is pressed\n");
		}
		if (button5.pressed){
			printf("button 5 is pressed\n");
		}
			    
    		changeValues (button1.pre_vals, button1.cur_val);
    		changeValues (button2.pre_vals, button2.cur_val);
    		changeValues (button3.pre_vals, button3.cur_val);
    		changeValues (button4.pre_vals, button4.cur_val);
    		changeValues (button5.pre_vals, button5.cur_val);
		
	}
}
