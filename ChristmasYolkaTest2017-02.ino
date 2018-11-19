#include <DMdriver.h>
#include <IRLib.h>
#include <RemoteDef.h>

#define RECV_PIN A0 //with PCI 0 = pin2, 1 = pin3
IRrecv irrecv(RECV_PIN);
IRrecv  My_Receiver(RECV_PIN);
IRdecode My_Decoder;
uint32_t IRresult;

volatile uint8_t volume = 3;
volatile uint16_t spd = 15;

uint8_t mode = 0;
const uint8_t maxMode = 5;
boolean multimode = false;
uint16_t multiCounter = 0;


#define TAIL 20 // the length of the 'LED tail'
#define STEP 2// speed of animation; more = faster
#define DMNUMBER 3 // number of DM LED drivers in your chain

//variable arrays for the justLights() function
boolean LedOn[48];
uint8_t fast[48];
uint16_t falseColor[48];
uint16_t lightscount[48];
uint8_t red, green, blue;

uint8_t ledseq[48] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
              17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
              33,34,35,36,37,38,39,40,41,42,43,44,45,46,47};

const uint8_t lights[360]={
  0,   0,   0,   0,   0,   1,   1,   2, 
  2,   3,   4,   5,   6,   7,   8,   9, 
 11,  12,  13,  15,  17,  18,  20,  22, 
 24,  26,  28,  30,  32,  35,  37,  39, 
 42,  44,  47,  49,  52,  55,  58,  60, 
 63,  66,  69,  72,  75,  78,  81,  85, 
 88,  91,  94,  97, 101, 104, 107, 111, 
114, 117, 121, 124, 127, 131, 134, 137, 
141, 144, 147, 150, 154, 157, 160, 163, 
167, 170, 173, 176, 179, 182, 185, 188, 
191, 194, 197, 200, 202, 205, 208, 210, 
213, 215, 217, 220, 222, 224, 226, 229, 
231, 232, 234, 236, 238, 239, 241, 242, 
244, 245, 246, 248, 249, 250, 251, 251, 
252, 253, 253, 254, 254, 255, 255, 255, 
255, 255, 255, 255, 254, 254, 253, 253, 
252, 251, 251, 250, 249, 248, 246, 245, 
244, 242, 241, 239, 238, 236, 234, 232, 
231, 229, 226, 224, 222, 220, 217, 215, 
213, 210, 208, 205, 202, 200, 197, 194, 
191, 188, 185, 182, 179, 176, 173, 170, 
167, 163, 160, 157, 154, 150, 147, 144, 
141, 137, 134, 131, 127, 124, 121, 117, 
114, 111, 107, 104, 101,  97,  94,  91, 
 88,  85,  81,  78,  75,  72,  69,  66, 
 63,  60,  58,  55,  52,  49,  47,  44, 
 42,  39,  37,  35,  32,  30,  28,  26, 
 24,  22,  20,  18,  17,  15,  13,  12, 
 11,   9,   8,   7,   6,   5,   4,   3, 
  2,   2,   1,   1,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0};
// set up the object. Change the values according to your setup
DMdriver Test (DM634, DMNUMBER, 9);

int curpos;
int numLeds = 16*DMNUMBER;


void setup() {
Test.init();
My_Receiver.enableIRIn();
//Test.setGlobalBrightness(127); // needed for the DM634 chips as they tend to
                                 // change their brightness on startup
randomSeed(analogRead(A3));
}

void shuffle()
{
  for (byte k=47; k>1; k--)
  {
    byte ctemp = ledseq[k];
    byte cpos = random(48);
    ledseq[k] = ledseq[cpos];
    ledseq[cpos] = ctemp;
  }
}

boolean checkIR(volatile uint32_t ash)
{
  boolean result = false;
  justWait(ash);
if (My_Receiver.GetResults(&My_Decoder)) 
    { 
    My_Receiver.resume();
    My_Decoder.decode();
   // if ((My_Decoder.value!=0xFFFFFFFF) && (My_Decoder.value>0)) 
      IRresult=My_Decoder.value;
       switch (IRresult) {
        case CarMP3CHminus:
             
             if (mode>0) {
              mode--;
              multimode = false;
              result = true;
              Test.clearAll();
             } else
              {
                mode = maxMode;
              result = true;
              Test.clearAll();
              }
             //mode%=3;
             

             
             
             break;

        case CarMP3CHplus:
             
             if (mode<maxMode) {
              mode++;
              multimode = false;
              result = true;
              Test.clearAll();
             } else
              {
                mode = 0;
                multimode = false;
              result = true;
              Test.clearAll();
              }
             //mode%=3;
             

             
             
             break;
             
        case CarMP3volPlus:
             if ( volume<7) volume++;
             break;

         case CarMP3volMinus:
             if ( volume>0) volume--;
             break;

         case CarMP3prev:
             if ( spd<30) spd+=2;
             break;

         case CarMP3next:
             if ( spd>2) spd-=2;
             break;


        default: result = false; break;
       
    
       
      }
     
    } 
 if (result) 
   {volume=3; spd = 15;
     if (mode == maxMode)
       {multimode = true; mode = 4;
       //random(5);
       multiCounter = 0;}
     }
 return (result);// return (true); else return(false);
}

void testWork()
{
  for (int k = 0; k < 8; k++) {
    Test.clearAll();
  
    for (byte kk=0; kk<6; kk++)
    {
      Test.setPoint(kk*8+k, 255);
    }
    Test.sendAll();
    while (!checkIR(1));
  }
}

void loop()
{
  switch (mode){
    case 0: loopByColor(); break;
    case 1: randomFill(); break;
    case 2: randomBlink(); break;
    case 3: justLights();break;
    case 4: loopSnake(); break;


    default: break;
  }
  //Test.clearAll();
  //loopSnake();
  //loopByColor();
  //allFadeIn();
  //loopsQuest(); //strange
  //test();
  //justLights(120, 3000);
  //randomFill();
  //randomBlink();
  //testIR();
  //testWork();
}




void loopByColor()
{
  //int volume=5;
  if (multimode)
    {volume = 4 ;
     spd = 13;
    }
  if (volume==0){
  for (int k = 0; k < 8; k++)
  {
    Test.clearAll();
    for (int y=0; y<240; y++)
    {
      for (byte kk=0; kk<6; kk++)
      {
      Test.setPoint(kk*8+k, lights[y]);
      //Test.setPoint(kk*8+k+1, lights[y]);
      }
      Test.sendAll();
      uint32_t spdTemp = spd;
       if (checkIR(spdTemp*200)) return;
    }
  }
 }
  else{
   for (int k = 0; k < 8; k++)
  {
    //Test.clearAll();
    for (int y=0; y<120; y++)
    {
      for (byte kk=0; kk<6; kk++)
      {
      Test.setPoint(kk*8+k, lights[y+120]);
      Test.setPoint((kk*8+k+volume)%48, lights[y]);
      //Test.setPoint(kk*8+k+1, lights[y]);
      }
      Test.sendAll();
      uint32_t spdTemp = spd;
       if (checkIR(spdTemp*200)) return;
    }
  } 
  }

 if (multimode){
   multiCounter++;
   if ((multiCounter>10) && (random(5) == 0)) 
     {mode = random(5); multiCounter = 0;}
 }
  
   
}



void randomFill()
{
  if (multimode)
    {volume = 4 ;
     spd = random(6) + 1;
    }
  shuffle();
  Test.clearAll();
  for (byte k=0; k<48; k++)
     { 
       Test.setPoint(ledseq[k], 255);
       Test.sendAll();
       uint32_t spdTemp = spd;
       if (checkIR(spdTemp*7000)) return;
       
     }

  for (byte k=0; k<48; k++)
     { 
       Test.setPoint(ledseq[k], 0);
       Test.sendAll();
       uint32_t spdTemp = spd;
       if (checkIR(spdTemp*10000)) return;
       
     }

 if (multimode){
   multiCounter++;
   if ((multiCounter>6) && (random(5) == 0)) 
     {mode = random(5); multiCounter = 0;}
 }
 
}

void randomBlink()
{
  if (multimode)
    {volume = 3 ;
     spd = random(7)+1;
    }
  shuffle();
  Test.clearAll();
  for (byte k=0; k<48; k++)
     { 
       
       for (byte y=0; y<((volume+1)*2); y++) Test.setPoint(ledseq[(k+y)%48], 255);
       Test.sendAll();
       uint32_t spdTemp = spd;
       if (checkIR(spdTemp*10000)) return;
       Test.setPoint(ledseq[k], 0);
       Test.sendAll();
     }
 if (multimode){
   multiCounter++;
   if ((multiCounter>10) && (random(5) == 0)) 
     {mode = random(5); multiCounter = 0;}
 }
}

void justLights()
{
  if (multimode)
    {volume = 7 ;
     spd = 11;
    }
  uint16_t rfactor = 360-(volume*50);
for(uint8_t kcount=0; kcount<48; kcount++){

      if ((!LedOn[kcount])&& (random(rfactor)==1)){        
        LedOn[kcount]=true;
        fast[kcount]=random(8)+1;
        lightscount[kcount]=0;}
      if (LedOn[kcount]){
        falseColor[kcount]=lights[lightscount[kcount]];
        lightscount[kcount]=lightscount[kcount]+fast[kcount];
        if (lightscount[kcount]>255) {LedOn[kcount]=false;falseColor[kcount]=0;};
      }

   Test.setPoint(kcount, falseColor[kcount]);
   Test.sendAll();
  }
  uint32_t spdTemp = spd;
       if (checkIR(spdTemp*1000)) return;
 if (multimode){
   multiCounter++;
   if ((multiCounter>1000)) 
     {mode = random(5); multiCounter = 0;}
 }
}

void loopSnake() {
  if (multimode)
    {volume = 4 ;
     spd = 11;
    }
int count = 0;
uint8_t tail = (volume+1)*5;
uint8_t stp = 42-spd;
byte stepLeds = 255/tail;

Test.clearAll();
do
{
  Test.setPoint((curpos+1)%numLeds, count);
  Test.setPoint(curpos, 255);
  for (int k = 1; k<tail; k++)
  {
    Test.setPoint((curpos+numLeds-k)%numLeds, stepLeds*(tail-k)+(255-count)/tail);
  }
Test.sendAll();
count+=stp;
if (checkIR(spd*500)) return;
}
while (count<256);
curpos++;
if (curpos>47) curpos = 0;

if (multimode){
   multiCounter++;
   if ((multiCounter>300) && (random(50) == 0)) 
     {mode = random(5); multiCounter = 0;}
 }
}

void test()
{
  Test.clearAll();
  for (int k = 0; k<16; k++)
  {
    Test.setPoint(k, 150);
  }
  Test.sendAll();
  delay(1000);
  Test.clearAll();
  for (int k = 0; k<16; k++)
  {
    Test.setPoint(k+16, 150);
  }
  Test.sendAll();
  delay(1000);
  Test.clearAll();
  for (int k = 0; k<16; k++)
  {
    Test.setPoint(k+32, 150);
  }
  Test.sendAll();
  delay(1000);
  
}

void allFadeIn()
{
  for (int y = 0; y< 256; y++)
  {  for (int k = 0; k<48; k++)
  {
    //Test.clearAll();
    Test.setPoint(k, y);
    //Test.sendAll();
    //delay(100);
  }
  Test.sendAll();
  delay(10);
  }
}

void loopsQuest()
{
  for (int k=20; k<256; k++)
  {
    Test.clearAll();
    for (int y=0; y<6; y++)
    Test.setPoint(y*8 + k, 200);
    Test.sendAll();
    delay(1000);
  }
}




void justWait (uint32_t period)
{
  for (uint32_t z = 0; z<period; z++) __asm__("nop\n\t");  
}

/*** bavkup stuff 
void testIR()
{
if (My_Receiver.GetResults(&My_Decoder)) 
    { 
    My_Receiver.resume();
    My_Decoder.decode();
   // if ((My_Decoder.value!=0xFFFFFFFF) && (My_Decoder.value>0)) 
      {IRresult=My_Decoder.value;
       switch (IRresult) {
        case CarMP3CHminus:
             Test.setPoint(0, 255);
             Test.sendAll();
                        
             break;
        case CarMP3CHplus:
             Test.setPoint(3, 255);
             Test.sendAll();
                        
             break;
         case CarMP3CH:
             Test.setPoint(2, 255);
             Test.sendAll();
                        
             break;
        default: break;
       }
    
       
      }
    } 

}

*/
