/*
 * ============================================================
 * ALARMA ANTIRROBO — Arduino UNO + HC-05
 * ============================================================
 *
 * Sketch para Arduino IDE (Arduino Sketch).
 *
 * Cómo subirlo:
 *   1. Abrir esta carpeta: Archivo > Abrir > alarma_uno.ino
 *   2. Herramientas > Placa > Arduino Uno
 *   3. Herramientas > Puerto > el del UNO (COMx o /dev/ttyACM0)
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
 *   EVENTO PIR | EVENTO PUERTA
 *   ALERTA INTRUSION
 *   ERR YA_ARMADA | ERR YA_DESARMADA | ERR COMANDO
 *
 * ESTADOS:
 *   DESARMADA -> ARMADA   (comando A)
 *   ARMADA    -> ALERTA   (PIR o puerta)
 *   ARMADA    -> DESARMADA (comando D)
 *   ALERTA    -> DESARMADA (comando D)
 *
 * FUNCIONES DE ESTE ARCHIVO:
 *   setup()            — arranque: pines, Serial y mensaje de listo
 *   loop()             — ciclo infinito: comandos, sensores, buzzer
 *   enviar()           — escribe el mismo texto por USB y Bluetooth
 *   enviarEstado()     — manda ESTADO DESARMADA / ARMADA / ALERTA
 *   procesarComando()  — interpreta A, D o E
 *   armar()            — pasa a ARMADA si estaba DESARMADA
 *   desarmar()         — vuelve a DESARMADA desde ARMADA o ALERTA
 *   consultarEstado()  — responde el estado actual (comando E)
 *   revisarSensores()  — lee PIR y puerta; alerta solo si está ARMADA
 *   activarAlerta()    — pasa a ALERTA, LED rojo y mensajes
 *   actualizarLeds()   — un LED encendido según el estado
 *   silenciarBuzzer()  — apaga el pitido
 *   parpadearBuzzer()  — pita cada 200 ms sin usar delay()
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
 * ARDUINO UNO           SENSORES
 * ─────────────         ────────
 * Pin 2          ------> OUT del PIR (VCC 5V, GND)
 * Pin 3          ------> Reed switch / contacto de puerta
 *                  el otro extremo del reed a GND (pull-up interno)
 * Pin 8          ------> Buzzer activo (+)
 * GND            ------> Buzzer (-)
 *
 * Puerta CERRADA = imán junto al reed = pin 3 en LOW
 * Puerta ABIERTA = imán lejos        = pin 3 en HIGH
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
const int PIN_PIR      = 2;   // movimiento
const int PIN_PUERTA   = 3;   // reed switch (HIGH = abierta)
const int PIN_BUZZER   = 8;   // pitido de alerta

// Segundo puerto serie en pines 10 (RX) y 9 (TX) para el HC-05.
SoftwareSerial bluetooth(10, 9);


// ============================================================
// ESTADOS
// ============================================================

enum EstadoAlarma {
  DESARMADA,  // LED verde; sensores no disparan alerta
  ARMADA,     // LED amarillo; PIR o puerta pasan a ALERTA
  ALERTA      // LED rojo + buzzer; solo se sale con D
};

EstadoAlarma estado = DESARMADA;


// ============================================================
// SENSORES Y TEMPORIZADORES
// ============================================================

bool pirAnterior = LOW;              // valor del PIR en el loop anterior
bool puertaAnteriorCerrada = true;   // true si la puerta estaba cerrada
unsigned long tiempoArranque = 0;    // millis() del encendido
unsigned long ultimoBeep = 0;        // última vez que se invirtió el buzzer
bool buzzerEncendido = false;

const unsigned long CALENTAMIENTO_PIR_MS = 3000;  // ignora el PIR 3 s al boot
const unsigned long PERIODO_BEEP_MS = 200;        // intervalo del pitido


// ============================================================
// PROTOTIPOS (Arduino necesita ver los nombres antes de setup/loop)
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


// ============================================================
// setup()
// Se ejecuta UNA vez al encender o resetear la placa.
// Deja pines, comunicaciones y estado inicial listos.
// ============================================================

void setup() {
  // Salidas: LEDs y buzzer. El Arduino pone 5V (HIGH) o 0V (LOW).
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  // Entradas: el PIR entrega HIGH/LOW. La puerta usa pull-up interno
  // (resistencia a 5V). Reed cerrado a GND = LOW = puerta cerrada.
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_PUERTA, INPUT_PULLUP);

  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_VERDE, LOW);
  silenciarBuzzer();

  Serial.begin(9600);      // USB / Monitor Serie
  bluetooth.begin(9600);   // HC-05

  // Guarda el instante de arranque y el valor actual de los sensores
  // para no tomar un flanco falso en el primer loop().
  tiempoArranque = millis();
  pirAnterior = digitalRead(PIN_PIR);
  puertaAnteriorCerrada = (digitalRead(PIN_PUERTA) == LOW);

  actualizarLeds();            // LED verde (DESARMADA)
  enviar("ALARMA UNO LISTA");  // aviso de que el firmware arrancó
  enviarEstado();              // ESTADO DESARMADA
}


// ============================================================
// loop()
// Se ejecuta en círculo para siempre. No usa delay() para no
// “congelar” la lectura de comandos A/D/E.
// Orden: 1) USB  2) Bluetooth  3) sensores  4) buzzer si hay alerta.
// ============================================================

void loop() {
  // USB: si hay un carácter, se lo pasa a procesarComando().
  // Se ignoran salto de línea y espacio que manda el Monitor Serie.
  if (Serial.available()) {
    char comando = Serial.read();
    if (comando != '\r' && comando != '\n' && comando != ' ') {
      procesarComando(comando);
    }
  }

  // Bluetooth: mismos comandos A/D/E, misma función.
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


// ============================================================
// enviar(mensaje)
// Manda UNA línea de texto por USB y por Bluetooth a la vez.
// Todas las respuestas del UNO pasan por acá.
// ============================================================

void enviar(const char* mensaje) {
  Serial.println(mensaje);
  bluetooth.println(mensaje);
}


// ============================================================
// enviarEstado()
// Traduce el enum interno a la línea de texto del protocolo.
// ============================================================

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


// ============================================================
// procesarComando(caracter)
// Recibe una letra (USB o Bluetooth), la pasa a mayúscula y
// llama a armar, desarmar o consultar. Cualquier otra letra
// responde ERR COMANDO.
// ============================================================

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


// ============================================================
// armar()
// Comando A. Solo vale desde DESARMADA.
// Si ya está ARMADA o ALERTA, responde ERR YA_ARMADA y no cambia nada.
// ============================================================

void armar() {
  if (estado != DESARMADA) {
    enviar("ERR YA_ARMADA");
    return;
  }

  estado = ARMADA;
  silenciarBuzzer();
  actualizarLeds();       // LED amarillo
  enviar("OK ARMAR");
  enviarEstado();         // ESTADO ARMADA
}


// ============================================================
// desarmar()
// Comando D. Vuelve a DESARMADA desde ARMADA o ALERTA.
// Apaga buzzer y LED rojo. Si ya estaba desarmada: ERR YA_DESARMADA.
// ============================================================

void desarmar() {
  if (estado == DESARMADA) {
    enviar("ERR YA_DESARMADA");
    return;
  }

  estado = DESARMADA;
  silenciarBuzzer();
  actualizarLeds();       // LED verde
  enviar("OK DESARMAR");
  enviarEstado();         // ESTADO DESARMADA
}


// ============================================================
// consultarEstado()
// Comando E. No cambia nada: solo informa el estado actual.
// ============================================================

void consultarEstado() {
  enviarEstado();
}


// ============================================================
// revisarSensores()
// Lee PIR (pin 2) y puerta (pin 3) en cada loop.
// Guarda el valor anterior para detectar FLANCO (el momento del
// cambio), no el nivel sostenido. Así el PIR en HIGH varios
// segundos no dispara la alerta una y otra vez.
// Solo llama a activarAlerta() si el estado es ARMADA.
// Desarmada o ya en alerta: lee igual (para no perder el flanco)
// pero no dispara.
// ============================================================

void revisarSensores() {
  bool pirAhora = digitalRead(PIN_PIR) == HIGH;       // HIGH = movimiento
  bool puertaAbierta = digitalRead(PIN_PUERTA) == HIGH;  // HIGH = abierta

  bool pirListo = (millis() - tiempoArranque) >= CALENTAMIENTO_PIR_MS;
  bool flancoPir = pirListo && pirAhora && !pirAnterior;           // LOW -> HIGH
  bool flancoPuerta = puertaAbierta && puertaAnteriorCerrada;      // cerrada -> abierta

  pirAnterior = pirAhora;
  puertaAnteriorCerrada = !puertaAbierta;

  if (estado != ARMADA) {
    return;
  }

  if (flancoPir) {
    activarAlerta("EVENTO PIR");
    return;
  }

  if (flancoPuerta) {
    activarAlerta("EVENTO PUERTA");
  }
}


// ============================================================
// activarAlerta(evento)
// Pasa ARMADA -> ALERTA. Enciende LED rojo y avisa:
// EVENTO PIR o EVENTO PUERTA, luego ALERTA INTRUSION y ESTADO ALERTA.
// El buzzer lo anima loop() con parpadearBuzzer().
// ============================================================

void activarAlerta(const char* evento) {
  estado = ALERTA;
  actualizarLeds();
  enviar(evento);
  enviar("ALERTA INTRUSION");
  enviarEstado();
}


// ============================================================
// actualizarLeds()
// Un solo LED encendido: verde / amarillo / rojo según estado.
// ============================================================

void actualizarLeds() {
  digitalWrite(LED_VERDE, estado == DESARMADA ? HIGH : LOW);
  digitalWrite(LED_AMARILLO, estado == ARMADA ? HIGH : LOW);
  digitalWrite(LED_ROJO, estado == ALERTA ? HIGH : LOW);
}


// ============================================================
// silenciarBuzzer()
// Apaga el pin 8. Se llama al armar, al desarmar y en setup().
// ============================================================

void silenciarBuzzer() {
  buzzerEncendido = false;
  digitalWrite(PIN_BUZZER, LOW);
}


// ============================================================
// parpadearBuzzer()
// Solo corre desde loop() cuando estado == ALERTA.
// Cada 200 ms invierte el buzzer (on/off) usando millis().
// No usa delay(): el loop sigue escuchando el comando D.
// ============================================================

void parpadearBuzzer() {
  unsigned long ahora = millis();
  if (ahora - ultimoBeep < PERIODO_BEEP_MS) {
    return;
  }

  ultimoBeep = ahora;
  buzzerEncendido = !buzzerEncendido;
  digitalWrite(PIN_BUZZER, buzzerEncendido ? HIGH : LOW);
}
