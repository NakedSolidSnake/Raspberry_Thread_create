#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <led.h>
#include <button.h>

#define _1MS    1000

void *LED(void *args);
void *Button(void *args);
void *Status(void *args);

static int state = 0;
static eState_t eState = eStateLow;

int main(int argc, char *argv[])
{
    pthread_t led;
    pthread_t button;
    pthread_t status;

    pthread_create(&led, NULL, LED, NULL);
    pthread_create(&button, NULL, Button, NULL);
    pthread_create(&status, NULL, Status, NULL);


    pthread_join(led, NULL);
    pthread_join(button, NULL);
    pthread_join(status, NULL);
    return 0;
}

void *LED(void *args)
{
    LED_t led = {
        .gpio.pin = 0,
        .gpio.eMode = eModeOutput
    };

    if(LED_init(&led))
        return (void *)EXIT_FAILURE;

    while(1)
    {
        if(state)
        {
            LED_set(&led, eState);
            if(eState == eStateLow)
                eState = eStateHigh;
            else
                eState = eStateLow;

            state = 0;
        }

        usleep(_1MS);
    }
}

void *Button(void *args)
{
    Button_t button = {
        .gpio.pin = 7,
        .gpio.eMode = eModeInput,
        .ePullMode = ePullModePullUp,
        .eIntEdge = eIntEdgeFalling,
    };

    if(Button_init(&button))
        return (void *)EXIT_FAILURE;

    while(1)
    {
        if (!Button_read(&button))
        {
            usleep(_1MS * 40);
            while (!Button_read(&button))
                ;

            state = 1;
        }
    }
}

void *Status(void *args)
{
    while(1)
    {
        printf("State = %s.\n", !eState ? "on" : "off");
        sleep(1);
    }
}