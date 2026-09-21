"""
Tests para la máquina de estados del sistema de alarma.
Ejecutar con: pytest tests/test_alarma.py -v
"""

import sys
import os

# Agregar directorio src al path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from alarma import SistemaAlarma, EstadoAlarma


def test_estado_inicial():
    """El sistema debe iniciar en estado DESARMADA."""
    sistema = SistemaAlarma()
    assert sistema.estado == EstadoAlarma.DESARMADA
    assert sistema.alerta_activa == False


def test_armar_desde_desarmada():
    """Debe poder armar la alarma cuando está desarmada."""
    sistema = SistemaAlarma()
    resultado = sistema.armar()
    
    assert resultado == True
    assert sistema.estado == EstadoAlarma.ARMADA
    assert sistema.alerta_activa == False


def test_no_armar_cuando_ya_armada():
    """No debe poder armar la alarma si ya está armada."""
    sistema = SistemaAlarma()
    sistema.armar()
    resultado = sistema.armar()
    
    assert resultado == False
    assert sistema.estado == EstadoAlarma.ARMADA


def test_desarmar_desde_armada():
    """Debe poder desarmar la alarma cuando está armada."""
    sistema = SistemaAlarma()
    sistema.armar()
    resultado = sistema.desarmar()
    
    assert resultado == True
    assert sistema.estado == EstadoAlarma.DESARMADA
    assert sistema.alerta_activa == False


def test_no_desarmar_cuando_ya_desarmada():
    """No debe poder desarmar si ya está desarmada."""
    sistema = SistemaAlarma()
    resultado = sistema.desarmar()
    
    assert resultado == False
    assert sistema.estado == EstadoAlarma.DESARMADA


def test_detectar_intrusion_cuando_armada():
    """Debe detectar intrusión y activar alerta cuando está armada."""
    sistema = SistemaAlarma()
    sistema.armar()
    resultado = sistema.detectar_intrusion()
    
    assert resultado == True
    assert sistema.estado == EstadoAlarma.ALERTA
    assert sistema.alerta_activa == True


def test_no_detectar_intrusion_cuando_desarmada():
    """No debe activar alerta si está desarmada."""
    sistema = SistemaAlarma()
    resultado = sistema.detectar_intrusion()
    
    assert resultado == False
    assert sistema.estado == EstadoAlarma.DESARMADA
    assert sistema.alerta_activa == False


def test_desarmar_desde_alerta():
    """Debe poder desarmar la alarma cuando está en alerta."""
    sistema = SistemaAlarma()
    sistema.armar()
    sistema.detectar_intrusion()
    resultado = sistema.desarmar()
    
    assert resultado == True
    assert sistema.estado == EstadoAlarma.DESARMADA
    assert sistema.alerta_activa == False


def test_obtener_estado():
    """Debe retornar el estado actual correctamente."""
    sistema = SistemaAlarma()
    estado = sistema.obtener_estado()
    
    assert "estado" in estado
    assert "alerta_activa" in estado
    assert "timestamp" in estado
    assert estado["estado"] == "desarmada"
    assert estado["alerta_activa"] == False


def test_historial_registra_eventos():
    """Debe registrar eventos en el historial."""
    sistema = SistemaAlarma()
    
    # Debe haber al menos el evento de inicialización
    historial_inicial = sistema.obtener_historial()
    assert len(historial_inicial) >= 1
    
    # Armar y verificar nuevo evento
    sistema.armar()
    historial_despues = sistema.obtener_historial()
    assert len(historial_despues) > len(historial_inicial)


def test_historial_formato():
    """El historial debe tener el formato correcto."""
    sistema = SistemaAlarma()
    sistema.armar()
    historial = sistema.obtener_historial()
    
    for evento in historial:
        assert "tipo" in evento
        assert "mensaje" in evento
        assert "timestamp" in evento


def test_flujo_completo():
    """Test del flujo completo: armar -> intrusión -> desarmar."""
    sistema = SistemaAlarma()
    
    # Estado inicial
    assert sistema.estado == EstadoAlarma.DESARMADA
    
    # Armar
    sistema.armar()
    assert sistema.estado == EstadoAlarma.ARMADA
    
    # Simular intrusión
    sistema.detectar_intrusion()
    assert sistema.estado == EstadoAlarma.ALERTA
    assert sistema.alerta_activa == True
    
    # Desarmar
    sistema.desarmar()
    assert sistema.estado == EstadoAlarma.DESARMADA
    assert sistema.alerta_activa == False
    
    # Verificar que hay eventos en el historial
    historial = sistema.obtener_historial()
    assert len(historial) >= 4  # init, armar, intrusión, desarmar
