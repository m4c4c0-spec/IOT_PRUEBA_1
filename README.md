# 🏠 Sistema de Alarma Antirrobo

Proyecto educativo para estudiantes de Analista Programador - Chile 🇨🇱

## 📋 Descripción

Sistema de seguridad automatizado para el hogar que detecta intrusiones cuando la vivienda se encuentra deshabitada. Ante un acceso no autorizado, el sistema envía una alerta inmediata al usuario. La solución permite activar y desactivar la alarma a través de una interfaz móvil, e incluye una función de enlace directo para contactar a emergencias (133 - Carabineros de Chile).

### Características

✅ Control de alarma (armar/desarmar)  
✅ Detección de intrusión con sensores simulados  
✅ Alertas en tiempo real  
✅ Enlace directo a emergencias (tel:133)  
✅ Interfaz móvil responsive  
✅ Historial de eventos  
✅ Tests automatizados  
✅ Simulador de hardware (sin necesidad de sensores físicos)  
✅ Firmware opcional para **Arduino UNO + HC-05** (mismo esquema que el semáforo del curso)

## 🚀 Inicio Rápido (Primera Sesión)

### Prerrequisitos

- Python 3.8 o superior
- Navegador web moderno
- Editor de código (VS Code, PyCharm, etc.)

### Instalación

1. **Clonar o descargar el repositorio**

```bash
git clone <url-repositorio>
cd alarma-antirrobo
```

2. **Instalar dependencias**

```bash
pip install -r requirements.txt
```

3. **Ejecutar el sistema**

```bash
python run.py
```

4. **Abrir en el navegador**

Navegar a: **http://localhost:8133**

¡Listo! El sistema está funcionando. 🎉

## 🎯 Cómo Usar

1. **Armar la alarma:** Presionar el botón "🔒 Armar Alarma"
2. **Simular intrusión:** Usar el panel "Simulador de Sensores" y presionar "Simular Movimiento"
3. **Ver alerta:** El sistema mostrará una alerta roja con opción de llamar al 133
4. **Desarmar:** Presionar "🔓 Desarmar Alarma" o "🔇 Silenciar Alerta"

## 📚 Estructura del Proyecto

```
alarma-antirrobo/
├── README.md                    # Este archivo
├── requirements.txt             # Dependencias Python
├── run.py                       # Script principal para ejecutar
│
├── src/                         # Código fuente
│   ├── alarma.py               # Máquina de estados de la alarma
│   ├── sensores.py             # Simulador de sensores (PIR y puerta)
│   ├── app.py                  # Servidor web Flask
│   └── static/                 # Interfaz web
│       ├── index.html          # Página principal
│       ├── style.css           # Estilos
│       └── app.js              # Lógica del cliente
│
├── tests/                       # Tests automatizados
│   ├── test_alarma.py          # Tests de la máquina de estados
│   └── test_sensores.py        # Tests de sensores
│
│
└── firmware/                    # Ejemplo del curso y puente USB
    ├── README.md               # Apunta a /home/cocus/Documentos/proyectos/IOT_PRUEBA_1
    ├── puente_serial.py        # Une el UNO con la app por USB
    └── ejemplos/
        └── semaforo_bluetooth.ino  # Ejemplo original de la Unidad 1
```

## 🧪 Ejecutar Tests

Los tests verifican que la máquina de estados funcione correctamente:

```bash
# Ejecutar todos los tests
pytest tests/ -v

# Ejecutar solo tests de alarma
pytest tests/test_alarma.py -v

# Ejecutar solo tests de sensores
pytest tests/test_sensores.py -v
```

Todos los tests deben pasar ✅ antes de continuar a la siguiente sesión.

## 📅 Plan de Trabajo Semanal

### Semana 1: Setup y Exploración ✅ (Esta sesión)
**Objetivo:** Entender el proyecto y ejecutarlo

- ✅ Instalar Python y dependencias
- ✅ Ejecutar el sistema web
- ✅ Probar armar/desarmar alarma
- ✅ Simular una intrusión
- ✅ Ejecutar tests (pytest)

**Entregable:** Captura de pantalla mostrando la alerta funcionando

---

### Semana 2: Máquina de Estados
**Objetivo:** Comprender y modificar la lógica de la alarma

**Tareas:**
1. Leer y entender `src/alarma.py`
2. Dibujar diagrama de estados en papel
3. Agregar un nuevo estado "ARMADO_PARCIAL" (solo sensor de puerta, sin PIR)
4. Agregar botón en la interfaz para "Armar Parcial"
5. Escribir tests para el nuevo estado

**Archivos a modificar:**
- `src/alarma.py` - Agregar estado ARMADO_PARCIAL
- `src/static/index.html` - Agregar botón
- `src/static/app.js` - Agregar función
- `src/app.py` - Agregar endpoint `/api/armar-parcial`
- `tests/test_alarma.py` - Agregar tests

**Entregable:** Tests pasando con el nuevo estado

---

### Semana 3: Persistencia de Datos
**Objetivo:** Guardar el historial de eventos en un archivo

**Tareas:**
1. Modificar `src/alarma.py` para guardar eventos en `historial.json`
2. Cargar historial al iniciar el sistema
3. Agregar botón "Limpiar Historial"
4. Limitar historial a últimos 100 eventos

**Archivos a modificar:**
- `src/alarma.py` - Métodos `guardar_historial()` y `cargar_historial()`
- `src/app.py` - Endpoint `/api/historial/limpiar`
- `src/static/app.js` - Función limpiar historial

**Entregable:** Historial que persiste después de reiniciar

---

### Semana 4: Mejoras en Sensores
**Objetivo:** Agregar configuración y calibración de sensores

**Tareas:**
1. Agregar temporizador de entrada (30 segundos para desarmar después de abrir puerta)
2. Agregar retardo de activación (1 minuto después de armar para salir)
3. Configuración visual de tiempos en la interfaz

**Archivos a modificar:**
- `src/sensores.py` - Agregar timers
- `src/static/index.html` - Panel de configuración
- `src/app.py` - Endpoints de configuración

**Entregable:** Demo de salir y entrar sin activar alarma en los tiempos configurados

---

### Semana 5: Notificaciones y UX
**Objetivo:** Mejorar la experiencia de usuario

**Tareas:**
1. Agregar notificaciones visuales (toasts) en lugar de console.log
2. Agregar sonido de alerta (usando Web Audio API)
3. Agregar confirmación antes de armar/desarmar
4. Modo oscuro (dark mode)

**Archivos a modificar:**
- `src/static/app.js` - Sistema de notificaciones
- `src/static/style.css` - Dark mode y animaciones
- `src/static/index.html` - Toggle de dark mode

**Entregable:** Video de 30 segundos mostrando las mejoras

---

### Semana 6: Códigos de Acceso
**Objetivo:** Agregar autenticación simple

**Tareas:**
1. Crear sistema de PIN de 4 dígitos
2. Solo permitir desarmar con PIN correcto
3. Guardar PINs en archivo (simple, sin encriptar por ahora)
4. Panel de administración de PINs

**Archivos a modificar:**
- `src/alarma.py` - Manejo de PINs
- `src/app.py` - Validación de PIN
- `src/static/index.html` - Teclado numérico
- `tests/test_alarma.py` - Tests de autenticación

**Entregable:** Demo desarmar con PIN correcto e incorrecto

---

### Semana 7: Arduino UNO + HC-05 (opcional, con hardware)
**Objetivo:** Usar el mismo esquema del semáforo Bluetooth de la Unidad 1

**Tareas:**
1. Comparar `firmware/ejemplos/semaforo_bluetooth.ino` con `/home/cocus/Documentos/proyectos/IOT_PRUEBA_1/alarma_uno/alarma_uno.ino`
2. Placa **Arduino Uno**, 9600 baud. Comandos de un carácter: `A` armar, `D` desarmar, `E` estado
3. LEDs: verde = desarmada, amarillo = armada, rojo = alerta (pines 11, 12 y 13)
4. Disparar el PIR y comprobar `ALERTA INTRUSION` por USB y por Bluetooth
5. Cambiar el PIN de fábrica del HC-05 (no dejar `1234`)

El celular habla con el UNO por HC-05. La app web sigue mostrando el `tel:133`.

**Entregable:** Foto del UNO en alerta y captura del Monitor Serie / terminal Bluetooth

---

### Semana 7+: Extensiones Opcionales

**Ideas para continuar:**
- 🔌 Montar el circuito en Arduino UNO + HC-05 (carpeta `firmware/`, mismo cableado que el semáforo)
- 📱 Notificaciones push (FCM/APNs) en una semana posterior; no hacen falta para correr el proyecto
- 🎨 Diseñar carcasa 3D para imprimir
- 📊 Dashboard con estadísticas (intrusiones por día/hora)
- 👥 Múltiples usuarios con diferentes PINs
- 📷 Integrar cámara para capturar foto al detectar intrusión
- 🌐 Deploy en servidor cloud (Heroku, Railway, etc.)
- 🔐 Encriptar PINs con bcrypt
- 📧 Enviar emails de alerta

## 🛠️ Tecnologías Utilizadas

- **Backend:** Python 3, Flask (micro-framework web)
- **Frontend:** HTML5, CSS3, JavaScript (vanilla, sin frameworks)
- **Testing:** pytest
- **Hardware (opcional):** Arduino UNO, HC-05, PIR, contacto de puerta, LEDs 11/12/13, buzzer

## 💡 Conceptos Aprendidos

Este proyecto enseña:

1. **Máquinas de estados finitos** - Modelar comportamiento de sistemas
2. **API REST** - Comunicación cliente-servidor
3. **Programación asíncrona** - Actualización en tiempo real
4. **Testing automatizado** - Garantizar calidad del código
5. **Diseño responsive** - Interfaces móviles
6. **Arquitectura MVC** - Separación de responsabilidades
7. **Arduino UNO + HC-05** (opcional) — mismos pines que el semáforo de la Unidad 1

## 🐛 Solución de Problemas

### El servidor no inicia

```bash
# Verificar que el puerto 8133 no está en uso
netstat -an | grep 8133

# Si está en uso, matar el proceso o cambiar puerto en src/app.py
```

### Los tests fallan

```bash
# Asegurarse de estar en la raíz del proyecto
pwd

# Reinstalar dependencias
pip install -r requirements.txt

# Ejecutar tests con más detalle
pytest tests/ -vv
```

### No puedo acceder desde el celular

1. Asegurarse de que el computador y celular están en la misma red WiFi
2. Obtener IP del computador:
   ```bash
   # Windows
   ipconfig
   
   # Linux/Mac
   ifconfig
   ```
3. En el celular, navegar a `http://<IP-COMPUTADOR>:8133`

## 📖 Recursos Adicionales

- [Documentación de Flask](https://flask.palletsprojects.com/)
- [Guía de pytest](https://docs.pytest.org/)
- [Tutorial de Máquinas de Estados](https://refactoring.guru/es/design-patterns/state)
- [Números de emergencia en Chile](https://www.chileatiende.gob.cl/fichas/2130-numeros-de-emergencia)

## 🤝 Contribuir

Este es un proyecto educativo. Para agregar mejoras:

1. Crear una rama con tu nombre: `git checkout -b mejora-nombre`
2. Hacer cambios y commits descriptivos
3. Probar que los tests pasen: `pytest tests/ -v`
4. Hacer push y crear un Pull Request

## 📜 Licencia

Proyecto educativo de código abierto. Libre de usar y modificar.

---

## 👨‍🏫 Notas para Profesores

Este proyecto está diseñado para:
- **Modalidad:** Grupos de 2-4 estudiantes
- **Duración:** 6-12 semanas (1 sesión semanal de 2-3 horas)
- **Nivel:** Técnico medio (Analista Programador)
- **Prerrequisitos:** Python básico, HTML/CSS básico

### Evaluación Sugerida

- **Semana 1-2 (20%):** Setup y comprensión del código base
- **Semana 3-4 (30%):** Implementación de nuevas funcionalidades
- **Semana 5-6 (30%):** Tests y mejoras de UX
- **Semana 7+ (20%):** Extensión opcional o documentación final

### Rúbrica

| Criterio | Peso | Descripción |
|----------|------|-------------|
| Funcionalidad | 40% | El sistema funciona según especificaciones |
| Código limpio | 20% | Código legible, bien comentado |
| Tests | 20% | Tests pasan y tienen buena cobertura |
| Documentación | 10% | README actualizado con cambios |
| Trabajo en equipo | 10% | Commits distribuidos, git bien usado |

---

¿Preguntas? Abrir un Issue en el repositorio o consultar con el profesor.

**¡Buena suerte con el proyecto! 🚀**
