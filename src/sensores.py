"""
Simulador de sensores para el sistema de alarma.
Simula un sensor PIR (movimiento) y un sensor de puerta.
"""

import random
from datetime import datetime
from typing import Dict


class SensorPIR:
    """
    Simulador de sensor PIR (Passive Infrared) para detectar movimiento.
    En Arduino UNO el OUT del PIR va al pin 2
    (ver /home/cocus/Documentos/proyectos/IOT_PRUEBA_1/alarma_uno).
    """
    
    def __init__(self):
        self.movimiento_detectado = False
        self.ultima_deteccion = None
    
    def simular_movimiento(self):
        """Simula la detección de movimiento."""
        self.movimiento_detectado = True
        self.ultima_deteccion = datetime.now()
    
    def limpiar(self):
        """Limpia el estado del sensor."""
        self.movimiento_detectado = False
    
    def leer(self) -> bool:
        """Lee el estado actual del sensor."""
        return self.movimiento_detectado
    
    def obtener_estado(self) -> Dict:
        """Retorna el estado completo del sensor."""
        return {
            "tipo": "PIR",
            "movimiento_detectado": self.movimiento_detectado,
            "ultima_deteccion": self.ultima_deteccion.isoformat() if self.ultima_deteccion else None
        }


class SensorPuerta:
    """
    Simulador de sensor de puerta (contacto magnético).
    En Arduino UNO el reed switch va al pin 3 con pull-up interno.
    """
    
    def __init__(self):
        self.puerta_abierta = False
        self.ultima_apertura = None
    
    def simular_apertura(self):
        """Simula la apertura de la puerta."""
        self.puerta_abierta = True
        self.ultima_apertura = datetime.now()
    
    def simular_cierre(self):
        """Simula el cierre de la puerta."""
        self.puerta_abierta = False
    
    def leer(self) -> bool:
        """Lee el estado actual del sensor (True = abierta, False = cerrada)."""
        return self.puerta_abierta
    
    def obtener_estado(self) -> Dict:
        """Retorna el estado completo del sensor."""
        return {
            "tipo": "Puerta",
            "puerta_abierta": self.puerta_abierta,
            "ultima_apertura": self.ultima_apertura.isoformat() if self.ultima_apertura else None
        }


class MonitorSensores:
    """
    Monitor que coordina todos los sensores y detecta intrusiones.
    """
    
    def __init__(self, alarma):
        self.alarma = alarma
        self.sensor_pir = SensorPIR()
        self.sensor_puerta = SensorPuerta()
    
    def verificar_sensores(self):
        """
        Verifica todos los sensores y activa la alarma si detecta intrusión.
        Retorna True si se detectó intrusión.
        """
        # Verificar si hay movimiento o puerta abierta
        movimiento = self.sensor_pir.leer()
        puerta_abierta = self.sensor_puerta.leer()
        
        if movimiento or puerta_abierta:
            return self.alarma.detectar_intrusion()
        
        return False
    
    def obtener_estado_sensores(self) -> Dict:
        """Retorna el estado de todos los sensores."""
        return {
            "pir": self.sensor_pir.obtener_estado(),
            "puerta": self.sensor_puerta.obtener_estado()
        }
