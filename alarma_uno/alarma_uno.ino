/*
 * ============================================================
 * ALARMA ANTIRROBO — Arduino UNO + HC-05
 * ============================================================
 *
 * Sketch para Arduino IDE (Arduino Sketch).
 *
 * El PIR se reemplazó por un ultrasónico HC-SR04 (no había
 * factibilidad de conseguir el PIR). Sigue mandando EVENTO PIR
 * en el protocolo, para no romper el puente USB con la app.
 *
 * Cómo subirlo:
 *   1. Abrir esta carpeta: Archivo > Abrir > alarma_uno.ino
 *   2. Herramientas > Placa → Arduino Uno
 *   3. Herramientas → Puerto → el del UNO (COMx o /dev/ttyACM0)
 *   4. Monitor Serie a 9600 baud (NL y CR, o "Ambos NL y CR")
 *   5. Subir
 *
 * COMANDOS (un carácter, USB o Bluetooth):
 *   A  -> Armar
 *   D  -> Desarmar
 *   E  -> Consultar estado
 *
 * RESPUESTAS:
 *   ALARMA UNO LISTA
 *   ESTADO DESARMADA | ESTADO ARMADA | ESTADO ALERTA
 *   OK ARMAR | OK DESARMAR
 *   EVENTO PIR
 *   ALERTA INTRUSION
 *   ERR YA_ARMADA | ERR YA_DESARMADA | ERR COMANDO
 *
 * ESTADOS:
 *   DESARMADA -> ARMADA   (comando A)
 *   ARMADA    -> ALERTA   (HC-SR04 a menos de 20 cm)
 *   ARMADA    -> DESARMADA (comando D)
 *   ALERTA    -> DESARMADA (comando D)
 *
 * ============================================================
 * CONEXIONES
 * ============================================================
 *
 * ARDUINO UNO           HC-05 BLUETOOTH
 * ─────────────         ──────────────
 * Pin 10 (RX)    ------> TX
 * Pin 9  (TX)    ------> RX
 * 5V             ------> VCC
 * GND            ------> GND
 *
 * ARDUINO UNO           LEDS (mismo esquema del semáforo)
 * ─────────────         ────
 * Pin 13         ------> LED ROJO     (alerta)
 * Pin 12         ------> LED AMARILLO (armada)
 * Pin 11         ------> LED VERDE    (desarmada)
 * GND            ------> Cátodo de los 3 LEDs (+ 220 ohm en cada ánodo)
 *
 * ARDUINO UNO           SENSOR Y BUZZER  (igual que Tinkercad)
 * ─────────────         ────────────────────────────────────
 * Pin 7 (TRIG)   ------> TRIG del HC-SR04
 * Pin 6 (ECHO)   ------> ECHO del HC-SR04
 * 5V             ------> VCC del HC-SR04
 * GND            ------> GND del HC-SR04
 * Pin 8          ------> Buzzer / piezo (+)
 * GND            ------> Buzzer (-)
 *
 * No hay reed switch ni sensor de puerta en esta maqueta.
 * Ultrasónico: objeto a menos de 20 cm = intrusión (reemplazo del PIR).
 *
 * ============================================================
 */

#include <SoftwareSerial.h>


// ============================================================
// PINES
// ============================================================

const int LED_ROJO     = 13;  // alerta
const int LED_AMARILLO = 12;  // armada
const int LED_VERDE    = 11;  // desarmada
const int PIN_TRIG     = 7;   // HC-SR04: dispara el pulso (reemplaza al PIR)
const int PIN_ECHO     = 6;   // HC-SR04: recibe el eco
const int PIN_BUZZER   = 8;   // pitido de alerta

SoftwareSerial bluetooth(10, 9);  // (RX, TX) cruzado con el HC-05

const int DISTANCIA_ALERTA_CM = 20;               // umbral de “objeto cerca”
const unsigned long INTERVALO_MEDICION_MS = 200;  // no medir en cada loop
const unsigned long PERIODO_BEEP_MS = 200;


// ============================================================
// ESTADOS
// ============================================================

enum EstadoAlarma {
  DESARMADA,  // LED verde; sensores no disparan alerta
  ARMADA,     // LED amarillo; HC-SR04 < 20 cm pasa a ALERTA
  ALERTA      // LED rojo + buzzer; solo se sale con D
};

EstadoAlarma estado = DESARMADA;


// ============================================================
// SENSORES Y TEMPORIZADORES
// ============================================================

bool objetoCercanoAnterior = false;  // true si el HC-SR04 ya veía < 20 cm
unsigned long ultimaMedicion = 0;
unsigned long ultimoBeep = 0;
bool buzzerEncendido = false;


// ============================================================
// PROTOTIPOS
// ============================================================

void enviar(const char* mensaje);
void actualizarLeds();
void silenciarBuzzer();
void parpadearBuzzer();
void procesarComando(char caracter);
void armar();
void desarmar();
void consultarEstado();
void enviarEstado();
void revisarSensores();
void activarAlerta(const char* evento);
long medirDistanciaCm();


// ============================================================
// setup()
// UNA vez al encender o resetear: pines, Serial y LED verde.
// ============================================================

void setup() {
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(PIN_TRIG, LOW);
  silenciarBuzzer();

  Serial.begin(9600);
  bluetooth.begin(9600);

  actualizarLeds();
  enviar("ALARMA UNO LISTA");
  enviarEstado();
}


// ============================================================
// loop()
// 1) USB  2) Bluetooth  3) sensores  4) buzzer si hay alerta.
// ============================================================

void loop() {
  if (Serial.available()) {
    char comando = Serial.read();
    if (comando != '\r' && comando != '\n' && comando != ' ') {
      procesarComando(comando);
    }
  }

  if (bluetooth.available()) {
    char comando = bluetooth.read();
    if (comando != '\r' && comando != '\n' && comando != ' ') {
      procesarComando(comando);
    }
  }

  revisarSensores();

  if (estado == ALERTA) {
    parpadearBuzzer();
  }
}


void enviar(const char* mensaje) {
  Serial.println(mensaje);
  bluetooth.println(mensaje);
}

void enviarEstado() {
  switch (estado) {
    case DESARMADA:
      enviar("ESTADO DESARMADA");
      break;
    case ARMADA:
      enviar("ESTADO ARMADA");
      break;
    case ALERTA:
      enviar("ESTADO ALERTA");
      break;
  }
}

void procesarComando(char caracter) {
  caracter = toupper(caracter);

  switch (caracter) {
    case 'A':
      armar();
      break;
    case 'D':
      desarmar();
      break;
    case 'E':
      consultarEstado();
      break;
    default:
      enviar("ERR COMANDO");
      break;
  }
}

void armar() {
  if (estado != DESARMADA) {
    enviar("ERR YA_ARMADA");
    return;
  }

  estado = ARMADA;
  silenciarBuzzer();
  actualizarLeds();
  enviar("OK ARMAR");
  enviarEstado();
}

void desarmar() {
  if (estado == DESARMADA) {
    enviar("ERR YA_DESARMADA");
    return;
  }

  estado = DESARMADA;
  silenciarBuzzer();
  actualizarLeds();
  enviar("OK DESARMAR");
  enviarEstado();
}

void consultarEstado() {
  enviarEstado();
}


// ============================================================
// medirDistanciaCm()
// Pulso en TRIG; ECHO permanece HIGH el tiempo de ida y vuelta.
// Distancia ≈ microsegundos / 58. Sin eco → -1.
// ============================================================

long medirDistanciaCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(20);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long tiempo = pulseIn(PIN_ECHO, HIGH, 25000UL);
  if (tiempo == 0) {
    return -1;
  }

  return tiempo / 58;
}


// ============================================================
// revisarSensores()
// Cada 200 ms lee el HC-SR04 (único sensor de esta maqueta).
// Flanco = acaba de pasar a menos de 20 cm.
// Solo alerta si está ARMADA. EVENTO PIR = sustituto del PIR.
// ============================================================

void revisarSensores() {
  unsigned long ahora = millis();
  if (ahora - ultimaMedicion < INTERVALO_MEDICION_MS) {
    return;
  }
  ultimaMedicion = ahora;

  long distancia = medirDistanciaCm();
  bool objetoCercano = (distancia > 0 && distancia < DISTANCIA_ALERTA_CM);
  bool flancoUltrasonico = objetoCercano && !objetoCercanoAnterior;
  objetoCercanoAnterior = objetoCercano;

  if (estado == ARMADA && flancoUltrasonico) {
    activarAlerta("EVENTO PIR");
  }
}

void activarAlerta(const char* evento) {
  estado = ALERTA;
  actualizarLeds();
  enviar(evento);
  enviar("ALERTA INTRUSION");
  enviarEstado();
}

void actualizarLeds() {
  digitalWrite(LED_VERDE, estado == DESARMADA ? HIGH : LOW);
  digitalWrite(LED_AMARILLO, estado == ARMADA ? HIGH : LOW);
  digitalWrite(LED_ROJO, estado == ALERTA ? HIGH : LOW);
}

void silenciarBuzzer() {
  buzzerEncendido = false;
  digitalWrite(PIN_BUZZER, LOW);
}

void parpadearBuzzer() {
  unsigned long ahora = millis();
  if (ahora - ultimoBeep < PERIODO_BEEP_MS) {
    return;
  }

  ultimoBeep = ahora;
  buzzerEncendido = !buzzerEncendido;
  digitalWrite(PIN_BUZZER, buzzerEncendido ? HIGH : LOW);
}
