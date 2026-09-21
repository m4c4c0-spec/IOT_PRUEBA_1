#!/usr/bin/env python3
"""
Puente USB entre Arduino UNO y la app web.

OPCIONAL. No se usa en la primera sesión.
El UNO habla por USB o HC-05 con comandos de un carácter (A, D, E),
igual que el semáforo del curso. Este script lee el USB y llama a la API local.

Uso (con python3 run.py ya en marcha y Monitor Serie cerrado):

    pip install pyserial
    python3 firmware/puente_serial.py
    python3 firmware/puente_serial.py --puerto COM3
"""

import argparse
import json
import os
import sys
import time
import urllib.error
import urllib.request

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    serial = None

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))
from protocolo_uno import (  # noqa: E402
    parsear_linea,
    comando_armar,
    comando_desarmar,
    comando_consultar_estado,
)

URL_BASE = "http://127.0.0.1:8133"


def exigir_pyserial():
    if serial is None:
        print("Falta pyserial. Instálalo solo para este puente: pip install pyserial")
        sys.exit(1)


def llamar_api(ruta, metodo="GET", cuerpo=None):
    datos = None
    headers = {}
    if cuerpo is not None:
        datos = json.dumps(cuerpo).encode("utf-8")
        headers["Content-Type"] = "application/json"

    pedido = urllib.request.Request(
        URL_BASE + ruta,
        data=datos,
        headers=headers,
        method=metodo,
    )
    with urllib.request.urlopen(pedido, timeout=3) as respuesta:
        return json.loads(respuesta.read().decode("utf-8"))


def detectar_puerto():
    exigir_pyserial()
    puertos = list(serial.tools.list_ports.comports())
    if not puertos:
        print("No hay puertos serie. Conecta el Arduino UNO por USB.")
        sys.exit(1)

    for puerto in puertos:
        texto = (puerto.description or "") + " " + (puerto.device or "")
        if "Arduino" in texto or "CH340" in texto or "USB" in texto or "ACM" in texto:
            return puerto.device

    return puertos[0].device


def procesar_mensaje(mensaje, serial_uno):
    tipo = mensaje["tipo"]
    valor = mensaje["valor"]

    if tipo == "evento" and valor == "pir":
        llamar_api("/api/simular/movimiento", "POST")
        print("PIR del UNO → alerta en la app")
        return

    if tipo == "evento" and valor == "puerta":
        llamar_api("/api/simular/puerta", "POST", {"abrir": True})
        print("Puerta del UNO → alerta en la app")
        return

    if tipo == "estado":
        print("Estado en el UNO:", valor)
        return

    if tipo == "listo":
        print("Arduino UNO conectado")
        serial_uno.write(comando_consultar_estado().encode("ascii"))


def sincronizar_app_hacia_uno(estado_anterior, serial_uno):
    try:
        datos = llamar_api("/api/estado")
    except (urllib.error.URLError, TimeoutError, json.JSONDecodeError):
        return estado_anterior

    estado = datos["alarma"]["estado"]
    if estado == estado_anterior:
        return estado_anterior

    if estado == "armada" and estado_anterior != "armada":
        serial_uno.write(comando_armar().encode("ascii"))
    elif estado == "desarmada" and estado_anterior != "desarmada":
        serial_uno.write(comando_desarmar().encode("ascii"))

    return estado


def main():
    parser = argparse.ArgumentParser(description="Puente Arduino UNO ↔ app de alarma")
    parser.add_argument("--puerto", help="Ej: COM3 o /dev/ttyACM0")
    parser.add_argument("--baud", type=int, default=9600)
    args = parser.parse_args()

    exigir_pyserial()

    puerto = args.puerto or detectar_puerto()
    print("Abriendo", puerto, "a", args.baud, "baud")
    print("Deja python3 run.py en marcha y cierra el Monitor Serie.")

    try:
        llamar_api("/api/estado")
    except (urllib.error.URLError, TimeoutError):
        print("No se pudo hablar con http://127.0.0.1:8133")
        print("Primero ejecuta: python3 run.py")
        sys.exit(1)

    serial_uno = serial.Serial(puerto, args.baud, timeout=0.2)
    estado_app = None
    buffer = ""

    try:
        while True:
            leido = serial_uno.read(128).decode("ascii", errors="ignore")
            if leido:
                buffer += leido
                while "\n" in buffer:
                    linea, buffer = buffer.split("\n", 1)
                    mensaje = parsear_linea(linea)
                    if mensaje:
                        procesar_mensaje(mensaje, serial_uno)

            estado_app = sincronizar_app_hacia_uno(estado_app, serial_uno)
            time.sleep(0.1)
    except KeyboardInterrupt:
        print("\nPuente cerrado")
    finally:
        serial_uno.close()


if __name__ == "__main__":
    main()
