#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softTone.h>

void state_init();
void state_red_on();
void state_green_on();
void state_green_blink1();
void state_green_blink2();
void state_yellow_on();
void *timer(void *duration);
void start_timer(int duration);
void *button_click();
void button_listener();
void *buzzer(void *hz);
void start_buzzer(int hz);

int event_button = 0, event_timeout = 0;

void (*state)() = state_init;
void main(){
	wiringPiSetup();
	pinMode(7,OUTPUT);
	pinMode(0,OUTPUT);
	pinMode(2,OUTPUT);
	pinMode(14,INPUT);
	//system("gpio -g mode 11 out");
	pullUpDnControl(14,PUD_DOWN);
	softToneCreate(1);
	digitalWrite(7,LOW);
	digitalWrite(0,LOW);
	digitalWrite(2,LOW);
	while(1){
		state();
	}
}

void *timer(void *duration){
	int d = *((int *) duration);
	printf("sleeping %d ...\n", d);
	sleep(d);
	event_timeout = 1;
	free(duration);
}

void start_timer(int duration){
	pthread_t mytimer;
	int *dur = malloc(sizeof(int));
 	*dur = duration;
	pthread_create(&mytimer, NULL, timer, (void *)dur);
}

void *button_click(){
	printf("waiting for button click...\n");
	int size = 100;
	char buffer[size];
	int a=0;
	while(digitalRead(14)==LOW);
	//pullUpDnControl(14,)
	//while(fgets(buffer, size, stdin) == NULL);
	//while(1){
		//a = system("gpio read 11");
	//}
	printf("click\n");
	event_button = 1;
}

void *buzzer(void *hz){
	int ihz = *((int *)hz);
	softToneWrite(1,ihz);
	while(!event_timeout);
	softToneWrite(1,0);
}

void start_buzzer(int ihz){
	int *hz = malloc(sizeof(int));
	*hz = ihz;
	pthread_t mybuzzer;
	pthread_create(&mybuzzer, NULL, buzzer, (void *)hz);
}

void button_listener(){
	pthread_t listener;
	pthread_create(&listener, NULL, button_click, NULL);
	pthread_join(listener, NULL);
}

void state_init(){
	printf("all led blink once\n");
	digitalWrite(7,HIGH);
	digitalWrite(0,HIGH);
	digitalWrite(2,HIGH);
	digitalWrite(1,HIGH);
	softToneWrite(1, 250);
	delay(800);
	digitalWrite(7,LOW);
	digitalWrite(0,LOW);
	digitalWrite(2,LOW);
	digitalWrite(1,LOW);
	softToneWrite(1, 0);
	delay(200);
	state = state_red_on;
}

void state_red_on(){
	printf("red is on\n");
	digitalWrite(2,HIGH);
	event_button = 0;
	button_listener();
	while(!event_button);
	event_timeout = 0;
	start_timer(2);
	while(!event_timeout);
	printf("red is off\n");
	digitalWrite(2,LOW);
	state = state_green_on;
}

void state_green_on(){
	printf("green is on\n");
	digitalWrite(7,HIGH);
	event_timeout = 0;
	start_timer(5);
	while(!event_timeout);
	printf("green is off\n");
	digitalWrite(7,LOW);
	state = state_green_blink1;
}

void state_green_blink1(){
	printf("green starts blinking 1\n");
	event_timeout = 0;
	start_timer(5);
	while(!event_timeout){
		digitalWrite(7,HIGH);
		softToneWrite(1, 500);
		delay(500);
		digitalWrite(7,LOW);
		softToneWrite(1, 0);
		delay(500);
	}
	printf("green stops blinking 1\n");
	state = state_green_blink2;
}

void state_green_blink2(){
	printf("green starts blinking 2\n");
	event_timeout = 0;
	start_timer(5);
	//start_buzzer(250);
	while(!event_timeout){
		digitalWrite(7,HIGH);
		softToneWrite(1, 500);
		delay(250);
		digitalWrite(7,LOW);
		softToneWrite(1, 0);
		delay(250);
	}
	printf("green stops blinking 2\n");
	state = state_yellow_on;
}
void play();
void test(int x,int y);
void state_yellow_on(){
	printf("yellow is on\n");
	digitalWrite(0,HIGH);
	event_timeout = 0;
	start_timer(2);
	start_buzzer(250);
	while(!event_timeout);
	printf("yellow is off\n");
	digitalWrite(0,LOW);
	//state = state_red_on;
	state = play;
}

void test(int x, int y){
	softToneWrite(1,x);
	delay(y);
	softToneWrite(1,0);
	delay(100);
	//state = state_red_on;
}

void play(){
	test(261,400);
	test(261,400);
	test(392,400);
	test(392,400);
	test(440,400);
	test(440,400);
	test(392,800);

	test(349,400);
	test(349,400);
	test(329,400);
	test(329,400);
	test(293,400);
	test(293,400);
	test(261,800);

	test(392,400);
	test(392,400);
	test(349,400);
	test(349,400);
	test(329,400);
	test(329,400);
	test(293,800);

	test(392,400);
	test(392,400);
	test(349,400);
	test(349,400);
	test(329,400);
	test(329,400);
	test(293,800);
	state = play;
}
