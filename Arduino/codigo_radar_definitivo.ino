/* Codigo para o funcionamento do radar com os motores de passo
 NEMA 17 e um sensor LIDAR.
 O fuso utilizado eh um TR-8, com avanço de 0.04mm/passo
 O passo do motor eh de 1.8 grau (200passos = 1 volta completa)

 Para o funcionamento, posicione o objeto a ser analisado em um
 apoio sobre a mesa e posicione o radar mirando na mesa.
 O objeto deve ter dimensao maxima de 20cm.

 O motor x eh o que move o radar; o motor y eh o que move a mesa
*/

#include <SoftwareSerial.h>
#include "TFMini.h"

// Define as variaveis do fuso
float avanco = 0.04; // mm/passo

// Define o tamanho do corpo
int X_corpo = 180; // mm
int X_passo = X_corpo/avanco; // em passos
int Y_corpo = 120; // mm
int Y_passo = Y_corpo/avanco; // em passos

// Define as posicoes do corpo
float x = 0.00; // mm
float dx = 3.0; // intervalo entre medidas (mm)
int dx_passo = dx/avanco; // em passos
float y = 0.00; // mm
float dy = 4.0; // intervalo entre medidas (mm)
int dy_passo = dy/avanco; // em passos
int z = 0;

// Variaveis do radar
int dist_mesa = 0.0; // distancia entre a mesa e o radar
int str_mesa = 0.0; // forca do sinal dessa leitura da mesa
SoftwareSerial mySerial(50,51);
TFMini tfmini;

// Definindo os pinos de cada motor
const int step_x = 48;
const int dir_x = 49;
const int en_x = 52;
const int step_y = 40;
const int dir_y = 41;
const int en_y = 53;

void setup() {

  pinMode(en_x,OUTPUT);
  digitalWrite(en_x,HIGH);
  pinMode(en_y,OUTPUT);
  digitalWrite(en_y,HIGH);
  
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Preparando os motores e o radar...");
  
  Serial.println();
  mySerial.begin(TFMINI_BAUDRATE);
  tfmini.begin(&mySerial);

  // Iniciar o radar
  Serial.println("Podemos medir a distancia ate a mesa?");
  // a pergunta acima DEVE ser respondida apenas com s
  while (Serial.available()==0);{
  }
  delay(1000);
  tfmini.getDistance();
  tfmini.getRecentSignalStrength();
  delay(250);
  tfmini.getDistance();
  tfmini.getRecentSignalStrength();
  delay(250);
  tfmini.getDistance();
  tfmini.getRecentSignalStrength();
  delay(250);
  dist_mesa = tfmini.getDistance();
  str_mesa = tfmini.getRecentSignalStrength();
  Serial.println("Distancia ate a mesa: ");
  Serial.println(dist_mesa);
  Serial.println("Forca do sinal: ");
  Serial.println(str_mesa);
  Serial.println();  

  // Inicia os motores
  pinMode(step_x,OUTPUT);
  pinMode(dir_x,OUTPUT);
  pinMode(step_y,OUTPUT);
  pinMode(dir_y,OUTPUT);

  Serial.println("Podemos iniciar a varredura?");
  while (Serial.available()==2); {
  }
  Serial.println("Iniciando varredura de radar");
  Serial.println("x,y,z,str"); // formato da saia

  digitalWrite(dir_y,LOW);

  digitalWrite(en_x,LOW);
  digitalWrite(en_y,LOW);

}

void loop() {
  
  digitalWrite(dir_x,LOW); // sentido de avanço
  for(int i=0; i < dx_passo; i++) { // andar um dx
    digitalWrite(step_x,HIGH);
    delayMicroseconds(500);
    digitalWrite(step_x,LOW);
    delayMicroseconds(500);
  }
  delay(500);
  // faz a medicao da distancia
  x = x+dx;
  
  // precisamos descartar uma medicao
  tfmini.getDistance();
  tfmini.getRecentSignalStrength();
  delay(250);
  // mede o valor definitivo
  uint16_t dist = tfmini.getDistance();
  uint16_t strength = tfmini.getRecentSignalStrength();
  z = dist_mesa-dist;
  if (abs(z) > 1){ // imprime os valores
  Serial.print(x);
  Serial.print(',');
  Serial.print(y);
  Serial.print(',');
  Serial.print(z);
  Serial.print(',');
  Serial.println(strength);
  }
  delay(500);

  if (x >= X_corpo) {
    digitalWrite(dir_x,HIGH); // inverte a direcao de x
    for (int i=0; i < X_passo; i++) { // retorna ao inicio
      digitalWrite(step_x,HIGH);
      delayMicroseconds(500);
      digitalWrite(step_x,LOW);
      delayMicroseconds(500);
    }
    x = 0;
    delay(500);
    for (int j=0; j < dy_passo; j++) { // anda dy
      digitalWrite(step_y,HIGH);
      delayMicroseconds(500);
      digitalWrite(step_y,LOW);
      delayMicroseconds(500);
    }
    y = y + dy;    
  }
  delay(500);
  if ((y >= Y_corpo) && (x >= X_corpo)) { // indica o fim da varredura
    Serial.println("Fim da varredura de radar...");
    while (1){
      Serial.println("Pode desconectar os motores");
      delay(60000);
    }
  }
}
