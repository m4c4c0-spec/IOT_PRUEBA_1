/*
 * ============================================================
 * ALARMA — Ejemplo para Tinkercad (maqueta online)
 * ============================================================
 *
 * Este archivo es un SEGUNDO sketch, aparte de alarma_uno.ino.
 * Sirve para simular la alarma en Tinkercad Circuits, sin placa física.
 *
 * Tinkercad NO tiene módulo HC-05. El control es solo por
 * Monitor Serie (A armar, D desarmar, E estado).
 * Usa un ultrasónico HC-SR04 (igual que alarma_uno.ino, que
 * reemplazó al PIR). Si está ARMADA y algo se acerca a menos
 * de 20 cm, pasa a ALERTA.
 *
 * ------------------------------------------------------------
 * CÓMO ARMARLO EN TINKERCAD
 * ------------------------------------------------------------
 *  1. https://www.tinkercad.com → Circuits → Create new circuit
 *  2. Arrastrá: Arduino Uno, 3 LEDs, 3 resistencias 220 Ω,
 *     1 Piezo (buzzer), 1 Ultrasonic Distance Sensor
 *  3. Cableá como la tabla de abajo (colores sugeridos)
 *  4. Click en "Code" → Text (no Blocks)
 *  5. Borrá el código de ejemplo y pegá TODO este archivo
 *  6. Start Simulation
 *  7. Abrí Serial Monitor (abajo). Debe salir:
 *       ALARMA LISTA - ESTADO DESARMADA
 *     y el LED verde pin 11 encendido
 *  8. En el Serial Monitor escribí A y Enter → LED rojo (armada)
 *  9. Acercá un objeto al ultrasónico (o mové el slider del sensor)
 *     a menos de 20 cm → LED amarillo + pitido (alerta)
 * 10. Escribí D → se silencia y vuelve el verde
 *
 * ------------------------------------------------------------
 * CONEXIONES TINKERCAD
 * ------------------------------------------------------------
 *
 *  ARDUINO UNO              COMPONENTE
 *  ───────────              ──────────
 *  Pin 13  --[220 Ω]-->     LED ROJO     ánodo; cátodo a GND
 *  Pin 12  --[220 Ω]-->     LED AMARILLO ánodo; cátodo a GND
 *  Pin 11  --[220 Ω]-->     LED VERDE    ánodo; cátodo a GND
 *  Pin 8   ---------------> Piezo (+)    el otro pin a GND
 *  Pin 7   ---------------> TRIG del ultrasónico
 *  Pin 6   ---------------> ECHO del ultrasónico
 *  5V      ---------------> VCC del ultrasónico
 *  GND     ---------------> GND (LEDs, piezo, ultrasónico)
 *
 * Ultrasónico Tinkercad (4 pines, de izquierda a derecha):
 *   VCC | TRIG | ECHO | GND
 *
 * ------------------------------------------------------------
 * LEDs (este ejemplo Tinkercad)
 * ------------------------------------------------------------
 *   Verde     pin 11  = DESARMADA  (acaba de encender / comando D)
 *   Rojo      pin 13  = ARMADA     (comando A)
 *   Amarillo  pin 12  = ALERTA     (objeto a menos de 20 cm)
 *
 * Nota: en alarma_uno.ino el amarillo es armada y el rojo es alerta.
 * Acá se respeta el mapeo que pediste para la maqueta.
 *
 * COMANDOS (Serial Monitor, 9600 baud):
 *   A / a  armar
 *   D / d  desarmar (también silencia la alerta)
 *   E / e  consultar estado
 * ============================================================
 */

const int LED_ROJO     = 13;  // se enciende cuando está ARMADA
const int LED_AMARILLO = 12;  // se enciende cuando está en ALERTA
const int LED_VERDE    = 11;  // se enciende cuando está DESARMADA
const int BUZZER       = 8;   // piezo de Tinkercad (usa tone / noTone)
const int TRIG         = 7;   // dispara el pulso del HC-SR04
const int ECHO         = 6;   // recibe el eco; el ancho = distancia

const int DISTANCIA_ALERTA_CM = 20;  // umbral para pasar a ALERTA
const unsigned long INTERVALO_MEDICION_MS = 200;
const unsigned long INTERVALO_PITIDO_MS   = 200;
const unsigned int TONO_HZ = 1500;


// ------------------------------------------------------------
// ESTADOS — misma máquina que la alarma, más simple
// DESARMADA --A--> ARMADA --objeto cerca--> ALERTA --D--> DESARMADA
// ------------------------------------------------------------

enum Estado { DESARMADA, ARMADA, ALERTA };
Estado estado = DESARMADA;

unsigned long ultimaMedicion = 0;  // última vez que se midió distancia
unsigned long ultimoPitido   = 0;  // última vez que se invirtió el tono
bool pitidoEncendido         = false;


// ------------------------------------------------------------
// actualizarLeds()
// Prende UN LED según el estado. Los otros quedan en LOW (apagados)
// porque digitalWrite(..., false) vale 0.
// ------------------------------------------------------------

void actualizarLeds() {
  digitalWrite(LED_AMARILLO, estado == ALERTA);
  digitalWrite(LED_ROJO,     estado == ARMADA);
  digitalWrite(LED_VERDE,    estado == DESARMADA);
}


// ------------------------------------------------------------
// medirDistanciaCm()
// HC-SR04: un pulso corto en TRIG; ECHO se pone HIGH el tiempo
// que tarda el sonido en ir y volver. Distancia ≈ tiempo / 58.
// Si no hay eco (pulseIn da 0), devuelve -1 para no alertar.
// En Tinkercad: el slider del sensor cambia esta distancia.
// ------------------------------------------------------------

long medirDistanciaCm() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(20);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG, LOW);

  unsigned long tiempo = pulseIn(ECHO, HIGH, 25000UL);
  if (tiempo == 0) {
    return -1;
  }

  return tiempo / 58;
}


// ------------------------------------------------------------
// setup() — corre UNA vez al pulsar Start Simulation
// Configura pines, abre el Serial y enciende el LED verde.
// ------------------------------------------------------------

void setup() {
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Serial.begin(9600);
  actualizarLeds();
  Serial.println("ALARMA LISTA - ESTADO DESARMADA");
}


// ------------------------------------------------------------
// loop() — se repite mientras la simulación está en Start
//  1) Lee A / D / E del Serial Monitor
//  2) Cada 200 ms mide distancia; si ARMADA y < 20 cm → ALERTA
//  3) Si está en ALERTA, pita el piezo a 1500 Hz intermitente
// ------------------------------------------------------------

void loop() {
  if (Serial.available()) {
    char comando = Serial.read();

    if (comando == 'A' || comando == 'a') {
      if (estado == DESARMADA) {
        estado = ARMADA;
        actualizarLeds();
        Serial.println("ESTADO ARMADA");
      }
    } else if (comando == 'D' || comando == 'd') {
      estado = DESARMADA;
      noTone(BUZZER);
      pitidoEncendido = false;
      actualizarLeds();
      Serial.println("ESTADO DESARMADA");
    } else if (comando == 'E' || comando == 'e') {
      if (estado == DESARMADA) Serial.println("ESTADO DESARMADA");
      if (estado == ARMADA)    Serial.println("ESTADO ARMADA");
      if (estado == ALERTA)    Serial.println("ESTADO ALERTA");
    }
  }

  if (millis() - ultimaMedicion >= INTERVALO_MEDICION_MS) {
    ultimaMedicion = millis();
    long distancia = medirDistanciaCm();

    if (distancia > 0) {
      Serial.print("DISTANCIA: ");
      Serial.print(distancia);
      Serial.println(" cm");

      if (estado == ARMADA && distancia < DISTANCIA_ALERTA_CM) {
        estado = ALERTA;
        actualizarLeds();
        Serial.println("ALERTA: OBJETO CERCANO");
      }
    }
  }

  if (estado == ALERTA && millis() - ultimoPitido >= INTERVALO_PITIDO_MS) {
    ultimoPitido = millis();
    pitidoEncendido = !pitidoEncendido;

    if (pitidoEncendido) {
      tone(BUZZER, TONO_HZ);
    } else {
      noTone(BUZZER);
    }
  }
}
