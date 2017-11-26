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
	
	int prev_vals[8];
	int cur_val;

	bool pressed;	
};

bool isPressed(int prev_vals[], int cur_val){
	if(!cur_val)
		return false;
	else {
		//for (int i = 0; i < 3; i++){
		//	if (!prev_vals[i])
		//		return false;
		//}
		for (int i = 0; i < 8; i++){
			if(prev_vals[i])
				return false;	
		}		
	}
	return true;
}

bool initialize(int prev_vals[]){
	for (int i = 0; i < 8; i++){
		prev_vals[i] = 0;
	}
	return true;
}

bool changeValues (int prev_vals[], int cur_val){
    for (int i = 7; i > 0; i--){
        prev_vals [i] = prev_vals [i - 1];
    }
    prev_vals[0] = cur_val;
    return true;
}

int main (int argc, char ** argv, char ** envp)
{	
	bool a = true;
	
	struct Button button1 = {1, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, false};
	struct Button button2 = {2, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, false};
	struct Button button3 = {3, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, false};
	struct Button button4 = {6, 0, 0, {0, 0, 0, 0, 0, 0 ,0, 0}, 0, false};
    	struct Button button5 = {18, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, false};
	
	button1.request = gpio_is_requested(button1.pin);
	button2.request = gpio_is_requested(button2.pin);
	button3.request = gpio_is_requested(button3.pin);
	button4.request = gpio_is_requested(button4.pin);
	button5.request = gpio_is_requested(button5.pin);
	
	a = initialize (button1.prev_vals);
	a = initialize (button2.prev_vals);
	a = initialize (button3.prev_vals);
	a = initialize (button4.prev_vals);
	a = initialize (button5.prev_vals);
	
	button1.cur_val = 0;
	button2.cur_val = 0;
	button3.cur_val = 0;
	button4.cur_val = 0;
	button5.cur_val = 0;

	
	
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
		
		//printf ("%i %i %i %i %i\n, button1.cur_val, button2.cur_val, button3.cur_val, button4.cur_val, button5.cur_val);
		//usleep (10000);
        
        
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
					    
    		a = changeValues (button1.prev_vals, button1.cur_val);
    		a = changeValues (button2.prev_vals, button2.cur_val);
    		a = changeValues (button3.prev_vals, button3.cur_val);
    		a = changeValues (button4.prev_vals, button4.cur_val);
    		a = changeValues (button5.prev_vals, button5.cur_val);
		
		button1.pressed = false;
		button2.pressed = false;
		button3.pressed = false;
		button4.pressed = false;
		button5.pressed = false;
		
	}
}
