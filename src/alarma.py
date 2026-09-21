"""
Máquina de estados para el sistema de alarma antirrobo.
Estados: DESARMADA, ARMADA, ALERTA

La misma máquina corre en Arduino UNO + HC-05
(/home/cocus/Documentos/proyectos/IOT_PRUEBA_1/alarma_uno/alarma_uno.ino),
con el esquema Bluetooth del semáforo de la Unidad 1. El hardware es opcional:
esta clase se prueba y se usa en la demo web sin placa.
"""

from enum import Enum
from datetime import datetime
from typing import Optional, List, Dict


class EstadoAlarma(Enum):
    """Estados posibles de la alarma."""
    DESARMADA = "desarmada"
    ARMADA = "armada"
    ALERTA = "alerta"


class EventoAlarma:
    """Representa un evento en el historial de la alarma."""
    def __init__(self, tipo: str, mensaje: str, timestamp: Optional[datetime] = None):
        self.tipo = tipo
        self.mensaje = mensaje
        self.timestamp = timestamp or datetime.now()
    
    def to_dict(self) -> Dict:
        return {
            "tipo": self.tipo,
            "mensaje": self.mensaje,
            "timestamp": self.timestamp.isoformat()
        }


class SistemaAlarma:
    """
    Sistema de alarma antirrobo con máquina de estados.
    
    Transiciones válidas:
    - DESARMADA -> ARMADA (cuando el usuario arma la alarma)
    - ARMADA -> ALERTA (cuando se detecta intrusión)
    - ARMADA -> DESARMADA (cuando el usuario desarma)
    - ALERTA -> DESARMADA (cuando el usuario desarma después de una alerta)
    """
    
    def __init__(self):
        self.estado = EstadoAlarma.DESARMADA
        self.historial: List[EventoAlarma] = []
        self.alerta_activa = False
        self._registrar_evento("sistema", "Sistema de alarma inicializado")
    
    def armar(self) -> bool:
        """
        Arma la alarma si está desarmada.
        Retorna True si se armó correctamente, False si ya estaba armada o en alerta.
        """
        if self.estado == EstadoAlarma.DESARMADA:
            self.estado = EstadoAlarma.ARMADA
            self.alerta_activa = False
            self._registrar_evento("armar", "Alarma armada")
            return True
        return False
    
    def desarmar(self) -> bool:
        """
        Desarma la alarma desde cualquier estado.
        Retorna True si se desarmó correctamente.
        """
        if self.estado != EstadoAlarma.DESARMADA:
            estado_anterior = self.estado.value
            self.estado = EstadoAlarma.DESARMADA
            self.alerta_activa = False
            self._registrar_evento("desarmar", f"Alarma desarmada (estaba en {estado_anterior})")
            return True
        return False
    
    def detectar_intrusion(self) -> bool:
        """
        Registra una intrusión si la alarma está armada.
        Retorna True si se activó la alerta, False si la alarma no estaba armada.
        """
        if self.estado == EstadoAlarma.ARMADA:
            self.estado = EstadoAlarma.ALERTA
            self.alerta_activa = True
            self._registrar_evento("intrusion", "¡INTRUSIÓN DETECTADA! Alarma activada")
            return True
        return False
    
    def obtener_estado(self) -> Dict:
        """Retorna el estado actual del sistema."""
        return {
            "estado": self.estado.value,
            "alerta_activa": self.alerta_activa,
            "timestamp": datetime.now().isoformat()
        }
    
    def obtener_historial(self, limite: int = 50) -> List[Dict]:
        """Retorna el historial de eventos recientes."""
        return [evento.to_dict() for evento in self.historial[-limite:]]
    
    def _registrar_evento(self, tipo: str, mensaje: str):
        """Registra un evento en el historial."""
        evento = EventoAlarma(tipo, mensaje)
        self.historial.append(evento)
        print(f"[{evento.timestamp.strftime('%H:%M:%S')}] {mensaje}")
