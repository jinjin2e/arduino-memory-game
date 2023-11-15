#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);



#define max_stage_level 4//스테이지당 최대 레벨(보여주는 숫자 수) 50 개 까지 설정가능 
#define skip_stage_conv 1//레벨 화면전환 여부 , 1이 화면전환 있음 0이 스킵

////////클리어 사운드 만들 때 사운드만 보려면 0, 0주면 바로 모든 스테이지 클리어 화면으로 넘어가집니다.
////////클리어 사운드 만들 때 사운드만 보려면 0, 0주면 바로 모든 스테이지 클리어 화면으로 넘어가집니다.


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
  SW1=3,SW2,SW3,SW4,LED1,LED2,LED3,SPK,LED4,LED_TIM           //핀을 효율적으로 관리하기 위해서 연결한 하드웨어의 핀 번호대로 이름을 매김
};
enum{
  MENU=0,START,GAME_OVER,CLEAR,                               //state 변수의 상태(프로그램의 흐름)
};
enum{
  stage1_delay=500,stage2_delay=200,stage3_delay=50,stage0=0,stage1,stage2  //여러 딜레이들
};
uint32_t game_tim=0,blink_tim=0;
uint8_t state=0,real_tim=9,a=0,b=7,restart_game=0, fail=0;
uint8_t ran_arr[50],your_arr[50];                                           //랜덤한 값이 들어올 배열과, 입력할 값이 들어올 배열
typedef struct{                                                             //구조체 형식으로 typedef까지 한번에 선언하면 구조체의 이름을 바꿀 수 있음 (선언할 때 사용할 이름은 생략 가능)
  uint8_t random_num = 0;
  uint8_t random_led = 0;
  uint8_t now_stage=0, state_ok=1, LEVEL=0,clicked=0,answer=0;
  uint16_t Delay = 0;
}start;                                                                     //구조체의 새 이름 설정 
start st;                                                                   //구조체 변수 만들기 st라는 start구조체의 변수를 만들었으니 [st.구조체의 멤버] 형식으로 접근 가능
byte conv1[] = {                                                            //화면전환에 사용될 CGRAM 
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


void ld(uint8_t x, uint8_t y, char *str){         //한번에 좌표와 입력하고싶은 문구를 입력받아 출력하는 함수
  lcd.setCursor(x,y);
  lcd.print(str);
}
void BUZ(int freq, int duration, int rep) {       //부저의 주파수, 주기(시간), 반복 횟수 등을 입력받아 출력
  for(int i=0; i<rep; i++){
    tone(SPK,freq);                               //SPK == 핀 번호
    delay(duration);
    noTone(SPK);
    delay(duration);
  }
}
void GAME_TIMER(){                                //게임오버 타이머 
  ld(8,0,"TIMER:");
  lcd.setCursor(14,0);                            //시간을 출력할 곳 설정
  lcd.print(real_tim);                            //real_tim 변수는 millis 타이머를 사용해 1초에 1씩 줄어드는 실제 시간 카운터 타이머
  if(millis()-game_tim >1000){                    //millis 함수는 stm32 systick이랑 동일함 (GetTick())
    real_tim--;                                   
    if(real_tim==0){                              //타임오버 시 게임 종료
      state=GAME_OVER;
      lcd.clear();
      return;
    }
    game_tim=millis();                            //game_tim을 초기화 해줘야 1초마다 if문으로 들어옴
  }
  if(real_tim<5){                                 //남은 시간이 5초 미만이라면 
    if(millis()-blink_tim >300){                  //300ms 주기로 LED블링크
      digitalWrite(LED_TIM,a=!a);           
      blink_tim=millis();                         //초기화를 해줘야 계속 동작
    }
  }
}
void blink_led(uint8_t num){                      //모든 LED를 깜빡이는 함수
  for(int i=0; i<num; i++){
    digitalWrite(LED1,1);
    digitalWrite(LED2,1);
    digitalWrite(LED3,1);
    digitalWrite(LED4,1);
    click_();                                     //이부분 고쳐야함 (delay 말고 tick이나 timer 사용해서 프로그램이 멈추지 않게)
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
  for (int i=0;i<50;i++){                                                       //변수의 끝까지 채움(혹시 모르니까)
    ran_arr[i]=0;                                                               //변수를 채우기 전 초기화
    your_arr[i]=0;                                                              //변수를 채우기 전 초기화
    st.random_num = random(1, 200);                                             //st.random_num 구조체 변수에 1~200 사이의 랜덤한 값을 저장
    if (st.random_num <= 50) st.random_led = LED1;                              //50 아래면 LED1(7) 을 st.random_led 구조체 변수에 저장 
    else if (st.random_num > 50 && st.random_num <= 100) st.random_led = LED2;  //50~100 이라면 "
    else if (st.random_num > 100 && st.random_num <= 150) st.random_led = LED3; //100~150 "
    else if (st.random_num <= 200) st.random_led = LED4;                        // <200 "
    ran_arr[i] = st.random_led;                                                 //그 값을 ran_arr에 저장함 ( 이부분은 불필요한 변수 2개 더 사용한듯, 처음부터 ran_arr[i]로 random 값을 받은 후 if(ran_arr[i]) 로 값 비교 후 ran_arr[i] = [해당 LED] 꼴로 만들면 2개 줄일 수 있을듯
  }
}
void click_(){
     while(!digitalRead(SW1)){      //첫번째 버튼을 눌렀을 때 
    delay(10);                      //바로 동작하지 않고 누른 버튼에 대응되는 LED 점등 
    GAME_TIMER();                   //타이머 준비
    digitalWrite(LED1,1);           //대응되는 LED점등
    if(digitalRead(SW1)) {          //첫번째 버튼을 땠을 때 동작
        if(state==START){           //******만약 게임 중 이라면******
        your_arr[st.clicked]=LED1;  //LED1(7) 을 버튼입력이 저장될 변수에 저장
        st.clicked++;               //클릭 횟수 증가
        digitalWrite(LED1,0);       //불 끔(누르고 있을 때 현재 누르고 있는 LED가 무엇인지 점등 한 후 손가락을 때면 꺼짐)
        Serial.println(st.clicked); 
        break;
      }
      if(state==GAME_OVER){         //*****만약 게임오버 상태에서 버튼을 눌렀다면 *****
         state=MENU;                //다시 게임 시작 대기화면으로 돌아감 
         lcd.clear();               //LCD초기화
         delay(100);                //버그 방지용
         break;
      }
    }
  }
    while(!digitalRead(SW2)){       ////////////버튼이랑 LED번호만 다르고 동작은 동일함///////////////////////
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
  ld(3,0,"Memory Game");                //시작화면 출력 
  if(millis()-blink_tim>500){           //500ms 마다 a를 반전
    a=!a; 
    Serial.println(b);
    if(b==11) b=6;                      //LED가 하나씩 순차적으로 점등되며 버튼 입력을 기다리는 동작
    b++;
    if(b==10) b=11;
    if(b==7) digitalWrite(LED4,0);
    if(b==8) digitalWrite(LED1,0);
    if(b==9) digitalWrite(LED2,0);
    if(b==11) digitalWrite(LED3,0);
    digitalWrite(b,1);
    if(a) ld(0,1,"Press any button");   //500ms 마다 반전되는 a를 이용해서 점멸하는 글자를 만듦
    else  ld(0,1,"                ");   //점멸
    blink_tim=millis();                 //타이머 초기화 
  }
  random_();                            //시작하기 전까지 계속해서 랜덤한 값 생성
  st.LEVEL=0;                           //변수들의 초기화(게임 오버나 클리어 화면에서 다시 메인으로 넘어왔을 때 다시 원활하게 게임을 진행하기 위해)
  st.now_stage=0;
  real_tim=9;
  st.state_ok=1;
  fail=0;
  delay(100);
  if(digitalRead(SW1)==LOW||digitalRead(SW2)==LOW||digitalRead(SW3)==LOW||digitalRead(SW4)==LOW){   //아무 버튼이나 눌렀다면 게임 시작
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
void screen_conv(){             //화면전환 함수
   random_();                   //랜덤한 값 재생성
   lcd.clear();
   ld(0,0,"STAGE   CLEAR!");
   ld(0,1,"Congratulations!");  //축하문자 출력
   lcd.setCursor(6,0);
   lcd.print(st.now_stage+1);   //클리어한 스테이지 
   if(skip_stage_conv==1){      //화면전환 보기 설정을 했다면 
     for(int i=0; i<16; i++){   //16x2lcd를 사용, 전체 칸 0~16까지 반복
      for(int j=1; j<6; j++){   //lcd 한 칸은 5x8의 도트로 이루어짐(도트들은 CGRAM을 사용해서 conv1,conv2 ... 의 byte 변수를 1,2,3,4,5 라는 이름으로 CGRAM 생성 
        lcd.setCursor(i,0);     //16x2중 윗줄 (점점 늘어나게)
        lcd.write(j)            //1,2,3,4,5라는 이름의 CGRAM을 순서대로 출력
        lcd.setCursor(i,1);     //16x2중 아랫줄 (점점 늘어나게)
        lcd.write(j);           //CGRAM 순서대로 출력
        delay(15);
      }                         //화면이 도트로 가득 차면 다시 도트를 없앤다
     }
     for(int i=15; i>0; i--){   //16번째 칸부터 줄어듬
      for(int j=5; j>-1; j--){  //한 칸 안에서 5개의 가로도트(점점 줄어들게)
        lcd.setCursor(i,0);     //윗줄
        lcd.write(j);           //출력
        lcd.setCursor(i,1);     //아랫줄
        lcd.write(j);           //출력
        delay(15);
      }
     }
   }
   if(st.now_stage<2){          //아직 클리어 할 스테이지가 남아있다면 
     lcd.clear();
     ld(5,0,"STAGE ");
     ld(6,1,"Start");
     lcd.setCursor(11,0);
     lcd.print(st.now_stage+2); //시작할 스테이지 표시
     delay(2000);               //읽을 시간동안 대기
     lcd.clear();
   }
   if(st.now_stage>2){          //스테이지를 모두 무사히 클리어했다면 
       state=CLEAR;             //클리어 화면으로 넘어감
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
  lcd.print(st.LEVEL+1);                                          //현재 레벨 표시 
  ld(0,0 ,"STAGE");
  lcd.setCursor(6,0);
  lcd.print(st.now_stage+1);                                      //현재 스테이지 표시
  GAME_TIMER();                                                   //타이머 시작 
  if(st.state_ok==1){                                             //아직 해당 레벨의 LED패턴을 보여주지 않았다면 (동작을 해야한다면)
    for(int i=0; i<st.LEVEL+1; i++){                              //현재레벨만큼 반복했는가?
      if(st.now_stage==2) st.Delay = stage3_delay;                //스테이지의 딜레이 설정(스테이지가 갈수록 짧아짐)
      else st.Delay = st.now_stage ? stage2_delay : stage1_delay;
      digitalWrite(ran_arr[i],1);                                 //랜덤한 패턴을 순서대로 출력 
      delay(st.Delay);                                            //해당 스테이지의 딜레이만큼 대기
      digitalWrite(ran_arr[i],0);                                 //다시 끔 
      delay(100);                                                 // 깜빡이는 패턴 사이 기본 딜레이
      st.state_ok=0;                                              //해당 레벨의 동작 완료
      blink_tim=millis();                                         //딜레이 초기화 
      game_tim=millis();
    }
  }
  click_();                                                       //클릭을 기다림
  if(st.clicked==st.LEVEL+1){                                     //클릭 횟수가 현재 레벨의 LED 점멸 횟수와 동일해지면 (== 버튼을 다 눌렀다면)
    st.clicked=0;                                                 //클릭 횟수 초기화
    for(int i=0; i<st.LEVEL+1; i++){                              //스테이지 레벨의 점멸 횟수만큼 랜덤한 배열과 입력한 배열의 값을 비교
      if(ran_arr[i]==your_arr[i]) st.answer = 1;                  //동일하게 눌렀다면 변수=1
      if(ran_arr[i]!=your_arr[i]) st.answer=0;                    //만약 동일하지 않다면 변수=0
      if(st.answer==0){                                           //동일하게 누르지 않았다면 
        lcd.clear();                                              //화면 지우기
        state=GAME_OVER;                                          //state를 GAME_OVER(define 2)상태로 만들어 while에서 GAME OVER 함수로 넘어가도록 설정
        return;                                                   //while로 튕겨나가기
      }else if(st.answer==1){                                     //알맞게 눌렀다면 
        st.state_ok=1;                                            //다음 스테이지를 출력할 준비
        delay(200);                                               //살짝 기다림(동작이 자연스럽게 느껴지도록) 
      }
    }
    st.LEVEL++;                                                   //레벨 증가
    real_tim=9;                                                   //레벨 타이머 초기화(0이되면 게임오버)
    digitalWrite(LED_TIM,0);                                      //타이머 LED 초기화
    lcd.clear();
    if(state!=GAME_OVER) clear_sound();                           //게임이 계속 진행된다면 클리어 사운드 재생
    if(st.LEVEL==max_stage_level){                                //만약 해당 스테이지의 최대 레벨까지 클리어 시
      st.LEVEL=0;                                                 //레벨 초기화
      screen_conv();                                              //화면전환( 스테이지 클리어 시 특수 화면전환)
      st.now_stage++;                                             //스테이지 증가
    }    
    if(st.now_stage==3) {                                         //만약 3번째 스테이지를 클리어했다면 
      state=CLEAR;                                                //state를 클리어로 만들어 while에서 클리어 함수를 실행
      return;                                                     //while로 튕겨져나감
    }
  }
}
void GAME_OVER_(){
  
  ld(4,0,"GAME OVER");              //게임 오버 시 현재 스테이지와 레벨을 표시함
  ld(0,1, "LEVEL:");
  lcd.setCursor(6,1);
  lcd.print(st.LEVEL);
  ld(8,1 ,"STAGE:");
  lcd.setCursor(14,1);
  lcd.print(st.now_stage+1);
  digitalWrite(LED1,1);
  digitalWrite(LED2,1);
  digitalWrite(LED3,1);
  digitalWrite(LED4,1);             //게임오버 사운드 재생할 동안 LED 점등
  if(fail==0){
    fail_sound();                   //게임오버 사운드 재생(한번만)
  }
  fail=1;                           //재생이 끝났음
  blink_led(3);                     //LED 점멸 
  if(restart_game==1){              //재시작 시 초기화 + 게임 시작 대기화면으로 이동 
    fail=0;
    restart_game=0;
    lcd.clear();
    state==MENU;
  }
}
void setup(){
  lcd.init();
  lcd.createChar(0, none);          //CGRAM 생성 
  lcd.createChar(1, conv1);
  lcd.createChar(2, conv2);
  lcd.createChar(3, conv3);
  lcd.createChar(4, conv4);
  lcd.createChar(5, conv5);
  lcd.backlight();                  //LCD 백라이트 켬
  lcd.clear();
  pinMode(SW1,INPUT);               //입출력 설정
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
  if(state==MENU) MENU_();                //state의 값에 따라서 함수로 구역을 나누어 관리함( 이게 가장 프로그래밍 하기 편했음 )
  else if(state==START) START_();
  else if(state==GAME_OVER) GAME_OVER_();
  else if(state==CLEAR) CLEAR_();
}
