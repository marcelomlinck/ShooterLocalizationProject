/*
Author:		Marcelo Melo Linck
University: 	PUCRS (Porto Alegre) / MSU (Bozeman, MT)
Version:	1.0
Description:	This is the node program. It reads the sound inputs and processes it in order to serially transfer it to the master computer.
Update: 	27/07/2013
*/

#include <DueTimer.h>

//defines
#define   NODEID   3
#define THRESHOLD  15
#define  clockPin  10
#define  BJT       12

//All nodes usable flags
int id = NODEID;           //ID for printing
volatile bool writeFlag[4]; //Array of flags that enables reading
int analog_value[4];      //analog values storage array
int countArray[4];            //counter storage array
int i=0, j=0;            //indexes
volatile int counter = 0;  //counter
volatile bool clockState = false;    //state of the clock

void setup() {
    Serial.begin(115200);
    pinMode(BJT, OUTPUT);
    digitalWrite(BJT, LOW);
    #if (NODEID == 0)
    pinMode(clockPin, OUTPUT);
    digitalWrite(clockPin, LOW);
    #endif
    #if (NODEID>0)
    pinMode(clockPin, INPUT);
    #endif
    for(i=0;i<4;i++)
    {
      analog_value[i] = 0;
      writeFlag[i] = false;
      countArray[i] = 0;
    }
    delay(NODEID*50);
    //Serial.print("Node "); Serial.print(id); Serial.println(" is ready...");
    delay(2000);
    Timer3.attachInterrupt(Clock).start(1000);  // calls Clock function every 1ms
    #if (NODEID>0)
    attachInterrupt(clockPin, SyncCounter, RISING);
    #endif
}

void loop() {
    bool ok = false;
  //if(i>3) i=0;     //creates index to read 
  //if(writeFlag[i])  // if allowed to read
  //{
    //analog_value[i] = analogRead(A0);  //reads
    int val = analogRead(A0);
    int cnt = counter;
    
    i = micros();
    digitalWrite(BJT, HIGH);
    while(micros() < i+128);  // may need to revise this.   -- Sound Capture Window (BJT reset)
    digitalWrite(BJT, LOW);
    SortArray();
    //count[i] = counter;    //record counter
    //if (val > THRESHOLD){   //if the value read is higher then Vt
    
      for(i=0;i<4;i++)
      {
        if(!writeFlag[i])
        {
          analog_value[i] = val;    // reads value from shield
          countArray[i] = cnt;
          writeFlag[i] = true;
          ok = true;
          break;
        }
      }
      if(!ok)
      {
        i = SmallestValue();
        if(val > analog_value[i])
        {
          analog_value[i] = val;
          countArray[i] = cnt;
          writeFlag[i] = true;
        }
      }
    //}
                                                      //if(counter >= NODEID*10 && counter < (NODEID*10)+10)
    if(counter == NODEID*10)//if(writeFlag)  //if it is time to print, print
    {
      SortArray();      //can't print twice in sequence, there are other nodes!
      for(j=0;j<4;j++)
      {
        if(writeFlag[j])      //if the value can't be read, it can be printed
        {
          Serial.print(id);      //prints ID
          Serial.print(": ");     
          Serial.print(analog_value[j]); //prints value
          Serial.print(" - ");
          Serial.println(countArray[j]);  //prints counter recorded
          analog_value[j] = 0;
          countArray[j] = 0;
          writeFlag[j] = false;
        }
      }
    }
    i++;
}

void Clock()
{
  if(counter < 1000)
  {
    #if (NODEID==0)
    if (counter == 0 || counter == 500) 
    {
      clockState = !clockState;
      digitalWrite(clockPin, clockState);
    }
    #endif
    counter++;
  }
  else
  {
    counter = 0;
  }
}

#if (NODEID>0)
//Other nodes flags only
void SyncCounter()
{
  counter = 1;
}
#endif

void SortArray()    //MAGIA NEGRA DE SORTING - may revise this
{
  int a, b, temp;
  bool tempFlag;
  for(a=0;a<3;a++)
  {
    for(b=0;b<3-a;b++)
    {
      if(countArray[j] > countArray[j+1])
      {
        temp = countArray[j];
        countArray[j] = countArray[j+1];
        countArray[j+1] = temp;
        
        temp = analog_value[j];
        analog_value[j] = analog_value[j+1];
        analog_value[j+1] = temp;
        
        tempFlag = writeFlag[j];
        writeFlag[j] = writeFlag[j+1];
        writeFlag[j+1] = tempFlag;
      }
    }
  }
}

int SmallestValue()
{
  int a, temp = 2000, index;
  for(a=0;a<4;a++)
  {
    if(temp > analog_value[a]) 
    {
      temp = analog_value[a];
      index = a;  
    }
  }
  return index;
}
