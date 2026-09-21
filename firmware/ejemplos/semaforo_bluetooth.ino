/*
 * ============================================================
 * SEMAFORO CONTROLADO POR BLUETOOTH
 * ============================================================
 *
 * Ejemplo de la Unidad 1 (Aplicaciones Moviles e IoT).
 * La alarma antirrobo reutiliza ESTE mismo esquema:
 * HC-05 en pines 10/9, LEDs 13/12/11 y comandos de un caracter.
 *
 * COMANDOS:
 *   R  -> Enciende LED ROJO (Alto) y DETIENE la secuencia
 *   A  -> Enciende LED AMARILLO (Precaucion) y DETIENE la secuencia
 *   V  -> Enciende LED VERDE (Paso) y DETIENE la secuencia
 *   S  -> Ejecuta secuencia automatica INFINITA
 *
 * ============================================================
 * CONEXIONES DEL CIRCUITO
 * ============================================================
 *
 * ARDUINO UNO           HC-05 BLUETOOTH
 * ─────────────         ──────────────
 * Pin 10 (RX)    ------> TX
 * Pin 9 (TX)     ------> RX
 * 5V             ------> VCC
 * GND            ------> GND
 * 3.3V           ------> EN (solo para modo AT)
 *
 * ARDUINO UNO           LEDS
 * ─────────────         ────
 * Pin 13         ------> LED ROJO (+ resistencia 220 ohm)
 * Pin 12         ------> LED AMARILLO (+ resistencia 220 ohm)
 * Pin 11         ------> LED VERDE (+ resistencia 220 ohm)
 * GND            ------> Cátodo de los 3 LEDs
 *
 * ============================================================
 */

#include <SoftwareSerial.h>


// ============================================================
// DEFINICION DE PINES
// ============================================================

const int LED_ROJO    = 13;  // LED rojo en pin 13
const int LED_AMARILLO = 12; // LED amarillo en pin 12
const int LED_VERDE   = 11;  // LED verde en pin 11


// ============================================================
// CONFIGURACION BLUETOOTH
// ============================================================

// RX en pin 10, TX en pin 9 (conexion cruzada con HC-05)
SoftwareSerial bluetooth(10, 9);  // (RX, TX)


// ============================================================
// VARIABLES GLOBALES
// ============================================================

char comando = ' ';          // Almacena el comando recibido
bool modoSecuencia = false;  // Indica si el semaforo esta en modo automatico


// ============================================================
// SETUP - CONFIGURACION INICIAL
// ============================================================

void setup() {
  
  // Configurar pines del semaforo como SALIDA
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  
  // Apagar todos los LEDs al inicio
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_VERDE, LOW);
  
  // Iniciar comunicacion con el PC (Monitor Serie)
  Serial.begin(9600);
  
  // Iniciar comunicacion con el Bluetooth
  bluetooth.begin(9600);
  
  // Mensaje de bienvenida
  Serial.println("=== SEMAFORO BLUETOOTH ===");
  Serial.println("Comandos: R (Rojo), A (Amarillo), V (Verde), S (Secuencia INFINITA)");
  Serial.println("ROJO en pin 13 | AMARILLO en pin 12 | VERDE en pin 11");
  Serial.println("Bluetooth: RX en pin 10, TX en pin 9");
  Serial.println("Presiona R, A o V para DETENER la secuencia");
}


// ============================================================
// LOOP - CICLO PRINCIPAL
// ============================================================

void loop() {
  
  // Verificar si llego un comando desde el PC (Monitor Serie)
  if (Serial.available()) {
    comando = Serial.read();
    
    // Ignorar caracteres de control
    if (comando != '\r' && comando != '\n' && comando != ' ') {
      procesarComando(comando);
    }
  }
  
  // Verificar si llego un comando desde el Bluetooth
  if (bluetooth.available()) {
    comando = bluetooth.read();
    
    // Ignorar caracteres de control
    if (comando != '\r' && comando != '\n' && comando != ' ') {
      procesarComando(comando);
    }
  }
  
  // Si el modo secuencia esta activo, ejecutar un paso de la secuencia
  // NOTA: Cada paso tiene su propio delay, por lo que el loop "espera"
  //       mientras se ejecuta la secuencia. Los comandos solo se
  //       revisan al inicio de cada paso.
  if (modoSecuencia) {
    ejecutarPasoVerde();
    ejecutarPasoAmarillo();
    ejecutarPasoRojo();
  }
}


// ============================================================
// FUNCION PARA PROCESAR COMANDOS
// ============================================================

void procesarComando(char caracter) {
  
  // Convertir a mayuscula (permite minusculas tambien)
  caracter = toupper(caracter);
  
  // Apagar todos los LEDs primero
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_VERDE, LOW);
  
  // Evaluar el comando recibido
  if (caracter == 'R') {
    // DETENER la secuencia
    modoSecuencia = false;
    digitalWrite(LED_ROJO, HIGH);
    Serial.println("ROJO - Secuencia DETENIDA");
    bluetooth.println("ROJO - Secuencia DETENIDA");
  }
  else if (caracter == 'A') {
    // DETENER la secuencia
    modoSecuencia = false;
    digitalWrite(LED_AMARILLO, HIGH);
    Serial.println("AMARILLO - Secuencia DETENIDA");
    bluetooth.println("AMARILLO - Secuencia DETENIDA");
  }
  else if (caracter == 'V') {
    // DETENER la secuencia
    modoSecuencia = false;
    digitalWrite(LED_VERDE, HIGH);
    Serial.println("VERDE - Secuencia DETENIDA");
    bluetooth.println("VERDE - Secuencia DETENIDA");
  }
  else if (caracter == 'S') {
    // ACTIVAR la secuencia infinita
    modoSecuencia = true;
    Serial.println("SECUENCIA INFINITA ACTIVADA");
    bluetooth.println("SECUENCIA INFINITA ACTIVADA");
    Serial.println("Presiona R, A o V para DETENER");
    bluetooth.println("Presiona R, A o V para DETENER");
  }
  else {
    Serial.println("Comando no valido. Use: R, A, V, S");
    bluetooth.println("Comando no valido. Use: R, A, V, S");
  }
}


// ============================================================
// FUNCIONES DE PASOS DE LA SECUENCIA
// ============================================================

void ejecutarPasoVerde() {
  // Solo ejecutar si la secuencia sigue activa
  if (!modoSecuencia) return;
  
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_VERDE, HIGH);
  
  Serial.println("VERDE (Paso)");
  bluetooth.println("VERDE");
  
  // Esperar 4 segundos, pero revisando si llega un comando
  delay(4000);
}

void ejecutarPasoAmarillo() {
  // Solo ejecutar si la secuencia sigue activa
  if (!modoSecuencia) return;
  
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARILLO, HIGH);
  
  Serial.println("AMARILLO (Precaucion)");
  bluetooth.println("AMARILLO");
  
  // Esperar 1.5 segundos
  delay(1500);
}

void ejecutarPasoRojo() {
  // Solo ejecutar si la secuencia sigue activa
  if (!modoSecuencia) return;
  
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_ROJO, HIGH);
  
  Serial.println("ROJO (Alto)");
  bluetooth.println("ROJO");
  
  // Esperar 4 segundos
  delay(4000);
}
