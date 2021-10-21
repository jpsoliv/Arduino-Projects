
#include "Wire.h"
#include <LiquidCrystal_I2C.h>

#define DS1307_ADDRESS 0x68

// Modulo I2C display no endereco 0x3F
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

byte zero = 0x00;

int led_io = 7;
int pot_io = 0;
int mode_io = 12;
int set_io = 11;

int relogio[3] = {00, 00, 00};    //{segundos, minutos, hora}
int liga[2] = {59, 18};           //{minutos, hora}
int desliga[2] = {00, 19 };        //{minutos, hora}

void setup() {

  Serial.begin(9600);             // initialize serial communication at 9600 bits per second:

  pinMode(led_io, OUTPUT);
  pinMode(mode_io, INPUT);
  pinMode(set_io, INPUT);

  Wire.begin();                   // inicializa comunicação com RC1307

  lcd.begin (16, 2);

  // Aciona o LCD
  TCCR2B = (TCCR2B & 0b11111000) | 0x07;

  escreve_hora_RTC(relogio[0], relogio[1], relogio[2]);     //inicializa a hora no lcd (usado dado que o RC da pau as vezes)

  print_horario_lcd(relogio[0], relogio[1], relogio[2]);    //imprime no lcd pela primeira vez
  print_liga_lcd(liga[0], liga[1]);
  print_desliga_lcd(desliga[0], desliga[1]);

}

void loop() {

  int pot_read;
  static int liga_status = 0;
  static byte mode = 0;

  contagem_hora();    // roda o tempo do relógio

  mode = mode_press ();  // Adquire o valor do MODE que controla os ajustes do relogio/controlador

  ajuste_horario(mode);      // Ajuste de hora atual

  ajuste_horario_liga(mode);   // Ajuste de hora que liga o LED

  ajuste_horario_desliga(mode); // Ajuste de hora que desliga o LED

  liga_status = liga_desliga_comando(liga[0], liga[1], desliga[0], desliga[1]);  //encontra o status se o led deve estar ligado ou desligado

  pot_read = analog_pot_read();

  liga_led_PWM(liga_status, pot_read);  //liga o led de acrordo com o status proveniente do liga_status

}

void contagem_hora() {
  int segundo;
  int minuto;
  int hora;
  int printLcdSec = relogio[0];

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  segundo = ConverteparaDecimal(Wire.read());
  minuto = ConverteparaDecimal(Wire.read());
  hora = ConverteparaDecimal(Wire.read() & 0b111111);

  relogio[0] = segundo;
  relogio[1] = minuto;
  relogio[2] = hora;

  if (printLcdSec != segundo) {
    print_horario_lcd(relogio[0], relogio[1], relogio[2]);
  }

}

byte mode_press() {
  static int mode_state = 0;
  byte mode_flag = 0;
  static byte count_print = 0;
  static byte zera_mode_state_tempo = relogio[0];
  static byte zera_mode_state_count = 0;
  static byte zera_mode_state = 0;
  static byte set_status = 0;

  while (digitalRead(mode_io) == HIGH) {
    mode_flag = 1;
    delay(25);
  }
  if (mode_flag == 1) {
    mode_flag = 0;
    mode_state++;
    //zera_mode_state = 1;
    //zera_mode_state_tempo = relogio[0];
    //    Serial.print(mode_state);
    if (mode_state > 7) {
      mode_state = 0;
    }
  }

  if (count_print != mode_state) {
    print_horario_lcd(relogio[0], relogio[1], relogio[2]);
    print_liga_lcd(liga[0], liga[1]);
    print_desliga_lcd(desliga[0], desliga[1]);
  }

  //  set_status = set_press();
  //
  //  if (set_status == 1) {
  //    zera_mode_state_tempo = relogio[0];
  //
  //    set_status = 0;
  //
  //  }
  //
  //  if ((zera_mode_state == 1) && (relogio[0] - zera_mode_state_tempo > 10)) { // zera mode_state se o mode ou set não for pressionado por 10 seg
  //    Serial.print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
  //    zera_mode_state = 0;
  //    zera_mode_state_tempo = relogio[0];
  //    mode_state = 0;
  //  }

  count_print = mode_state;

  return mode_state;
}

byte set_press() {
  byte set_flag = 0;

  while (digitalRead(set_io) == HIGH) {
    set_flag = 1;
    delay(25);
  }
  //  Serial.print(set_flag);
  return set_flag;
}

/////////////////////////////////////////
// mode_state = 1: ajuste hora relógio
// mode_state = 2: ajuste minuto relógio
// mode_state = 3: zera segundos relógio
// mode_state = 4: ajuste hora liga
// mode_state = 5: ajuste minuto liga
// mode_state = 6: ajuste hora desliga
// mode_state = 7: ajuste minuto desliga
/////////////////////////////////////////

void ajuste_horario(byte mode_state) {

  static byte set_flag = 0;
  static int set_state = 0;
  int segundo = relogio[0];
  int minuto = relogio[1];
  int hora = relogio[2];

  switch (mode_state) {
    case 1:
      pisca_mode(mode_state);
      set_flag = set_press();
      //      Serial.print(set_flag);
      if (set_flag == 1) {

        set_flag = 0;
        hora++;
        escreve_hora_RTC(segundo, minuto, hora);
        if (hora == 24) {
          hora = 0;
        }
      }
      break;

    case 2:
      pisca_mode(mode_state);
      set_flag = set_press();
      if (set_flag == 1) {
        set_flag = 0;
        minuto++;
        escreve_hora_RTC(segundo, minuto, hora);
        if (minuto == 60) {
          minuto = 0;
        }
      }
      break;
    case 3:
      pisca_mode(mode_state);
      set_flag = set_press();
      if (set_flag == 1) {
        set_flag = 0;

        segundo = 0;
        //        minuto++;

        escreve_hora_RTC(segundo, minuto, hora);
      }
  }
  relogio[0] = segundo;
  relogio[1] = minuto;
  relogio[2] = hora;
  //  print_horario_lcd(segundo, minuto, hora);  // mostra o horário no lcd
}

void escreve_hora_RTC(int segundo, int minuto, int hora) {
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //Stop no CI para que o mesmo possa receber os dados
  Wire.write(ConverteParaBCD(segundo));
  Wire.write(ConverteParaBCD(minuto));
  Wire.write(ConverteParaBCD(hora));
  Wire.write(zero);
  Wire.endTransmission();
}

void ajuste_horario_liga (byte mode_state) {

  byte set_flag = 0;
  static int set_state = 0;
  int minuto = liga[0];
  int hora = liga[1];

  switch (mode_state) {
    case 4:
      pisca_mode(mode_state);
      set_flag = set_press();
      if (set_flag == 1) {
        set_flag = 0;
        hora++;
        if (hora == 24) {
          hora = 0;
        }
      }
      break;

    case 5:
      pisca_mode(mode_state);
      set_flag = set_press();
      if (set_flag == 1) {
        set_flag = 0;
        minuto++;
        if (minuto == 60) {
          minuto = 0;
        }
      }
      break;
  }
  liga[0] = minuto;
  liga[1] = hora;
  //  print_liga_lcd(minuto, hora);  // mostra o horário no lcd
}

void ajuste_horario_desliga(byte mode_state) {
  byte set_flag = 0;
  static int set_state = 0;
  int minuto = desliga[0];
  int hora = desliga[1];

  switch (mode_state) {
    case 6:
      pisca_mode(mode_state);
      set_flag = set_press();
      if (set_flag == 1) {
        set_flag = 0;
        hora++;
        if (hora == 24) {
          hora = 0;
        }
      }
      break;

    case 7:
      pisca_mode(mode_state);
      set_flag = set_press();
      if (set_flag == 1) {
        set_flag = 0;
        minuto++;
        if (minuto == 60) {
          minuto = 0;
        }
      }
      break;
  }
  desliga[0] = minuto;
  desliga[1] = hora;
  //  print_liga_lcd(minuto, hora);  // mostra o horário no lcd

}

int liga_desliga_comando(int minuto_liga, int hora_liga, int minuto_desliga, int hora_desliga) {
  int liga_decimal;
  int desliga_decimal;
  int hora_atual_decimal;
  int minutos_deve_estar_ligado;
  static int minutos_ligado = 0;
  static int minutos_mudanca = relogio[1];
  int flag;
  int vira_madruga;
  static int virou = 0;
  int desliga_no_instante_decimal;
  int desliga_no_instante[1];

  hora_atual_decimal = relogio[2] * 60 + relogio[1];
  liga_decimal = hora_liga * 60 + minuto_liga;
  desliga_decimal = hora_desliga * 60 + minuto_desliga;

  if (desliga_decimal > liga_decimal) {
    minutos_deve_estar_ligado = desliga_decimal - liga_decimal;
    vira_madruga = 0;
  }
  else {
    minutos_deve_estar_ligado = 1440 + desliga_decimal - liga_decimal;
    vira_madruga = 1;
  }

  if (vira_madruga == 0) {
    if ((hora_atual_decimal >= liga_decimal) && (hora_atual_decimal < desliga_decimal)) {
      flag = 1;
    }
    else {
      flag = 0;
    }
  }

  if (vira_madruga == 1) {

    if (hora_atual_decimal == 0) {
      virou = 1;
    }
    if (virou == 0) {
      if ((hora_atual_decimal >= liga_decimal ) && (hora_atual_decimal < desliga_decimal + 1440)) {
        flag = 1;
      }
      else {
        flag = 0;
      }
    }
    if (virou == 1) {
      if (hora_atual_decimal < desliga_decimal) {
        flag = 1;;
      }
      else {
        flag = 0;
        virou = 0;
      }
    }
  }

  return flag;
}


int analog_pot_read() {
  int val;
  val = analogRead(pot_io);
  delay(1);
  print_pot_led(val);
  return val / 4;
}

void liga_led_PWM(int status_ligado, int PWM_val) {
  if (status_ligado == 1) {
    digitalWrite(led_io, HIGH);
    //    analogWrite(led_io, PWM_val);
  }
  else {
    digitalWrite(led_io, LOW);
  }
}


//void print_hora_monitor(int segundo, int minuto, int hora) {
//  Serial.print(hora);
//  Serial.print(":");
//  Serial.print(minuto);
//  Serial.print(":");
//  Serial.println(segundo);
//}

void print_horario_lcd(int segundo, int minuto, int hora) {

  if (hora < 10) {
    lcd.setCursor(0, 0);
    lcd.print(0);
    lcd.setCursor(1, 0);
    lcd.print(hora);
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print(hora);
  }

  lcd.print(":");

  if (minuto < 10) {
    lcd.setCursor(3, 0);
    lcd.print(0);
    lcd.setCursor(4, 0);
    lcd.print(minuto);
  }
  else {
    lcd.setCursor(3, 0);
    lcd.print(minuto);
  }

  lcd.print(":");

  if (segundo < 10) {
    lcd.setCursor(6, 0);
    lcd.print(0);
    lcd.setCursor(7, 0);
    lcd.print(segundo);
  }
  else {
    lcd.setCursor(6, 0);
    lcd.print(segundo);
  }
}

void print_liga_lcd(int minuto, int hora) {
  lcd.setCursor(0, 1);
  lcd.print("L=");

  if (hora < 10) {
    lcd.setCursor(2, 1);
    lcd.print(0);
    lcd.setCursor(3, 1);
    lcd.print(hora);
  }
  else {
    lcd.setCursor(2, 1);
    lcd.print(hora);
  }

  lcd.print(":");

  if (minuto < 10) {
    lcd.setCursor(5, 1);
    lcd.print(0);
    lcd.setCursor(6, 1);
    lcd.print(minuto);
  }
  else {
    lcd.setCursor(5, 1);
    lcd.print(minuto);
  }
}

void print_desliga_lcd(int minuto, int hora) {
  lcd.setCursor(8, 1);
  lcd.print("D=");

  if (hora < 10) {
    lcd.setCursor(10, 1);
    lcd.print(0);
    lcd.setCursor(11, 1);
    lcd.print(hora);
  }
  else {
    lcd.setCursor(10, 1);
    lcd.print(hora);
  }

  lcd.print(":");

  if (minuto < 10) {
    lcd.setCursor(13, 1);
    lcd.print(0);
    lcd.setCursor(14, 1);
    lcd.print(minuto);
  }
  else {
    lcd.setCursor(13, 1);
    lcd.print(minuto);
  }
}

void print_pot_led(long int val) {
  val = val * 100 / 1023;
  lcd.setCursor(12, 0);
  lcd.print(val);
  lcd.print("%  ");
}

void pisca_mode(int pos) {
  static unsigned int i = 0;
#define count_total 60
#define count_aceso 35

  switch (pos) {
    case 1:       // mode_state = 1: ajuste hora relógio
      if (i == count_aceso) {
        lcd.setCursor(0, 0);
        lcd.print("  ");
      }
      if (i == count_total) {
        print_horario_lcd(relogio[0], relogio[1], relogio[2]);
      }
      break;

    case 2:       // mode_state = 2: ajuste minuto relógio
      if (i == count_aceso) {
        lcd.setCursor(3, 0);
        lcd.print("  ");
      }
      if (i == count_total) {
        print_horario_lcd(relogio[0], relogio[1], relogio[2]);
      }
      break;

    case 3:       // mode_state = 3: zera segundos relógio
      if (i == count_aceso) {
        lcd.setCursor(6, 0);
        lcd.print("  ");
      }
      if (i == count_total) {
        print_horario_lcd(relogio[0], relogio[1], relogio[2]);
      }
      break;

    case 4:       // mode_state = 3: ajuste hora liga
      if (i == count_aceso) {
        lcd.setCursor(2, 1);
        lcd.print("  ");
      }
      if (i == count_total) {
        print_liga_lcd(liga[0], liga[1]);
      }
      break;

    case 5:        // mode_state = 4: ajuste minuto liga
      if (i == count_aceso) {
        lcd.setCursor(5, 1);
        lcd.print("  ");
      }
      if (i == count_total) {
        print_liga_lcd(liga[0], liga[1]);
      }
      break;

    case 6:         // mode_state = 5: ajuste hora desliga
      if (i == count_aceso) {
        lcd.setCursor(10, 1);
        lcd.print("  ");
      }
      if (i == count_total) {
        print_desliga_lcd(desliga[0], desliga[1]);
      }
      break;
    case 7:       // mode_state = 6: ajuste minuto desliga
      if (i == count_aceso) {
        lcd.setCursor(13, 1);
        lcd.print("  ");
      }
      if (i == count_total) {
        print_desliga_lcd(desliga[0], desliga[1]);
      }
      break;
  }
  if (i == count_total) {
    i = 0;
  }
  i++;
  //  Serial.println(i);
}

byte ConverteParaBCD(byte val)
{
  //Converte o número de decimal para BCD
  return ( (val / 10 * 16) + (val % 10) );
}

byte ConverteparaDecimal(byte val)
{
  //Converte de BCD para decimal
  return ( (val / 16 * 10) + (val % 16) );
}
/////////////////////////////////////////
// mode_state = 1: ajuste hora relógio
// mode_state = 2: ajuste minuto relógio
// mode_state = 3: zera segundos relógio
// mode_state = 3: ajuste hora liga
// mode_state = 4: ajuste minuto liga
// mode_state = 5: ajuste hora desliga
// mode_state = 6: ajuste minuto desliga
/////////////////////////////////////////
