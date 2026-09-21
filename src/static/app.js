/**
 * Aplicación cliente para el sistema de alarma antirrobo
 */

// Estado de la aplicación
let estadoActual = 'desarmada';
let intervaloActualizacion = null;

// Elementos del DOM
const btnArmar = document.getElementById('btn-armar');
const btnDesarmar = document.getElementById('btn-desarmar');
const btnSimularMovimiento = document.getElementById('btn-simular-movimiento');
const btnSimularPuerta = document.getElementById('btn-simular-puerta');
const btnSilenciar = document.getElementById('btn-silenciar');
const btnActualizarHistorial = document.getElementById('btn-actualizar-historial');

const estadoBadge = document.getElementById('estado-badge');
const iconoEstado = document.getElementById('icono-estado');
const mensajeEstado = document.getElementById('mensaje-estado');
const panelAlerta = document.getElementById('panel-alerta');
const estadoPir = document.getElementById('estado-pir');
const estadoPuerta = document.getElementById('estado-puerta');
const historialEventos = document.getElementById('historial-eventos');

// Inicializar aplicación
document.addEventListener('DOMContentLoaded', () => {
    console.log('Aplicación de alarma iniciada');
    
    // Configurar event listeners
    btnArmar.addEventListener('click', armarAlarma);
    btnDesarmar.addEventListener('click', desarmarAlarma);
    btnSimularMovimiento.addEventListener('click', simularMovimiento);
    btnSimularPuerta.addEventListener('click', simularPuerta);
    btnSilenciar.addEventListener('click', silenciarAlerta);
    btnActualizarHistorial.addEventListener('click', cargarHistorial);
    
    // Cargar estado inicial
    actualizarEstado();
    cargarHistorial();
    
    // Actualizar estado cada 2 segundos
    intervaloActualizacion = setInterval(actualizarEstado, 2000);
});

/**
 * Obtiene el estado actual del sistema
 */
async function actualizarEstado() {
    try {
        const respuesta = await fetch('/api/estado');
        const datos = await respuesta.json();
        
        // Actualizar interfaz con el estado
        actualizarInterfaz(datos.alarma, datos.sensores);
        
    } catch (error) {
        console.error('Error al obtener estado:', error);
        mostrarMensaje('Error de conexión con el servidor', 'error');
    }
}

/**
 * Actualiza la interfaz con el estado actual
 */
function actualizarInterfaz(alarma, sensores) {
    const estado = alarma.estado;
    const alerta = alarma.alerta_activa;
    
    // Actualizar estado global
    estadoActual = estado;
    
    // Actualizar badge de estado
    estadoBadge.className = 'badge badge-' + estado;
    estadoBadge.textContent = estado.charAt(0).toUpperCase() + estado.slice(1);
    
    // Actualizar panel principal según estado
    switch (estado) {
        case 'desarmada':
            iconoEstado.textContent = '🔓';
            mensajeEstado.textContent = 'Sistema desarmado y seguro';
            btnArmar.disabled = false;
            btnDesarmar.disabled = true;
            panelAlerta.style.display = 'none';
            break;
            
        case 'armada':
            iconoEstado.textContent = '🔒';
            mensajeEstado.textContent = 'Sistema armado - Protegiendo su hogar';
            btnArmar.disabled = true;
            btnDesarmar.disabled = false;
            panelAlerta.style.display = 'none';
            break;
            
        case 'alerta':
            iconoEstado.textContent = '🚨';
            mensajeEstado.textContent = '¡ALERTA ACTIVADA!';
            btnArmar.disabled = true;
            btnDesarmar.disabled = false;
            
            // Mostrar panel de alerta
            if (alerta) {
                panelAlerta.style.display = 'block';
                
                // Hacer scroll al panel de alerta
                panelAlerta.scrollIntoView({ behavior: 'smooth', block: 'center' });
            }
            break;
    }
    
    // Actualizar estado de sensores
    actualizarEstadoSensores(sensores);
}

/**
 * Actualiza el estado visual de los sensores
 */
function actualizarEstadoSensores(sensores) {
    // Sensor PIR
    if (sensores.pir.movimiento_detectado) {
        estadoPir.textContent = '⚠️ Movimiento detectado';
        estadoPir.classList.add('activo');
    } else {
        estadoPir.textContent = 'Sin movimiento';
        estadoPir.classList.remove('activo');
    }
    
    // Sensor de puerta
    if (sensores.puerta.puerta_abierta) {
        estadoPuerta.textContent = '⚠️ Puerta abierta';
        estadoPuerta.classList.add('activo');
        btnSimularPuerta.textContent = 'Cerrar Puerta';
    } else {
        estadoPuerta.textContent = 'Puerta cerrada';
        estadoPuerta.classList.remove('activo');
        btnSimularPuerta.textContent = 'Abrir Puerta';
    }
}

/**
 * Arma la alarma
 */
async function armarAlarma() {
    try {
        const respuesta = await fetch('/api/armar', { method: 'POST' });
        const datos = await respuesta.json();
        
        if (datos.success) {
            mostrarMensaje('Alarma armada correctamente', 'exito');
            await actualizarEstado();
            await cargarHistorial();
        } else {
            mostrarMensaje(datos.mensaje, 'error');
        }
    } catch (error) {
        console.error('Error al armar:', error);
        mostrarMensaje('Error al armar la alarma', 'error');
    }
}

/**
 * Desarma la alarma
 */
async function desarmarAlarma() {
    try {
        const respuesta = await fetch('/api/desarmar', { method: 'POST' });
        const datos = await respuesta.json();
        
        if (datos.success) {
            mostrarMensaje('Alarma desarmada correctamente', 'exito');
            panelAlerta.style.display = 'none';
            await actualizarEstado();
            await cargarHistorial();
        } else {
            mostrarMensaje(datos.mensaje, 'error');
        }
    } catch (error) {
        console.error('Error al desarmar:', error);
        mostrarMensaje('Error al desarmar la alarma', 'error');
    }
}

/**
 * Simula detección de movimiento
 */
async function simularMovimiento() {
    try {
        btnSimularMovimiento.disabled = true;
        const respuesta = await fetch('/api/simular/movimiento', { method: 'POST' });
        const datos = await respuesta.json();
        
        mostrarMensaje('Movimiento simulado', 'info');
        
        if (datos.intrusion_detectada) {
            mostrarMensaje('¡Intrusión detectada!', 'alerta');
        }
        
        await actualizarEstado();
        await cargarHistorial();
        
        // Reactivar botón después de 1 segundo
        setTimeout(() => {
            btnSimularMovimiento.disabled = false;
        }, 1000);
        
    } catch (error) {
        console.error('Error al simular movimiento:', error);
        mostrarMensaje('Error al simular movimiento', 'error');
        btnSimularMovimiento.disabled = false;
    }
}

/**
 * Simula apertura/cierre de puerta
 */
async function simularPuerta() {
    try {
        btnSimularPuerta.disabled = true;
        
        // Determinar si abrir o cerrar según texto del botón
        const abrir = btnSimularPuerta.textContent.includes('Abrir');
        
        const respuesta = await fetch('/api/simular/puerta', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ abrir })
        });
        const datos = await respuesta.json();
        
        mostrarMensaje(datos.mensaje, 'info');
        
        if (datos.intrusion_detectada) {
            mostrarMensaje('¡Intrusión detectada!', 'alerta');
        }
        
        await actualizarEstado();
        await cargarHistorial();
        
        // Reactivar botón después de 500ms
        setTimeout(() => {
            btnSimularPuerta.disabled = false;
        }, 500);
        
    } catch (error) {
        console.error('Error al simular puerta:', error);
        mostrarMensaje('Error al simular puerta', 'error');
        btnSimularPuerta.disabled = false;
    }
}

/**
 * Silencia la alerta (desarma la alarma)
 */
function silenciarAlerta() {
    desarmarAlarma();
}

/**
 * Carga el historial de eventos
 */
async function cargarHistorial() {
    try {
        const respuesta = await fetch('/api/historial?limite=20');
        const eventos = await respuesta.json();
        
        if (eventos.length === 0) {
            historialEventos.innerHTML = '<p class="texto-placeholder">No hay eventos recientes</p>';
            return;
        }
        
        // Renderizar eventos (más recientes primero)
        historialEventos.innerHTML = eventos
            .reverse()
            .map(evento => {
                const fecha = new Date(evento.timestamp);
                const horaFormateada = fecha.toLocaleTimeString('es-CL');
                
                return `
                    <div class="evento-item">
                        <span class="evento-tipo ${evento.tipo}">${evento.tipo.toUpperCase()}</span>
                        ${evento.mensaje}
                        <div class="evento-timestamp">${horaFormateada}</div>
                    </div>
                `;
            })
            .join('');
            
    } catch (error) {
        console.error('Error al cargar historial:', error);
        historialEventos.innerHTML = '<p class="texto-placeholder">Error al cargar eventos</p>';
    }
}

/**
 * Muestra un mensaje temporal al usuario
 */
function mostrarMensaje(mensaje, tipo) {
    console.log(`[${tipo.toUpperCase()}] ${mensaje}`);
    
    // TODO: Implementar notificaciones visuales más elegantes
    // Por ahora solo usa console.log
}
