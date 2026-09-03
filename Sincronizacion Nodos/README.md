# Sistema de monitoreo de salud estructural

Firmware para una red de dos nodos sensores y un concentrador basada en un
`dsPIC33EP256MC202`. El concentrador se comunica con una Raspberry Pi por SPI
y con los nodos por RS485.

Este README documenta exclusivamente:

- `nodo1.c`
- `nodo2.c`
- `Concentrador.c`

## Arquitectura

```text
Raspberry Pi <-- SPI1 --> Concentrador <-- RS485 --> Nodo 1 (ID 1)
                                      \-----------> Nodo 2 (ID 2)
                                          |
                                          +--> Pulso INT_SINC por MAX485 dedicado
```

El concentrador es el maestro del bus RS485. Los nodos permanecen escuchando,
aceptan tramas dirigidas a su dirección o a broadcast y responden únicamente
cuando corresponde.

## Archivos principales

| Archivo | Funcion |
| --- | --- |
| [`nodo1.c`](nodo1.c) | Firmware del nodo sensor con direccion RS485 `1`. |
| [`nodo2.c`](nodo2.c) | Firmware del nodo sensor con direccion RS485 `2`. Es funcionalmente igual a `nodo1.c`, pero cambia `IDNODO`. |
| [`Concentrador.c`](Concentrador.c) | Coordina la Raspberry Pi, el bus RS485 y la sincronizacion de los nodos. |

Los tres programas incluyen [`rs485.c`](rs485.c), que contiene las constantes y
la rutina comun `EnviarTramaRS485`. Este archivo es una dependencia de
compilacion, no un firmware independiente documentado en este README.

## Hardware y configuracion

### Microcontrolador

- `dsPIC33EP256MC202`
- Reloj interno FRC con PLL, aproximadamente `80 MHz`
- Sin cristal externo
- Pines analogicos configurados como digitales
- Velocidad UART RS485: `2,000,000 baudios`
- Formato UART: 8 bits, sin paridad, 1 bit de parada

### Nodos

Cada nodo utiliza:

- UART1 para el bus RS485 de datos.
- MAX485 bidireccional para transmitir y recibir datos.
- Un segundo receptor MAX485 permanente para capturar `INT_SINC` mediante
  `INT1` en `RB14`.
- `RA2` / `TEST1` como indicador visual del pulso de sincronizacion.
- `RB12` para controlar `DE/RE` del MAX485 de datos.

La direccion se selecciona en el codigo:

```c
#define IDNODO 1  // nodo1.c
#define IDNODO 2  // nodo2.c
```

No deben programarse dos nodos con la misma direccion si van a responder en el
mismo bus.

### Concentrador

- UART2 para el bus RS485 de datos.
- SPI1 en modo esclavo para la Raspberry Pi.
- MAX485 bidireccional para RS485, controlado desde `RB11`.
- Segundo MAX485 configurado como transmisor permanente para `INT_SINC`.
- `RA0` genera el pulso de sincronizacion.
- `RA1` / `INT_SINC` es el heartbeat de sincronizacion (`D5`).
- `RA3` / `INT_SINC_2` indica trafico RS485 valido (`D2`).
- `RA4` / `RP1` genera una interrupcion hacia la Raspberry Pi.

Los bits de configuracion deben usar `FRCPLL` como fuente del oscilador y
`OSC2` como I/O para poder utilizar `RA3`.

## Protocolo RS485

Todas las tramas tienen este formato:

```text
[0x3A] [Direccion] [Funcion] [numDatos LSB] [numDatos MSB] [Payload...]
```

- `0x3A`: inicio de trama.
- `Direccion`: `1` para Nodo 1, `2` para Nodo 2 y `255` para broadcast.
- `Funcion`: codigo de operacion.
- `numDatos`: cantidad de bytes del payload, en little-endian.
- `Payload`: datos asociados a la funcion.

### Respuesta de prueba de los nodos

Cuando un nodo recibe una trama con:

```text
Funcion = 0xF1
Payload[0] = 0xD2
```

responde con:

```text
Funcion = 0xF1
Payload = [0xD2, IDNODO]
```

Esto permite comprobar el enlace bidireccional y confirmar que respondio el
nodo esperado.

## Comandos SPI del concentrador

La Raspberry Pi inicia las operaciones enviando un comando y finalizandolo con
su funcion de cierre. Los comandos implementados en `Concentrador.c` son:

| Inicio | Fin | Operacion |
| --- | --- | --- |
| `0xA0` | `0xF0` | Operacion generica y lectura de una respuesta pendiente. |
| `0xA1` | `0xF1` | Iniciar muestreo en un nodo. Recibe direccion y bandera de sobrescritura de SD. |
| `0xA2` | `0xF2` | Detener muestreo en un nodo. |
| `0xA7` | `0xF7` | Solicitar el estado de un nodo. |
| `0xA8` | `0xF8` | Reenviar una instruccion generica por RS485. |
| `0xAA` | `0xFA` | Leer desde SPI el payload recibido por RS485. |

Las respuestas hacia la Raspberry Pi se anuncian mediante un pulso de `20 us`
en `RP1`.

## Sincronizacion

El concentrador genera un pulso `INT_SINC` cada `1 s`:

- `TMR3` mantiene la periodicidad.
- `RA0` (`INT_SINC_1`) produce un pulso de `1 ms` hacia el MAX485 dedicado.
- Cada nodo captura el pulso con `INT1` y conmuta su LED `TEST1`.
- El LED `INT_SINC` del concentrador tambien conmuta como heartbeat.

Si la red esta conectada correctamente, los LEDs de heartbeat del concentrador
y de los nodos deben cambiar de estado en fase.

## Timeout y descarte de tramas

- El concentrador espera `300 ms` una respuesta individual por RS485.
- Si vence el timeout, envia a la Raspberry Pi el payload de error:
  `0xD3 0xEE 0xE4`.
- Un nodo descarta las tramas dirigidas a otro nodo para evitar colisiones.
- Despues de descartar una trama, el nodo deshabilita temporalmente UART1 y lo
  recupera mediante `TMR2`.

## Compilacion

1. Abrir el proyecto correspondiente en **mikroC PRO for dsPIC**.
2. Seleccionar el dispositivo `dsPIC33EP256MC202`.
3. Verificar los bits de configuracion del oscilador indicados arriba.
4. Mantener `rs485.c` en la misma carpeta para resolver el `#include`.
5. Compilar y programar cada firmware en su placa.

Antes de conectar el bus completo, se recomienda validar primero el eco
`0xF1/0xD2` y luego comprobar la sincronizacion visual mediante los LEDs.

## Estado actual

Los firmwares de los nodos estan orientados a validar el bus RS485 y la cadena
de sincronizacion Daisy Chain. La logica principal de los nodos permanece en
espera y atiende las tramas mediante interrupciones.