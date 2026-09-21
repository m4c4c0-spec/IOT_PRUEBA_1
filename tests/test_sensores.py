"""
Tests para los simuladores de sensores.
Ejecutar con: pytest tests/test_sensores.py -v
"""

import sys
import os

# Agregar directorio src al path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from sensores import SensorPIR, SensorPuerta, MonitorSensores
from alarma import SistemaAlarma, EstadoAlarma


def test_sensor_pir_inicial():
    """El sensor PIR debe iniciar sin detectar movimiento."""
    sensor = SensorPIR()
    assert sensor.movimiento_detectado == False
    assert sensor.ultima_deteccion == None


def test_sensor_pir_simular_movimiento():
    """El sensor PIR debe detectar movimiento cuando se simula."""
    sensor = SensorPIR()
    sensor.simular_movimiento()
    
    assert sensor.movimiento_detectado == True
    assert sensor.ultima_deteccion != None


def test_sensor_pir_limpiar():
    """El sensor PIR debe limpiarse correctamente."""
    sensor = SensorPIR()
    sensor.simular_movimiento()
    sensor.limpiar()
    
    assert sensor.movimiento_detectado == False


def test_sensor_pir_leer():
    """El método leer() debe retornar el estado correcto."""
    sensor = SensorPIR()
    assert sensor.leer() == False
    
    sensor.simular_movimiento()
    assert sensor.leer() == True


def test_sensor_pir_obtener_estado():
    """El método obtener_estado() debe retornar un diccionario válido."""
    sensor = SensorPIR()
    estado = sensor.obtener_estado()
    
    assert "tipo" in estado
    assert "movimiento_detectado" in estado
    assert "ultima_deteccion" in estado
    assert estado["tipo"] == "PIR"


def test_sensor_puerta_inicial():
    """El sensor de puerta debe iniciar cerrada."""
    sensor = SensorPuerta()
    assert sensor.puerta_abierta == False
    assert sensor.ultima_apertura == None


def test_sensor_puerta_abrir():
    """El sensor debe detectar apertura de puerta."""
    sensor = SensorPuerta()
    sensor.simular_apertura()
    
    assert sensor.puerta_abierta == True
    assert sensor.ultima_apertura != None


def test_sensor_puerta_cerrar():
    """El sensor debe detectar cierre de puerta."""
    sensor = SensorPuerta()
    sensor.simular_apertura()
    sensor.simular_cierre()
    
    assert sensor.puerta_abierta == False


def test_sensor_puerta_leer():
    """El método leer() debe retornar el estado correcto."""
    sensor = SensorPuerta()
    assert sensor.leer() == False
    
    sensor.simular_apertura()
    assert sensor.leer() == True


def test_monitor_inicializacion():
    """El monitor debe inicializar con una alarma y sensores."""
    alarma = SistemaAlarma()
    monitor = MonitorSensores(alarma)
    
    assert monitor.alarma == alarma
    assert monitor.sensor_pir is not None
    assert monitor.sensor_puerta is not None


def test_monitor_verificar_sin_intrusion():
    """El monitor no debe detectar intrusión si no hay sensores activos."""
    alarma = SistemaAlarma()
    monitor = MonitorSensores(alarma)
    alarma.armar()
    
    resultado = monitor.verificar_sensores()
    
    assert resultado == False
    assert alarma.estado == EstadoAlarma.ARMADA


def test_monitor_verificar_con_movimiento():
    """El monitor debe detectar intrusión con movimiento."""
    alarma = SistemaAlarma()
    monitor = MonitorSensores(alarma)
    alarma.armar()
    
    monitor.sensor_pir.simular_movimiento()
    resultado = monitor.verificar_sensores()
    
    assert resultado == True
    assert alarma.estado == EstadoAlarma.ALERTA


def test_monitor_verificar_con_puerta():
    """El monitor debe detectar intrusión con puerta abierta."""
    alarma = SistemaAlarma()
    monitor = MonitorSensores(alarma)
    alarma.armar()
    
    monitor.sensor_puerta.simular_apertura()
    resultado = monitor.verificar_sensores()
    
    assert resultado == True
    assert alarma.estado == EstadoAlarma.ALERTA


def test_monitor_no_detectar_si_desarmada():
    """El monitor no debe activar alerta si la alarma está desarmada."""
    alarma = SistemaAlarma()
    monitor = MonitorSensores(alarma)
    
    # Alarma desarmada
    monitor.sensor_pir.simular_movimiento()
    resultado = monitor.verificar_sensores()
    
    assert resultado == False
    assert alarma.estado == EstadoAlarma.DESARMADA


def test_monitor_obtener_estado_sensores():
    """El monitor debe retornar el estado de todos los sensores."""
    alarma = SistemaAlarma()
    monitor = MonitorSensores(alarma)
    
    estado = monitor.obtener_estado_sensores()
    
    assert "pir" in estado
    assert "puerta" in estado
    assert estado["pir"]["tipo"] == "PIR"
    assert estado["puerta"]["tipo"] == "Puerta"


def test_integracion_completa():
    """Test de integración completa del sistema."""
    alarma = SistemaAlarma()
    monitor = MonitorSensores(alarma)
    
    # Armar la alarma
    alarma.armar()
    assert alarma.estado == EstadoAlarma.ARMADA
    
    # Simular intrusión por movimiento
    monitor.sensor_pir.simular_movimiento()
    monitor.verificar_sensores()
    assert alarma.estado == EstadoAlarma.ALERTA
    
    # Desarmar
    alarma.desarmar()
    assert alarma.estado == EstadoAlarma.DESARMADA
    
    # Armar nuevamente
    alarma.armar()
    
    # Simular intrusión por puerta
    monitor.sensor_puerta.simular_apertura()
    monitor.verificar_sensores()
    assert alarma.estado == EstadoAlarma.ALERTA
