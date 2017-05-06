#include <stdio.h>
#include <Arduino.h>
#include "tones.h"
#include "sleep.h"
#include "neopixel_blit.h"

#define BIT5 (1<<5)
#define BIT6 (1<<6)
#define FULL 1
#define EMPTY 0
#define LEFT_BUTTON 4
#define RIGHT_BUTTON 19
#define SWITCH 21

int check_water();
int wait_for_water(int cycle);
void clear_pix();
void water_filled();
void pause_wait_timer();
void fade_random_pix();
void fade();

Pixels pix;
unsigned int pause_used;

void setup(void){
  DDRD = BIT5;
  DDRB &= ~(BIT5|BIT6);

  pinMode(LEFT_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(SWITCH, INPUT);

  init_speaker();
  init_neopixel_blit();
}

void loop(void){
  /* Clearing pixels, setting pause used to false. */ 
  clear_pix();
  pause_used = 0;

  if(check_water() == EMPTY){
    int cycle = 0;

    while(wait_for_water(cycle++) == EMPTY);
    water_filled();
  }else{
    if(digitalRead(SWITCH)){
      fade();
    }else{
      clear_pix();
      delay(2500);
    }
  }
}

/* Capacitance check. */
int check_water(){
  unsigned int cnt = 0, sum = 0, i;
  unsigned int false_negative = 3, false_positive = 3;

  while(1){
    cnt = 0;
    sum = 0;

    DDRD |= BIT6;
    PORTD |= BIT6;
    PORTD |= BIT5;
    delay(10);
    DDRD &= ~BIT6;
    PORTD &= ~BIT6;
    PORTD &= ~BIT5;

    /* Sums the count so we have a more accurate disparity between filled and not filled. */
    for(i = 0; i < 100; i++){
      while(PIND & BIT6) {
        cnt++;
        if(cnt>300)break;
      }
      sum += cnt;
    }

    /* Checks that a false negative/positive did not occur for at least three times in a row. */
    if(sum > 2000){
      if(--false_positive == 0){
        break;
      }
      if(false_negative < 3){
        false_negative = 3;
      }
    }else{
      if(--false_negative == 0){
        break;
      }
      if(false_positive < 3){
        false_positive = 3;
      }
    }
  }
  
  if(sum > 2000){
    return FULL;
  }else{
    return EMPTY;
  }
}

/* Plays the "angry" mode, increasing speed, brightness, and frequency until a (very annoying) threshold is reached. */
int wait_for_water(int cycle){
  int i = 0;

  clear_pix();

  if(cycle > 20){
    cycle = 20;
  }
  
  for(i = 0;i < 10; i++){
    pix[i][PIX_GRN] = 0;
    pix[i][PIX_RED] = 55+10*cycle;
    pix[i][PIX_BLU] = 0;
    delay(120-(cycle*5));
    neopixel_blit(pix);
  }

  if(cycle > 5){
    speaker_tone((cycle-5)*70, 1000-(cycle*30));
  }
  delay(120-(cycle*5));
  clear_pix();

  for(i = 0;i < 10; i++){
    pix[i][PIX_GRN] = 0;
    pix[i][PIX_RED] = 55 + 10*cycle;
    pix[i][PIX_BLU] = 0;
  }

  delay(120-(cycle*5));
  neopixel_blit(pix);
  delay(220-(cycle*5));

  /* Lets you pause once. */
  if((digitalRead(LEFT_BUTTON) || digitalRead(RIGHT_BUTTON))
     && pause_used == 0){
    pause_wait_timer();
    pause_used = 1;
  }
  
  return check_water();
}

/* Plays a small light show with a ticking sound to represent a paused state. */
void pause_wait_timer(){
  int pause_duration = 15000;

  while(check_water()== EMPTY && pause_duration > 0){
    clear_pix();
    delay(50);

    pix[1][PIX_GRN] = 100;
    pix[3][PIX_GRN] = 100;
    pix[6][PIX_GRN] = 100;
    pix[8][PIX_GRN] = 100;

    neopixel_blit(pix);

    speaker_tone(700, 40);
    speaker_tone(720, 40);

    delay(300);
    pause_duration -= 400;

    clear_pix();
    delay(50);

    pix[0][PIX_BLU] = 100;
    pix[2][PIX_BLU] = 100;
    pix[4][PIX_BLU] = 100;
    pix[5][PIX_BLU] = 100;
    pix[7][PIX_BLU] = 100;
    pix[9][PIX_BLU] = 100;

    neopixel_blit(pix);

    speaker_tone(700, 40);
    speaker_tone(720, 40);

    delay(300);
    pause_duration -= 400;
  }
}

void clear_pix(){
  int i = 0;

  for(i = 0; i < 10; i++){
    pix[i][PIX_GRN] = 0;
    pix[i][PIX_RED] = 0;
    pix[i][PIX_BLU] = 0;
  }
  neopixel_blit(pix);
}

/* Plays the "happy" mode, consisting of a rainbow light show and a happy tone. */
void water_filled(){
  pix[0][PIX_RED] = 255;
  pix[0][PIX_GRN] = 0;
  pix[0][PIX_BLU] = 0;
  delay(50);
  neopixel_blit(pix);

  pix[1][PIX_RED] = 255;
  pix[1][PIX_GRN] = 70;
  pix[1][PIX_BLU] = 0;
  delay(50);
  neopixel_blit(pix);

  pix[2][PIX_RED] = 255;
  pix[2][PIX_GRN] = 175;
  pix[2][PIX_BLU] = 0;
  delay(50);
  neopixel_blit(pix);

  pix[3][PIX_RED] = 255;
  pix[3][PIX_GRN] = 255;
  pix[3][PIX_BLU] = 0;
  delay(50);
  neopixel_blit(pix);

  pix[4][PIX_RED] = 188;
  pix[4][PIX_GRN] = 255;
  pix[4][PIX_BLU] = 0;
  delay(50);
  neopixel_blit(pix);

  pix[5][PIX_RED] = 0;
  pix[5][PIX_GRN] = 255;
  pix[5][PIX_BLU] = 0;
  delay(50);
  neopixel_blit(pix);

  pix[6][PIX_RED] = 0;
  pix[6][PIX_GRN] = 255;
  pix[6][PIX_BLU] = 160;
  delay(50);
  neopixel_blit(pix);

  pix[7][PIX_RED] = 0;
  pix[7][PIX_GRN] = 255;
  pix[7][PIX_BLU] = 255;
  delay(50);
  neopixel_blit(pix);

  pix[8][PIX_RED] = 0;
  pix[8][PIX_GRN] = 0;
  pix[8][PIX_BLU] = 255;
  delay(50);
  neopixel_blit(pix);

  pix[9][PIX_RED] = 120;
  pix[9][PIX_GRN] = 0;
  pix[9][PIX_BLU] = 255;
  delay(50);
  neopixel_blit(pix);
  delay(200);

  /*************/

  clear_pix();
  delay(200);
  pix[0][PIX_RED] = 255;
  pix[0][PIX_GRN] = 0;
  pix[0][PIX_BLU] = 0;

  pix[1][PIX_RED] = 255;
  pix[1][PIX_GRN] = 70;
  pix[1][PIX_BLU] = 0;
  
  pix[2][PIX_RED] = 255;
  pix[2][PIX_GRN] = 175;
  pix[2][PIX_BLU] = 0;
   
  pix[3][PIX_RED] = 255;
  pix[3][PIX_GRN] = 255;
  pix[3][PIX_BLU] = 0;
   
  pix[4][PIX_RED] = 188;
  pix[4][PIX_GRN] = 255;
  pix[4][PIX_BLU] = 0;
   
  pix[5][PIX_RED] = 0;
  pix[5][PIX_GRN] = 255;
  pix[5][PIX_BLU] = 0;
  
  pix[6][PIX_RED] = 0;
  pix[6][PIX_GRN] = 255;
  pix[6][PIX_BLU] = 160;
  
  pix[7][PIX_RED] = 0;
  pix[7][PIX_GRN] = 255;
  pix[7][PIX_BLU] = 255;
  
  pix[8][PIX_RED] = 0;
  pix[8][PIX_GRN] = 0;
  pix[8][PIX_BLU] = 255;
   
  pix[9][PIX_RED] = 120;
  pix[9][PIX_GRN] = 0;
  pix[9][PIX_BLU] = 255;

  neopixel_blit(pix);

  speaker_tone(198, 250);
  delay(210);
  speaker_tone(250, 250);
  delay(210);
  speaker_tone(500, 300);
  delay(1000);
  clear_pix();
}

/* 
 * A function that fades 3 random neopixels in and out, creating a serene water-like light show. 
 * This light show is played when the filter is full and the switch is on.
 */
void fade_random_pix(){
  int i, j, k;
  int random1 = rand()%10;
  int random2 = rand()%10;
  int random3 = rand()%10;

  while(random2 == random1){
    random2 = rand()%10;
  }

  while(random3 == random1 || random3 == random2) {
    random3 = rand()%10;
  }

  for(i = 0, j = -200, k = -350
	; i < 256; i++, j +=2, k += 2) {
    pix[random1][PIX_BLU] = i;
    if(j>0 && j < 256){
      pix[random2][PIX_BLU] = j;
    }
    if(k>0 && k < 256){
      pix[random3][PIX_BLU] = k;
    }
    
    delay(5);
    neopixel_blit(pix);
  }

  if(check_water == EMPTY){
    clear_pix();
    return;
  }

  for(i = 255; j>=0 ; i--, j-- , k--) {
    if(i >= 0){
      pix[random1][PIX_BLU] = i;
    }
    if(j >= 0 && j < 256){
      pix[random2][PIX_BLU] = j;
    }
    if(k >= 0 && k < 256){
      pix[random3][PIX_BLU] = k;
    }
    
    delay(5);
    neopixel_blit(pix);
  }

  clear_pix();
}

void fade(){ 
  srand(millis());
  clear_pix();

  while(digitalRead(SWITCH) && check_water() == FULL) {
    fade_random_pix();
  }

  clear_pix();
}