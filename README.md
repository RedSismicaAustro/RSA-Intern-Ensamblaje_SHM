# RSA-Intern-Ensamblaje_SHM

Repositorio de ensamblaje, firmware y validacion de una red distribuida de Monitorizacion de Salud Estructural (SHM). El sistema utiliza microcontroladores dsPIC33EP256MC202, comunicacion RS485, sensores acelerometricos ADXL355, sincronizacion por pulsos y almacenamiento directo en tarjeta MicroSD.

## Arquitectura general

```text
Raspberry Pi
    |
   SPI1
    |
Concentrador ---- RS485 de datos ---- Nodo sensor 1 ---- Nodo sensor 2
    |                                    |                  |
    +-- pulso INT_SINC por MAX485 ------+------------------+
                                         |
                              ADXL355 + MicroSD
```

El concentrador coordina la red y recibe instrucciones desde la Raspberry Pi. Los nodos responden a tramas dirigidas a su direccion, adquieren aceleracion y pueden guardar datos en sectores fisicos de la MicroSD.

## Estructura del repositorio

| Ruta | Contenido |
| --- | --- |
| `docs/` | Esquemas electricos, lista de materiales, planificacion e informes tecnicos. |
| `firmware/NodoAcelerometro.c` | Firmware base del nodo sensor con ADXL355, RS485 y almacenamiento. |
| `firmware/concentrador.c` | Firmware base del concentrador. |
| `Sincronizacion Nodos/` | Version integrada para concentrador, nodo 1, nodo 2 y libreria RS485, junto con proyectos de mikroC. |
| `Main_SD/` | Proyecto principal de prueba de tarjeta SD, controlador de bloques y configuracion SPI. |
| `Pruebas de funcionamiento sd/` | Pruebas aisladas de pines, LED, comunicacion SPI y tarjeta SD. |

Los archivos `*.asm`, `*.hex`, `*.lst`, `*.dbg`, `*.dlt`, `*.mcl`, `*.cp` y similares son salidas o archivos auxiliares generados por mikroC. Los archivos fuente principales son `*.c` y `*.h`.

## Firmware de la red

### Concentrador

`Sincronizacion Nodos/Concentrador.c` y `firmware/concentrador.c` implementan el equipo maestro:

- UART2 para el bus RS485.
- SPI1 esclavo para comunicarse con la Raspberry Pi.
- Generacion de `INT_SINC` mediante TMR3.
- Pulso de sincronizacion de aproximadamente 1 ms cada 1 s.
- Aviso a la Raspberry Pi mediante una interrupcion dedicada.
- Timeout de aproximadamente 300 ms para respuestas de nodos.

El concentrador usa `RA0` para generar el pulso de sincronizacion, `RA1` como indicador, `RA3` para trafico RS485 y `RA4/RP1` para avisar a la Raspberry Pi.

### Nodos sensores

`Sincronizacion Nodos/nodo1.c` y `Sincronizacion Nodos/nodo2.c` son funcionalmente equivalentes. Se diferencian por su direccion:

```c
#define IDNODO 1   /* nodo1.c */
#define IDNODO 2   /* nodo2.c */
```

Cada nodo utiliza:

- UART1 para RS485 de datos.
- `RB12` para controlar `DE/RE` del MAX485.
- `RB14/INT1` para capturar el pulso de sincronizacion.
- `RA2/TEST1` como indicador de sincronizacion.
- ADXL355 mediante SPI en el firmware del sensor.
- MicroSD para almacenamiento local cuando se integra esa funcionalidad.

No deben programarse dos nodos con el mismo `IDNODO` dentro de la misma red.

### Parametros electricos y de frecuencia

- Microcontrolador: `dsPIC33EP256MC202`.
- Reloj del proyecto: aproximadamente `80 MHz`, equivalente a `40 MIPS` segun la configuracion usada.
- UART RS485: `2,000,000 baudios`, 8 bits, sin paridad y 1 bit de parada.
- La alimentacion de la tarjeta SD debe ser de `3.3 V`.
- Verificar adaptacion de niveles cuando el modulo SD no sea compatible con el dsPIC.

## Protocolo RS485

Las tramas tienen el siguiente formato:

```text
[0x3A] [Direccion] [Funcion] [Longitud LSB] [Longitud MSB] [Payload]
```

- `0x3A`: inicio de trama.
- `Direccion`: `1`, `2` o `255` para broadcast.
- `Funcion`: codigo de la operacion.
- `Longitud`: cantidad de bytes del payload en little-endian.
- `Payload`: datos de la operacion.

La prueba de enlace usa la funcion `0xF1` con payload `0xD2`. El nodo responde con `0xD2` y su `IDNODO`.

Los comandos principales entre Raspberry Pi y concentrador son:

| Inicio | Fin | Funcion |
| --- | --- | --- |
| `0xA0` | `0xF0` | Operacion generica o lectura de respuesta pendiente. |
| `0xA1` | `0xF1` | Iniciar muestreo en un nodo. |
| `0xA2` | `0xF2` | Detener muestreo. |
| `0xA7` | `0xF7` | Solicitar estado. |
| `0xA8` | `0xF8` | Reenviar instruccion por RS485. |
| `0xAA` | `0xFA` | Leer payload recibido por RS485. |

`Sincronizacion Nodos/rs485.c` contiene la rutina comun `EnviarTramaRS485` y las definiciones compartidas por los tres firmwares.

## Sincronizacion de nodos

El concentrador genera un pulso periodico con `TMR3`. El pulso se envia por un MAX485 dedicado y cada nodo lo captura mediante `INT1`. Los LED de heartbeat del concentrador y de los nodos permiten comprobar visualmente que la cadena esta sincronizada y que el pulso llega a todos los equipos.

Antes de conectar toda la red se recomienda comprobar, en este orden:

1. Alimentacion y direcciones unicas de los nodos.
2. Eco RS485 `0xF1/0xD2` con un nodo a la vez.
3. Continuidad del bus Daisy Chain y terminacion.
4. Pulso `INT_SINC` en el concentrador y en cada nodo con osciloscopio.
5. Lectura del ADXL355 y almacenamiento en SD.

## Tarjeta MicroSD

El proyecto `Main_SD/` accede directamente a sectores fisicos de 512 bytes. No crea archivos FAT32 ni directorios. Una escritura puede sobrescribir datos existentes, por lo que se debe usar una tarjeta de prueba o una copia de seguridad.

### Pines SPI1 de la SD

| Senal | dsPIC |
| --- | --- |
| `SCK1` | `RB7/RP39` |
| `SDO1/MOSI` | `RB8/RP40` |
| `SDI1/MISO` | `RB9/RP41` |
| `CS` | `RB0`, activo en bajo |
| LED de prueba | `RA2` |
| Card Detect | `RA4`, no disponible en la placa actual |

La inicializacion comienza a aproximadamente `625 kHz` y, despues de recibir una respuesta valida, cambia a aproximadamente `2.5 MHz`. La libreria distingue tarjetas SDSC y SDHC/SDXC mediante el bit CCS de OCR y ajusta la direccion de bloque cuando corresponde.

### Prueba del sector 2500

`Main_SD/test_sd_sector2500.c` es una prueba aislada para mikroC. No consulta el pin de deteccion, no lee sectores y solo ejecuta `SD_Write_Block` sobre el sector `2500`. El buffer contiene la secuencia `00 01 02 ... FF` repetida hasta completar 512 bytes.

Para compilarla:

1. Crear o abrir un proyecto para `P33EP256MC202`.
2. Usar `test_sd_sector2500.c` como unico archivo con `main()`.
3. Agregar `sdcard.c`, `sdcard.h`, `spiSD.c` y `spiSD.h`.
4. Configurar el reloj del proyecto a aproximadamente `80 MHz`.
5. Mantener el SPI lento durante la inicializacion y rapido despues, tal como define `spiSD.c`.
6. Programar la placa y comprobar el sector `2500` con una herramienta de lectura directa de sectores.

En mikroC, `Delay_ms()` requiere un valor constante. Por eso la prueba utiliza retardos literales como `Delay_ms(1000)` y `Delay_ms(250)`, no una variable como argumento.

### Pruebas SD adicionales

La carpeta `Pruebas de funcionamiento sd/` contiene:

- `test_blink.c`: prueba basica de grabacion y LED.
- `test_pines.c`: prueba de direcciones y estados de pines.
- `test_sin_sd.c`: prueba del programa sin inicializar la libreria SD.
- `nodo1.c` y `nodo1_v2.c`: variantes de prueba de inicializacion, escritura y lectura.
- `sdcard.c/.h` y `spiSD.c/.h`: copias de las librerias necesarias para esos proyectos.

## Compilacion general

1. Instalar `MikroC PRO for dsPIC`.
2. Seleccionar el dispositivo `P33EP256MC202`.
3. Confirmar el reloj de aproximadamente `80 MHz` y los bits de configuracion del oscilador.
4. Mantener cada archivo con `main()` en un proyecto separado.
5. Incluir las librerias `.c` y `.h` requeridas por ese firmware.
6. Verificar el mapeo PPS y las conexiones fisicas antes de programar.

El proyecto no se puede compilar como un unico programa porque existen varios puntos de entrada `main()` para distintas placas y pruebas.

## Documentacion tecnica

En `docs/` se encuentran los esquemas del concentrador y del nodo, la lista de materiales, el plan de ensamblaje y validacion, y el informe de diseno de las PCBs. Estos documentos deben consultarse junto con las tablas de pines antes de soldar o conectar la red.

## Advertencias

- No conectar una tarjeta SD con datos importantes durante las pruebas de escritura directa.
- No utilizar simultaneamente dos firmwares con el mismo bus o la misma direccion de nodo.
- Comprobar niveles de tension, masa comun, CS en alto durante reposo y continuidad de SCK/MOSI/MISO.
- Las pruebas LED identifican etapas de ejecucion, pero no sustituyen las mediciones con osciloscopio.
- Los archivos generados por mikroC pueden depender de rutas locales de Windows y no son necesarios para entender el codigo fuente.
