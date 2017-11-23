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
        for (int i = 0; i < 5; i ++){
            if(prev_vals[i])
                return false;
        }
    }
    
    return true;
};

int main (int argc, char ** argv, char ** envp)
{
    struct Button button1 = {0};
    struct Button button2 = {0};
    struct Button button3 = {0};
    struct Button button4 = {0};
    struct Button button5 = {0};
    
    struct Button buttons[5] = {0};
    
    for (int i = 0; i < 5; i++){
        buttons[i].pin = i + 1;
        buttons[i].request = gpio_is_requested(buttons[i].pin);
        if (buttons[i].request < 0){
            perror("gpio_is_requested");
            return EXIT_FAILURE;
        }
        if (!buttons[i].request){
            if (buttons[i].request < 0){
                perror("gpio_request");
                return EXIT_FAILURE;
            }
        }
        buttons[i].receive = gpio_direction_input(buttons[i].pin, NULL);
    }
    
    bool run = true;
    
    while(run){
        
        for (int i = 0; i < 5; i++){
            buttons[i].cur_val = gpio_get_value(buttons[i].pin);
            buttons[i].pressed = isPressed(buttons[i].prev_vals, buttons[i].cur_val);
        }
         
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
     
        
    }
    
}

