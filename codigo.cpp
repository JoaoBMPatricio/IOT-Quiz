#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
 
const int botaoVermelho = 2;
const int botaoAmarelo  = 3;
const int botaoAzul     = 7;
 
const int ledVermelho = 4;
const int ledAmarelo  = 5;
const int buzzer      = 6;
const int ledAzul     = 8;
 
int acertos = 0;
bool nivelMaximoMostrado = false;
 
int respostaCorreta = 0;
int opcaoVermelho = 0;
int opcaoAmarelo = 0;
int opcaoAzul = 0;
 
int n1, n2, resultado;
char simbolo;
 
unsigned long inicioPergunta;
const unsigned long TEMPO_LIMITE = 10000;
 
void desligarLeds() {
  digitalWrite(ledVermelho, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledAzul, LOW);
}
 
void ligarTodosLeds() {
  digitalWrite(ledVermelho, HIGH);
  digitalWrite(ledAmarelo, HIGH);
  digitalWrite(ledAzul, HIGH);
}
 
void mostrarNivelMaximo() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("NIVEL MAXIMO!");
 
  for(int i=0;i<8;i++){
    ligarTodosLeds();
    tone(buzzer,1500);
    delay(120);
    desligarLeds();
    noTone(buzzer);
    delay(120);
  }
}
 
void gerarPergunta() {
 
  lcd.clear();
  oled.clearDisplay();
 
  int fase;
 
  if(acertos <= 3) fase = 0;
  else if(acertos <= 6) fase = 1;
  else if(acertos <= 9) fase = 2;
  else fase = 3;
 
  if(fase == 0){
    n1 = random(1,21);
    n2 = random(1,21);
  }
  else if(fase == 1){
    n1 = random(1,51);
    n2 = random(1,51);
  }
  else{
    n1 = random(1,100);
    n2 = random(1,100);
  }
 
  int operacao;
 
  if(fase == 0) operacao = random(0,2);      // + -
  else if(fase < 3) operacao = random(0,3);  // + - *
  else operacao = random(0,4);               // + - * /
 
  switch(operacao){
 
    case 0:
      simbolo = '+';
      resultado = n1 + n2;
      break;
 
    case 1:
      simbolo = '-';
      if(n2 > n1){
        int t=n1; n1=n2; n2=t;
      }
      resultado = n1 - n2;
      break;
 
    case 2:
      simbolo='*';
      n1 = random(1,16);
      n2 = random(1,16);
      resultado = n1*n2;
      break;
 
    case 3:
      simbolo='/';
      n2 = random(1,13);
      resultado = random(1,21);
      n1 = resultado * n2;
      break;
  }
 
  int errada1, errada2;
 
  do {
    errada1 = resultado + random(-10,11);
  } while(errada1 == resultado || errada1 < 0);
 
  do {
    errada2 = resultado + random(-10,11);
  } while(errada2 == resultado || errada2 == errada1 || errada2 < 0);
 
  respostaCorreta = random(1,4);
 
  switch(respostaCorreta){
    case 1:
      opcaoVermelho = resultado;
      opcaoAmarelo = errada1;
      opcaoAzul = errada2;
      break;
 
    case 2:
      opcaoVermelho = errada1;
      opcaoAmarelo = resultado;
      opcaoAzul = errada2;
      break;
 
    case 3:
      opcaoVermelho = errada1;
      opcaoAmarelo = errada2;
      opcaoAzul = resultado;
      break;
  }
 
  lcd.setCursor(0,0);
  lcd.print(n1);
  lcd.print(" ");
  lcd.print(simbolo);
  lcd.print(" ");
  lcd.print(n2);
  lcd.print("=?");
 
  lcd.setCursor(0,1);
  lcd.print("Acertos:");
  lcd.print(acertos);
 
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
 
  oled.setCursor(0,0);
  oled.print("VERM:");
  oled.println(opcaoVermelho);
 
  oled.setCursor(0,22);
  oled.print("AMAR:");
  oled.println(opcaoAmarelo);
 
  oled.setCursor(0,44);
  oled.print("AZUL:");
  oled.println(opcaoAzul);
 
  oled.display();
 
  ligarTodosLeds();
 
  inicioPergunta = millis();
}
 
void desenharBarraTempo(){
 
  int largura = map(
    millis() - inicioPergunta,
    0,
    TEMPO_LIMITE,
    128,
    0
  );
 
  if(largura < 0) largura = 0;
 
  oled.fillRect(0,60,128,4,BLACK);
  oled.fillRect(0,60,largura,4,WHITE);
  oled.display();
}
 
void tempoEsgotado(){
 
  acertos = 0;
 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TEMPO ESGOTOU");
 
  desligarLeds();
 
  for(int i=0;i<3;i++){
    tone(buzzer,300);
    delay(250);
    noTone(buzzer);
    delay(150);
  }
 
  delay(1500);
  gerarPergunta();
}
 
void verificarResposta(int botao){
 
  desligarLeds();
 
  int ledSelecionado;
 
  if(botao == 1) ledSelecionado = ledVermelho;
  else if(botao == 2) ledSelecionado = ledAmarelo;
  else ledSelecionado = ledAzul;
 
  for(int i=0;i<5;i++){
    digitalWrite(ledSelecionado,HIGH);
    tone(buzzer,700);
    delay(100);
    digitalWrite(ledSelecionado,LOW);
    noTone(buzzer);
    delay(100);
  }
 
  lcd.clear();
 
  if(botao == respostaCorreta){
 
    acertos++;
 
    lcd.setCursor(0,0);
    lcd.print("CORRETO!");
 
    lcd.setCursor(0,1);
    lcd.print("ACERTOS:");
    lcd.print(acertos);
 
    for(int i=0;i<7;i++){
      tone(buzzer,1800);
      delay(60);
      noTone(buzzer);
      delay(60);
    }
 
    if(acertos >= 10 && !nivelMaximoMostrado){
      nivelMaximoMostrado = true;
      mostrarNivelMaximo();
    }
 
  } else {
 
    acertos = 0;
 
    lcd.setCursor(0,0);
    lcd.print("ERRADO!");
 
    lcd.setCursor(0,1);
    lcd.print("ZEROU SCORE");
 
    tone(buzzer,300);
    delay(500);
    noTone(buzzer);
  }
 
  desligarLeds();
 
  if(respostaCorreta == 1) digitalWrite(ledVermelho,HIGH);
  if(respostaCorreta == 2) digitalWrite(ledAmarelo,HIGH);
  if(respostaCorreta == 3) digitalWrite(ledAzul,HIGH);
 
  delay(2000);
 
  gerarPergunta();
}
 
void setup() {
 
  lcd.init();
  lcd.backlight();
 
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 
  pinMode(botaoVermelho, INPUT_PULLUP);
  pinMode(botaoAmarelo, INPUT_PULLUP);
  pinMode(botaoAzul, INPUT_PULLUP);
 
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledAzul, OUTPUT);
 
  pinMode(buzzer, OUTPUT);
 
  randomSeed(analogRead(A0));
 
  gerarPergunta();
}
 
void loop() {
 
  desenharBarraTempo();
 
  if(millis() - inicioPergunta >= TEMPO_LIMITE){
    tempoEsgotado();
    return;
  }
 
  if(digitalRead(botaoVermelho) == LOW){
    verificarResposta(1);
    while(digitalRead(botaoVermelho)==LOW);
  }
 
  if(digitalRead(botaoAmarelo) == LOW){
    verificarResposta(2);
    while(digitalRead(botaoAmarelo)==LOW);
  }
 
  if(digitalRead(botaoAzul) == LOW){
    verificarResposta(3);
    while(digitalRead(botaoAzul)==LOW);
  }
}
