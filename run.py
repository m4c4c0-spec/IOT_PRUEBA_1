#!/usr/bin/env python3
"""
Script principal para ejecutar el sistema de alarma antirrobo.
Uso: python run.py
"""

import sys
import os

# Agregar directorio src al path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

# Importar y ejecutar la aplicación
from app import app

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8133, debug=True)
