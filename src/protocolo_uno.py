"""
Protocolo de texto entre Arduino UNO y el computador.

El firmware usa el mismo esquema que el semáforo del curso:
comandos de un carácter por USB o HC-05 (9600 baud).

  A = armar, D = desarmar, E = consultar estado

Las respuestas siguen siendo líneas de texto (ESTADO ARMADA, EVENTO PIR, ...).
Este módulo se puede probar sin la placa.
"""

from typing import Dict, Optional


ESTADOS_VALIDOS = {"desarmada", "armada", "alerta"}


def parsear_linea(linea: str) -> Optional[Dict[str, str]]:
    """
    Interpreta una línea enviada por el firmware del UNO.

    Ejemplos:
    - "ESTADO ARMADA" -> {"tipo": "estado", "valor": "armada"}
    - "EVENTO PIR" -> {"tipo": "evento", "valor": "pir"}
    - "ALERTA INTRUSION" -> {"tipo": "alerta", "valor": "intrusion"}
    - "OK ARMAR" -> {"tipo": "ok", "valor": "armar"}
    """
    texto = linea.strip().upper()
    if not texto:
        return None

    partes = texto.split()
    comando = partes[0]
    argumento = partes[1].lower() if len(partes) > 1 else ""

    if comando == "ESTADO":
        if argumento not in ESTADOS_VALIDOS:
            return None
        return {"tipo": "estado", "valor": argumento}

    if comando == "EVENTO":
        if argumento not in {"pir", "puerta"}:
            return None
        return {"tipo": "evento", "valor": argumento}

    if comando == "ALERTA":
        return {"tipo": "alerta", "valor": argumento or "intrusion"}

    if comando == "OK":
        return {"tipo": "ok", "valor": argumento}

    if comando == "ERR":
        return {"tipo": "error", "valor": argumento or "desconocido"}

    if texto == "ALARMA UNO LISTA":
        return {"tipo": "listo", "valor": "uno"}

    return None


def comando_armar() -> str:
    """Carácter que el PC o el celular envían al UNO para armar (igual que el semáforo)."""
    return "A"


def comando_desarmar() -> str:
    """Carácter que el PC o el celular envían al UNO para desarmar."""
    return "D"


def comando_consultar_estado() -> str:
    """Carácter que el PC envía al UNO para pedir el estado."""
    return "E"
