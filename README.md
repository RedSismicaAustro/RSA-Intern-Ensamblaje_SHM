# RSA-Intern-Ensamblaje_SHM: Ensamblaje, Programación y Validación de Red de Monitorización de Salud Estructural (SHM)

Este repositorio contiene la documentación, esquemáticos, código fuente de firmware y espacio de entregables para el desarrollo del proyecto de pasantía enfocado en el **ensamblaje físico, programación, auditoría y validación analítica** de un sistema distribuido de Monitorización de Salud Estructural (SHM) orientado a edificaciones, puentes y presas.

---

## 📌 Objetivos del Proyecto

1. **Manufactura y Soldadura de Hardware**: Ensamblar y auditar 3 placas de circuito impreso (PCB):
   - **1 Nodo Concentrador** (basado en microcontrolador dsPIC33EP256MC202 y doble transceptor MAX485).
   - **2 Nodos Sensores** (dsPIC33EP256MC202 + Acelerómetro triaxial ADXL355 + Lector MicroSD + MAX485), evaluando una versión con plano de masa (GND) y una versión sin plano de masa.
2. **Desarrollo e Implementación de Firmware**: Adaptar y programar los firmware en **MikroC PRO for dsPIC** para la comunicación RS485, lectura de aceleración por SPI, almacenamiento local en MicroSD y captura de pulsos de sincronización dedicados (`INT_SINC_1`).
3. **Integración en Topología Daisy Chain**: Interconectar la red en cascada mediante cableado UTP con conectores RJ45 siguiendo la norma **T-568B**.
4. **Evaluación Experimental**: Medir retardo y latencias de sincronización mediante osciloscopio y comparar el nivel de ruido electromagnético entre la versión de nodo sensor con plano GND y la versión sin plano GND.

---

## 📁 Estructura del Repositorio

```text
RSA-Intern-Ensamblaje_SHM/
├── README.md                                           # Información general y guía del repositorio
├── docs/                                               # Documentación técnica y especificaciones
│   ├── Planificacion_Ensamblaje_Validacion_Red_Monitorizacion.pdf  # Plan de trabajo detallado de 96 horas
│   ├── Informe_diseño_pcbs_shm.pdf                     # Informe técnico detallado de diseño de PCBs
│   ├── Esquema_concentrador.pdf                         # Diagramas esquemáticos del Nodo Concentrador
│   ├── Esquema_nodo.pdf                                 # Diagramas esquemáticos del Nodo Sensor
│   └── Lista_de_Materiales_Sistema_Monitorizacion.pdf  # Lista de componentes (BOM) para ensamblaje
├── firmware/                                           # Código fuente y proyectos de firmware (MikroC)
│   ├── concentrador.c                                  # Firmware base del Nodo Concentrador (dsPIC33EP256MC202)
│   ├── NodoAcelerometro.c                              # Firmware base de los Nodos Sensores (ADXL355 + RS485)
│   └── concentrador/                                   # Subdirectorio para proyectos y compilaciones MikroC
└── results/                                            # Directorio de entregables del pasante
```

---

## 📄 Detalle del Contenido

### 1. Documentación (`/docs`)
- **`Planificacion_Ensamblaje_Validacion_Red_Monitorizacion.pdf`**: Plan de trabajo con cronograma de 96h distribuidas en 4 fases principales:
  - *Fase 1*: Ensamblaje e Inspección de Hardware (16 h).
  - *Fase 2*: Desarrollo y Adaptación de Firmware en MikroC (36 h).
  - *Fase 3*: Integración, Pruebas de Red y Evaluaciones Comparativas (24 h).
  - *Fase 4*: Documentación, Solución de Problemas y Reporte Final (20 h).
- **`Informe_diseño_pcbs_shm.pdf`**: Memoria técnica de diseño que describe la actualización a cableado Daisy Chain, intercambio de pines A/B del MAX485, adición del canal MAX485 dedicado a la sincronización e inclusión de 2 conectores RJ45 (J2 Entrada, J3 Salida) en los nodos sensores.
- **`Esquema_concentrador.pdf` & `Esquema_nodo.pdf`**: Planos eléctricos de circuito impreso para guiar la soldadura e inspección física.
- **`Lista_de_Materiales_Sistema_Monitorizacion.pdf`**: Bill of Materials (BOM) para verificación de componentes SMD y THT antes de soldar.

### 2. Firmware (`/firmware`)
- **`concentrador.c`**: Código en C para MikroC cargado en el dsPIC33EP256MC202 del concentrador. Gestiona la generación periódica del pulso de sincronización mediante interrupciones de temporizador a través del pin `INT_SINC_1` y controla el bus de datos RS485.
- **`NodoAcelerometro.c`**: Código en C para MikroC cargado en los nodos sensores. Maneja la interrupción externa proveniente del MAX485 receptor para captura inmediata de sincronía, lectura SPI del sensor triaxial ADXL355 y transmisión direccionada por RS485.

### 3. Resultados y Entregables (`/results`)
Este directorio se reservará para que el pasante suba los entregables requeridos al finalizar la pasantía:
- Capturas de pantalla de osciloscopio con mediciones del pulso de sincronización y latencias a lo largo de la cadena Daisy Chain.
- Tablas comparativas de nivel de ruido en las señales acelerométricas entre el nodo sensor con plano de masa y el nodo sensor sin plano de masa.
- Guía de solución de problemas (*Troubleshooting*) de ensamblaje, soldadura y firmware.
- Informe Final consolidado de la pasantía.

---

## 🔧 Requisitos del Entorno de Desarrollo

- **Firmware**: Compiler **MikroC PRO for dsPIC** v7.1.0 o superior.
- **Microcontroladores objetivo**: Microchip dsPIC33EP256MC202.
- **Programador/Depurador**: PICKit 3 / PICKit 4 o MPLAB IPE.
- **Hardware de Pruebas**: Osciloscopio digital de 2+ canales, multímetro digital, estación de soldadura con punta fina / aire caliente.
- **Análisis de Datos**: Python 3.x (bibliotecas recomendadas: `numpy`, `matplotlib`, `pandas`) para análisis de ruido y visualización de eventos acelerométricos.
