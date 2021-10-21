/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.
*/
int h = 0;
int m = 0;
int s = 0;
int cent = 0;
int flag = 0;
int mode_state = 0;
int ligado = 2;
int desligado = 8;
//byte set_state = 0;
const int fan_pin = 13;
const int mode_pin = 8;
const int set_pin = 9;
byte mode_once = 0;
byte set_once = 0;


// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);         // initialize serial communication at 9600 bits per second:
  
  pinMode(fan_pin, OUTPUT);
  pinMode(mode_pin, INPUT);
  pinMode(set_pin, INPUT);
  
  digitalWrite(fan_pin, LOW);
}

void loop() {
 
  int sensorValue = analogRead(A0);          // read the input on analog pin 0:
  
  relogio();

  configuracao();
  
  //int hora[3] = {h,m,s};
  // print out the value you read:
  //Serial.println(sensorValue);
  //delay(1);        // delay in between reads for stability

  
  if((m>=ligado) && (flag == 1)){
  digitalWrite(fan_pin, LOW);
  flag = 0;
  m = 0;
  }

  if((m>=desligado) && (flag == 0)){
    digitalWrite(fan_pin, HIGH);
    flag = 1;
    m = 0;
  }
  
}

void relogio(){
   
  cent = cent+1;

  if(cent==100){
    cent = 0;
    s = s+1;
    
    Serial.print(h);
    Serial.print(":");
    Serial.print(m);
    Serial.print(":");
    Serial.println(s);
  }
  
  if(s==60){
    s=0;
    m=m+1;
  }

  if(m==60){
    m = 0;
    h = h+1;
  }
  
  delay(9);
}

void configuracao(){

  if((digitalRead(mode_pin)==HIGH) && (mode_once==0)){
    mode_state = mode_state+1;
    mode_once = 1;
    if(mode_state==5){
      mode_state=0;
    }
  }
  if((digitalRead(mode_pin)==LOW) && (mode_once==1))mode_once=0;
  
  switch(mode_state){
    case 1:
    if((digitalRead(set_pin)==HIGH) && (set_once==0)){
      h = h+1;
      set_once = 1;
      if(h==24){
        h = 0;
      }
    }
    if((digitalRead(set_pin)==LOW) && (set_once==1))set_once=0;
    break;
    case 2:
    if((digitalRead(set_pin)==HIGH) && (set_once==0)){
      m = m+1;
      set_once=1;
      if(m==60){
        m = 0;
      }
    }
    if((digitalRead(set_pin)==LOW) && (set_once==1))set_once=0;
    break;
    case 3:
    if((digitalRead(set_pin)==HIGH) && (set_once==0)){
      desligado = desligado + 1;
      set_once = 1;
    }
    if((digitalRead(set_pin)==LOW) && (set_once==1))set_once=0;
    break;
    case 4:
    if((digitalRead(set_pin)==HIGH) && (set_once==0)){
      ligado = ligado + 1;
      set_once = 1;
    }
    if((digitalRead(set_pin)==LOW) && (mode_once==1))set_once=0;
    break;    
  }
}
