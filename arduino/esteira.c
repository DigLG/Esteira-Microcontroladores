/* mudando um pouco a ideia:
1. Sensor ótico 1 (Parada da Câmera): Quando a caixa interromper o feixe desse sensor, o Arduino freia o motor imediatamente. 
COLOCAR O SENSOR DEPOIS DO MEIO DA CÂMERA, EXEMPLO: Suponha que sua caixa tenha 10 cm de comprimento e o QR Code esteja no meio dela
(a 5 cm da borda). Se você instalar o sensor 5 cm depois do centro da câmera, quando a borda dianteira da caixa encostar no sensor e
parar a esteira, o centro da caixa (e o QR Code!) estará exatamente debaixo da lente da câmera.
2. Câmera: Fica apontada para o local exato onde a caixa parou. Assim que lê o QR Code, envia um ok pelo cabo USB para o Arduino.
3. Sensor optico 2 (Parada de Retirada): Fixe mais à frente, no final da esteira. Quando a caixa chega nele, o Arduino freia o motor pela segunda vez.
Assim que a caixa sai, o sensor fica livre e a esteira volta a girar sozinha.
*/
/* Teoria 9*/
#include <AFMotor.h>
AF_DCMotor motorA(1);

int sensorOtico1 = A11;
int sensorOtico2 = A12;

// Variável para o Arduino saber qual parada é a próxima:
// 1 = Indo para a câmera | 2 = espera a câmera para poder ir para a próxima | 3 = indo até o sensor 2
int etapa = 1; 

void setup () {
  motorA.setSpeed(255); // valor entre 0 e 255
  pinMode(sensorOtico1, INPUT);
  pinMode(sensorOtico2, INPUT);
  Serial.begin(9600);
}
void loop () {
  int leituraSensor1 = digitalRead(sensorOtico1);
  int leituraSensor2 = digitalRead(sensorOtico2);
  if (etapa == 1){
    if (leituraSensor1 == LOW) {
      motorA.run(FORWARD);
    }
    else {
      Serial.println("Caixa identificada!");
      motorA.run(RELEASE);
      //leitura do QRcode pela câmera
      etapa = 2;
    }
  }

  else if (etapa == 2){
    motorA.run(RELEASE); // só por segurança
    if (Serial.available() > 0) {
      String texto = Serial.readStringUntil('\n');
      texto.trim(); // remove quebra de linha
        if (texto == "QRcode lido!") {
          etapa= 3;
      }
    }
  }
  
  else if (etapa == 3){
    if (leituraSensor2 == LOW) {
      motorA.run(FORWARD); // Mesmo passando pelo Sensor 1, agora ele ignora e segue!
    }
    else { // Caixa chegou no Sensor 2!
      motorA.run(RELEASE);
      Serial.println("Caixa aguradando para ser retirada");
      //código da garra
      // Trava de segurança: garante que a esteira só volta a andar depois que a garra tirou a caixa do lugar
      while (digitalRead(sensorOtico2) == HIGH) {
        delay(50); // a cada 50 milisegundos verifica se a caixa ainda está na frente do sensor
      }
      delay(1000);
      Serial.println("Caixa retirada!");
      etapa = 1;
    }
  }
}
