#include <Ultrasonic.h>
//12V

#define pino_trigger 9
#define pino_echo 10

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

// Variável para o Arduino saber qual parada é a próxima:
// 1 = Indo para a câmera | 2 = espera a câmera para poder ir para a próxima | 3 = indo até o sensor 2
int etapa = 1; 

void setup () {
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  Serial.begin(9600);
  Serial.println("Lendo dados do sensor...");
}

void velocidade(){
  analogWrite(5, 550);
  analogWrite(6, 0);
}

void loop () {
  //Le as informacoes do sensor em cm
  float cmMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  //Exibe informacoes no serial monitor
  
  if (etapa == 1){
    if (cmMsec > 25) {
      velocidade();
      Serial.print("Distancia em cm: ");
      Serial.println(cmMsec);
      delay(400);
    }
    else {
      Serial.println("Caixa identificada!");
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      //leitura do QRcode pela câmera
      etapa = 2;
    }
  }

  else if (etapa == 2){
    digitalWrite(5, LOW);
    digitalWrite(6, LOW); // só por segurança
    if (Serial.available() > 0) {
      Serial.print("Distancia em cm: ");
      Serial.println(cmMsec);
      delay(400);
      String texto = Serial.readStringUntil('\n');
      texto.trim(); // remove quebra de linha
        if (texto == "ok") { //qrcode foi lido
          etapa= 3;
      }
    }
  }
  
  else if (etapa == 3){
    if (cmMsec > 15) {
      velocidade();
      Serial.print("Distancia em cm: ");
      Serial.println(cmMsec);
      delay(400);
    }
    else { // Caixa chegou no Sensor 2!
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      Serial.println("Caixa aguradando para ser retirada");
      //código da garra
      //Trava de segurança: garante que a esteira só volta a andar depois que a garra tirou a caixa do lugar
      while (cmMsec == 15) {
        delay(50); // a cada 50 milisegundos verifica se a caixa ainda está na frente do sensor
      }
      delay(1000);
      Serial.println("Caixa retirada!");
      etapa = 1;
    }
  }
}