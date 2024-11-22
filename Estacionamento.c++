#include <LiquidCrystal.h>
#include <Servo.h>

// Definição dos pinos
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Configuração do Servo
Servo servo;
int servoPin = 13;

// Definição dos pinos dos sensores
const int trigPinEntrada = 12;          // Sensor de entrada (Trig)
const int echoPinEntrada = 11; 	        // Sensor de entrada (Echo)
const int trigPinSaida = 10;            // Sensor de saída (Trig)
const int echoPinSaida = 9;    	        // Sensor de saída (Echo)

// Definição dos pinos dos LEDs
const int ledEntrada = A2; 		        // LED verde (entrada)
const int ledSaida = A1;   		        // LED vermelho (saída)

// Variáveis de controle
const int capacidadeEstacionamento = 5;
int vagasDisponiveis = capacidadeEstacionamento;

void setup() {
  // Inicializa o LCD
  lcd.begin(16, 2);
  lcd.print("Estacionamento");
  delay(2000);
  lcd.clear();

  // Inicializa o servo
  servo.attach(servoPin);
  servo.write(0); // Barreira fechada

  // Configura pinos
  pinMode(trigPinEntrada, OUTPUT);
  pinMode(echoPinEntrada, INPUT);
  pinMode(trigPinSaida, OUTPUT);
  pinMode(echoPinSaida, INPUT);
  pinMode(ledEntrada, OUTPUT);
  pinMode(ledSaida, OUTPUT);

  // Exibe o estado inicial
  atualizarDisplay();
}

void loop() {
  // Medir a distância de entrada e saída
  long distanciaEntrada = medirDistancia(trigPinEntrada, echoPinEntrada);
  long distanciaSaida = medirDistancia(trigPinSaida, echoPinSaida);

  // Verifica a presença de um veículo na entrada (sensor de entrada)
  if (distanciaEntrada < 70 && distanciaEntrada > 0) { // Se a distância for inferior a 70cm, há um carro na entrada
    if (vagasDisponiveis > 0) {         // Se houver vagas disponíveis
      digitalWrite(ledEntrada, HIGH);   // Acende LED verde (entrada)
      digitalWrite(ledSaida, LOW);      // Apaga LED vermelho (saída)
      abrirBarreira();
      vagasDisponiveis--;  
      atualizarDisplay();               // Atualiza o display após a mudança nas vagas
      delay(5000);                      // Espera 5 segundos enquanto a barreira está aberta
      fecharBarreira();
    } else {                            // Se o estacionamento estiver lotado
      digitalWrite(ledEntrada, LOW);    // Apaga LED verde (entrada)
      digitalWrite(ledSaida, HIGH);     // Acende LED vermelho (saída)
      delay(2000);                      // Aguarda um tempo antes de verificar novamente
    }
  }

  // Verifica a presença de um veículo na saída (sensor de saída)
  if (distanciaSaida < 70 && distanciaSaida > 0) { // Se a distância for inferior a 10cm, há um carro saindo
    if (vagasDisponiveis < capacidadeEstacionamento) { // Se não estiver lotado
      digitalWrite(ledSaida, LOW);      // Apaga LED vermelho (saída)
      vagasDisponiveis++;  
      atualizarDisplay();               // Atualiza o display após a mudança nas vagas
      delay(2000);                      // Espera 2 segundos enquanto a barreira está aberta
    }
  }

  // Caso nenhum veículo seja detectado
  else {
    digitalWrite(ledEntrada, LOW);      // Apaga LED verde (entrada)
    digitalWrite(ledSaida, LOW);        // Apaga LED vermelho (saída)
  }

  delay(1000);                          // Espera um pouco antes de medir novamente
}

long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Calcula a duração do eco
  long duracao = pulseIn(echoPin, HIGH);

  // Converte distância em centímetros
  long distancia = duracao * 0.034 / 2;

  // Verifica se a leitura da distância é válida
  if (distancia <= 0 || distancia > 400) {
    distancia = 999;                    // Se a distância for inválida, define um valor alto
  }

  return distancia;
}

void abrirBarreira() {
  servo.write(90);                      // Abre a barreira
}

void fecharBarreira() {
  servo.write(0);                       // Fecha a barreira
}

void atualizarDisplay() {
  lcd.clear();                          // Limpa o display antes de escrever
  lcd.print("Vagas: ");
  lcd.print(vagasDisponiveis);          // Exibe o número de vagas restantes

  lcd.setCursor(0, 1);                  // Move o cursor para a segunda linha
  if (vagasDisponiveis > 0) {
    lcd.print("Entrada liberada");      // Exibe "Entrada liberada"
  } else {
    lcd.print("!Lotado!");              // Exibe "Lotado"
  }
}
