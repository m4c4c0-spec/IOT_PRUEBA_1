"""
Tests del protocolo USB del Arduino UNO.
No necesitan la placa: solo interpretan las líneas de texto.
"""

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from protocolo_uno import (
    parsear_linea,
    comando_armar,
    comando_desarmar,
    comando_consultar_estado,
)


def test_parsear_estado_desarmada():
    mensaje = parsear_linea("ESTADO DESARMADA")
    assert mensaje == {"tipo": "estado", "valor": "desarmada"}


def test_parsear_estado_armada():
    mensaje = parsear_linea("ESTADO ARMADA\r")
    assert mensaje == {"tipo": "estado", "valor": "armada"}


def test_parsear_estado_alerta():
    mensaje = parsear_linea("estado alerta")
    assert mensaje == {"tipo": "estado", "valor": "alerta"}


def test_parsear_evento_pir():
    mensaje = parsear_linea("EVENTO PIR")
    assert mensaje == {"tipo": "evento", "valor": "pir"}


def test_parsear_evento_puerta():
    mensaje = parsear_linea("EVENTO PUERTA")
    assert mensaje == {"tipo": "evento", "valor": "puerta"}


def test_parsear_alerta_intrusion():
    mensaje = parsear_linea("ALERTA INTRUSION")
    assert mensaje == {"tipo": "alerta", "valor": "intrusion"}


def test_parsear_ok_armar():
    mensaje = parsear_linea("OK ARMAR")
    assert mensaje == {"tipo": "ok", "valor": "armar"}


def test_parsear_error():
    mensaje = parsear_linea("ERR YA_ARMADA")
    assert mensaje == {"tipo": "error", "valor": "ya_armada"}


def test_parsear_listo():
    mensaje = parsear_linea("ALARMA UNO LISTA")
    assert mensaje == {"tipo": "listo", "valor": "uno"}


def test_linea_vacia():
    assert parsear_linea("   ") is None


def test_estado_invalido():
    assert parsear_linea("ESTADO PAUSA") is None


def test_comandos_hacia_el_uno():
    assert comando_armar() == "A"
    assert comando_desarmar() == "D"
    assert comando_consultar_estado() == "E"
