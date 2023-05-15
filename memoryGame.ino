#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);



#define max_stage_level 8//스테이지당 최대 레벨(보여주는 숫자 수) 50 개 까지 설정가능 
#define skip_stage_conv 1//레벨 화면전환 여부 , 1이 화면전환 있음 0이 스킵

////////클리어 사운드 만들 때는 0, 0주면 바로 모든 스테이지 클리어 화면으로 넘어가집니다.


void CLEAR_(){
  ld(0,0,"ALL STAGE CLEAR!");
  ld(0,1,"CONGRATULATIONS!");
  ////////////////////////made your song///////////////////////////////////////
  BUZ(523,40,1);
  BUZ(698,40,1);
  BUZ(880,40,1);
  BUZ(1046,40,1);
  BUZ(523,40,1);
  BUZ(698,40,1);
  BUZ(880,40,1);
  BUZ(1046,40,1);
  delay(1000);

  ////////////////////////////////////////////////////////////////
}
void clear_sound(){
  //////////////////////////////스테이지 클리어 사운드(짧아야함)
  BUZ(523,40,1);
  BUZ(698,40,1);
  BUZ(880,40,1);
  BUZ(1046,40,1);
}
void fail_sound(){////////////////스테이지 클리어 실패 사운드(길이 상관 없음)
  BUZ(1318,40,1);
  BUZ(1046,60,1);
  BUZ(880,70,1);
  BUZ(698,80,1);
  BUZ(523,90,1);
  BUZ(440,100,1);
  BUZ(349,110,1);
  BUZ(261,120,1);
}










enum{
  SW1=3,SW2,SW3,SW4,LED1,LED2,LED3,SPK,LED4,LED_TIM
};
enum{
  MENU=0,START,GAME_OVER,CLEAR,
};
enum{
  stage1_delay=500,stage2_delay=200,stage3_delay=50,stage0=0,stage1,stage2
};
uint32_t game_tim=0,blink_tim=0;
uint8_t state=0,real_tim=9,a=0,b=7,restart_game=0, fail=0;
uint8_t ran_arr[50],your_arr[50];
typedef struct{
  uint8_t random_num = 0;
  uint8_t random_led = 0;
  uint8_t now_stage=0, state_ok=1, LEVEL=0,clicked=0,answer=0;
  uint16_t Delay = 0;
}start;
start st;
byte conv1[] = {
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10
};
byte conv2[] = {
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18
};
byte conv3[] = {
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C
};
byte conv4[] = {
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E
};
byte conv5[] = {
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};
byte none[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};


void ld(uint8_t x, uint8_t y, char *str){
  lcd.setCursor(x,y);
  lcd.print(str);
}
void BUZ(int freq, int duration, int rep) {
  for(int i=0; i<rep; i++){
    tone(SPK,freq);
    delay(duration);
    noTone(SPK);
    delay(duration);
  }
}
void GAME_TIMER(){
  ld(8,0,"TIMER:");
  lcd.setCursor(14,0);
  lcd.print(real_tim);
  if(millis()-game_tim >1000){
    real_tim--;
    if(real_tim==0){
      state=GAME_OVER;
      lcd.clear();
      return;
    }
    game_tim=millis();
  }
  if(real_tim<5){
    if(millis()-blink_tim >300){
      digitalWrite(LED_TIM,a=!a);
      blink_tim=millis();
    }
  }
}
void blink_led(uint8_t num){
  for(int i=0; i<num; i++){
    digitalWrite(LED1,1);
    digitalWrite(LED2,1);
    digitalWrite(LED3,1);
    digitalWrite(LED4,1);
    click_();
    delay(100);
    click_();
    delay(100);
    click_();
    delay(100);
    click_();
    digitalWrite(LED1,0);
    digitalWrite(LED2,0);
    digitalWrite(LED3,0);
    digitalWrite(LED4,0);
    delay(100);
    click_();
    delay(100);
    click_();
    delay(100);
    click_();
  }
}
void random_(){
  for (int i=0;i<50;i++){
    ran_arr[i]=0;
    your_arr[i]=0;
    st.random_num = random(1, 200);
    if (st.random_num <= 50) st.random_led = LED1;
    else if (st.random_num > 50 && st.random_num <= 100) st.random_led = LED2;
    else if (st.random_num > 100 && st.random_num <= 150) st.random_led = LED3;
    else if (st.random_num <= 200) st.random_led = LED4;
    ran_arr[i] = st.random_led;
  }
}
void click_(){
     while(!digitalRead(SW1)){
    delay(10);
    GAME_TIMER();
    digitalWrite(LED1,1);
    if(digitalRead(SW1)) {
        if(state==START){
        your_arr[st.clicked]=LED1;
        st.clicked++;
        digitalWrite(LED1,0);
        Serial.println(st.clicked);
        break;
      }
      if(state==GAME_OVER){
         state=MENU;
         lcd.clear();
         delay(100);
         break;
      }
    }
  }
    while(!digitalRead(SW2)){
      delay(10);
      GAME_TIMER();
      digitalWrite(LED2,1);
    if(digitalRead(SW2)) {
      if(state==START){
        your_arr[st.clicked]=LED2;
        st.clicked++;
        digitalWrite(LED2,0);
        Serial.println(st.clicked);
        break;
      }
      if(state==GAME_OVER){
         state=MENU;
         lcd.clear();
         break;
      }
    }
  }
    while(!digitalRead(SW3)){
      delay(10);
      GAME_TIMER();
      digitalWrite(LED3,1);
    if(digitalRead(SW3)) {
      if(state==START){
        your_arr[st.clicked]=LED3;
        st.clicked++;
        digitalWrite(LED3,0);
        Serial.println(st.clicked);
        break;
      }
      if(state==GAME_OVER){
         state=MENU;
         lcd.clear();
         break;
      }
    }
  }
    while(!digitalRead(SW4)){
      delay(10);
      GAME_TIMER();
      digitalWrite(LED4,1);
    if(digitalRead(SW4)) {
      if(state==START){
        your_arr[st.clicked]=LED4;
        st.clicked++;
        digitalWrite(LED4,0);
        Serial.println(st.clicked);
        break;
      }
      if(state==GAME_OVER){
         state=MENU;
         lcd.clear();
         break;
      }
    }
  }
}

void MENU_(){
  ld(3,0,"Memory Game");
  if(millis()-blink_tim>500){
    a=!a;
    Serial.println(b);
    if(b==11) b=6;
    b++;
    if(b==10) b=11;
    if(b==7) digitalWrite(LED4,0);
    if(b==8) digitalWrite(LED1,0);
    if(b==9) digitalWrite(LED2,0);
    if(b==11) digitalWrite(LED3,0);
    digitalWrite(b,1);
    if(a) ld(0,1,"Press any button");
    else  ld(0,1,"                ");
    blink_tim=millis();
  }
  random_();
  st.LEVEL=0;
  st.now_stage=0;
  real_tim=9;
  st.state_ok=1;
  fail=0;
  delay(100);
  if(digitalRead(SW1)==LOW||digitalRead(SW2)==LOW||digitalRead(SW3)==LOW||digitalRead(SW4)==LOW){
    lcd.clear();
    blink_tim=millis();
    state=START;
    digitalWrite(LED4,0);
    digitalWrite(LED1,0);
    digitalWrite(LED2,0);
    digitalWrite(LED3,0);
    delay(1000);
  }
}
void screen_conv(){
   random_();
   lcd.clear();
   ld(0,0,"STAGE   CLEAR!");
   ld(0,1,"Congratulations!");
   lcd.setCursor(6,0);
   lcd.print(st.now_stage+1);
   if(skip_stage_conv==1){
     for(int i=0; i<16; i++){
      for(int j=1; j<6; j++){
        lcd.setCursor(i,0);
        lcd.write(j);
        lcd.setCursor(i,1);
        lcd.write(j);
        delay(15);
      }
     }
     for(int i=15; i>0; i--){
      for(int j=5; j>-1; j--){
        lcd.setCursor(i,0);
        lcd.write(j);
        lcd.setCursor(i,1);
        lcd.write(j);
        delay(15);
      }
     }
   }
   if(st.now_stage<2){
     lcd.clear();
     ld(5,0,"STAGE ");
     ld(6,1,"Start");
     lcd.setCursor(11,0);
     lcd.print(st.now_stage+2);
     delay(2000);
     lcd.clear();
   }
   if(st.now_stage>2){
       state=CLEAR;
       lcd.clear();
   }
}
void START_(){
  if(max_stage_level==0) {
    state=CLEAR;
    return;
  }
  ld(0,1, "LEVEL:");
  lcd.setCursor(6,1);
  lcd.print(st.LEVEL+1);
  ld(0,0 ,"STAGE");
  lcd.setCursor(6,0);
  lcd.print(st.now_stage+1);
  GAME_TIMER();
  if(st.state_ok==1){
    for(int i=0; i<st.LEVEL+1; i++){
      if(st.now_stage==2) st.Delay = stage3_delay;
      else st.Delay = st.now_stage ? stage2_delay : stage1_delay;
      digitalWrite(ran_arr[i],1);
      delay(st.Delay);
      digitalWrite(ran_arr[i],0);
      delay(100);
      st.state_ok=0;
      blink_tim=millis();
      game_tim=millis();
    }
  }
  click_();
  if(st.clicked==st.LEVEL+1){
    st.clicked=0;
    for(int i=0; i<st.LEVEL+1; i++){
      if(ran_arr[i]==your_arr[i]) st.answer = 1;
      if(ran_arr[i]!=your_arr[i]) st.answer=0;
      if(st.answer==0){
        lcd.clear();
        state=GAME_OVER;
        return;
      }else if(st.answer==1){
        st.state_ok=1;
        delay(200);
      }
    }
    st.LEVEL++;
    real_tim=9;
    digitalWrite(LED_TIM,0);
    lcd.clear();
    if(state!=GAME_OVER) clear_sound();
    if(st.LEVEL==max_stage_level){
      st.LEVEL=0;
      screen_conv();
      st.now_stage++;
    }    
    if(st.now_stage==3) {
      state=CLEAR;
      return;
    }
  }
}
void GAME_OVER_(){
  
  ld(4,0,"GAME OVER");
  ld(0,1, "LEVEL:");
  lcd.setCursor(6,1);
  lcd.print(st.LEVEL);
  ld(8,1 ,"STAGE:");
  lcd.setCursor(14,1);
  lcd.print(st.now_stage+1);
  digitalWrite(LED1,1);
  digitalWrite(LED2,1);
  digitalWrite(LED3,1);
  digitalWrite(LED4,1);
  if(fail==0){
    fail_sound();
  }
  fail=1;
  blink_led(3);
  if(restart_game==1){
    fail=0;
    restart_game=0;
    lcd.clear();
    state==MENU;
  }
}
void setup(){
  lcd.init();
  lcd.createChar(0, none);
  lcd.createChar(1, conv1);
  lcd.createChar(2, conv2);
  lcd.createChar(3, conv3);
  lcd.createChar(4, conv4);
  lcd.createChar(5, conv5);
  lcd.backlight();
  lcd.clear();
  pinMode(SW1,INPUT);
  pinMode(SW2,INPUT);
  pinMode(SW3,INPUT);
  pinMode(SW4,INPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  pinMode(SPK,OUTPUT);
  pinMode(LED_TIM,OUTPUT);
  ld(3,0,"Memory Game");
  ld(0,1,"Press any button");
  Serial.begin(9600);
}

void loop(){
  if(state==MENU) MENU_();
  else if(state==START) START_();
  else if(state==GAME_OVER) GAME_OVER_();
  else if(state==CLEAR) CLEAR_();
}
