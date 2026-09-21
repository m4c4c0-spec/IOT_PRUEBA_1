"""
Aplicación web Flask para controlar el sistema de alarma.
Proporciona una interfaz móvil para armar/desarmar y recibir alertas.
"""

from flask import Flask, render_template, jsonify, request, send_from_directory
from alarma import SistemaAlarma
from sensores import MonitorSensores
import os

# Crear aplicación Flask
app = Flask(__name__, 
            static_folder='static',
            static_url_path='')

# Inicializar sistema de alarma
sistema = SistemaAlarma()
monitor = MonitorSensores(sistema)


@app.route('/')
def index():
    """Página principal de la aplicación."""
    return send_from_directory('static', 'index.html')


@app.route('/api/estado', methods=['GET'])
def obtener_estado():
    """Retorna el estado actual del sistema."""
    return jsonify({
        "alarma": sistema.obtener_estado(),
        "sensores": monitor.obtener_estado_sensores()
    })


@app.route('/api/armar', methods=['POST'])
def armar():
    """Arma la alarma."""
    if sistema.armar():
        # Limpiar sensores al armar
        monitor.sensor_pir.limpiar()
        return jsonify({"success": True, "mensaje": "Alarma armada correctamente"})
    return jsonify({"success": False, "mensaje": "No se pudo armar la alarma"}), 400


@app.route('/api/desarmar', methods=['POST'])
def desarmar():
    """Desarma la alarma."""
    if sistema.desarmar():
        # Limpiar sensores al desarmar
        monitor.sensor_pir.limpiar()
        monitor.sensor_puerta.simular_cierre()
        return jsonify({"success": True, "mensaje": "Alarma desarmada correctamente"})
    return jsonify({"success": False, "mensaje": "La alarma ya está desarmada"}), 400


@app.route('/api/historial', methods=['GET'])
def obtener_historial():
    """Retorna el historial de eventos."""
    limite = request.args.get('limite', 50, type=int)
    return jsonify(sistema.obtener_historial(limite))


@app.route('/api/simular/movimiento', methods=['POST'])
def simular_movimiento():
    """Simula detección de movimiento por el sensor PIR."""
    monitor.sensor_pir.simular_movimiento()
    intrusion_detectada = monitor.verificar_sensores()
    
    return jsonify({
        "success": True,
        "mensaje": "Movimiento simulado",
        "intrusion_detectada": intrusion_detectada,
        "estado": sistema.obtener_estado()
    })


@app.route('/api/simular/puerta', methods=['POST'])
def simular_puerta():
    """Simula apertura de puerta."""
    data = request.get_json() or {}
    abrir = data.get('abrir', True)
    
    if abrir:
        monitor.sensor_puerta.simular_apertura()
    else:
        monitor.sensor_puerta.simular_cierre()
    
    intrusion_detectada = monitor.verificar_sensores()
    
    return jsonify({
        "success": True,
        "mensaje": f"Puerta {'abierta' if abrir else 'cerrada'}",
        "intrusion_detectada": intrusion_detectada,
        "estado": sistema.obtener_estado()
    })


if __name__ == '__main__':
    print("=" * 50)
    print("Sistema de Alarma Antirrobo")
    print("=" * 50)
    print("\nAbriendo servidor web en http://localhost:8133")
    print("Presiona Ctrl+C para detener\n")
    
    # Puerto 8133 (133 es el número de emergencia en Chile)
    app.run(host='0.0.0.0', port=8133, debug=True, use_reloader=False)
