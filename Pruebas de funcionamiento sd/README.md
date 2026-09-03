# Pruebas de funcionamiento de tarjeta SD por SPI

Firmware de prueba para un nodo sensor basado en el microcontrolador **dsPIC33EP256MC202**. El proyecto comprueba el funcionamiento del microcontrolador, los pines de la placa y la comunicación con una tarjeta SD mediante el periférico **SPI1**.

El código está preparado para compilarse en **mikroC PRO for dsPIC**.

## Contenido del repositorio

| Archivo | Descripción |
| --- | --- |
| `nodo1_v2.c` | Firmware principal recomendado para probar inicialización, escritura y lectura de la SD. Incluye configuración PPS en código y diagnóstico mediante LED. |
| `nodo1.c` | Primera versión del firmware principal. Conserva la configuración PPS original y tiempos de diagnóstico distintos. |
| `sdcard.c` | Librería de bajo nivel para inicializar la SD y leer o escribir bloques de 512 bytes. |
| `sdcard.h` | Comandos SPI de la SD, códigos de estado, estructura de banderas y prototipos. |
| `spiSD.c` | Capa de comunicación SPI1. |
| `spiSD.h` | Prototipos y constantes `SLOW`/`FAST` de la capa SPI. |
| `test_blink.c` | Prueba mínima del reloj, grabación del microcontrolador y LED en RA2. No usa la SD. |
| `test_pines.c` | Comprueba la configuración de pines del nodo y las salidas principales. No usa la SD. |
| `test_sin_sd.c` | Comprueba el flujo principal sin ejecutar la librería de la tarjeta SD. |
| `Esquema_nodo.pdf` | Esquema eléctrico del nodo, si se desea consultar junto con el código. |
| `InformeFinal_JhonatanCambisaca.pdf` | Informe del proyecto. |
| `Informe_Actividad__Prácticas_Laborales.pdf` | Documento complementario del proyecto. |

## Hardware

- dsPIC33EP256MC202.
- Tarjeta o módulo SD conectado en modo SPI.
- LED de diagnóstico conectado a RA2.
- Alimentación compatible con el microcontrolador y la tarjeta SD.
- Adaptación de niveles de tensión si el módulo SD no es compatible con las señales del dsPIC.

### Asignación de pines

| Señal | Pin del dsPIC | Función |
| --- | --- | --- |
| `TEST` | RA2 | LED de diagnóstico |
| `CsADXL` | RA3 | Chip Select del acelerómetro, inactivo en alto |
| `sd_CS` | RB0 | Chip Select de la tarjeta SD |
| `SCK1` | RB7 / RP39 | Reloj SPI hacia la SD |
| `SDO1` / MOSI | RB8 / RP40 | Datos del dsPIC hacia la SD |
| `SDI1` / MISO | RB9 / RP41 | Datos de la SD hacia el dsPIC |
| `Card Detect` | RA4 | Detección de tarjeta; en el firmware principal se fuerza `detected = true` |
| `MSRS485` | RB12 | Control de dirección del transceptor RS485 |

La tarjeta SD debe trabajar a **3.3 V**. `sd_CS` permanece en nivel alto cuando la tarjeta no está seleccionada.

## Configuración en mikroC

1. Crear un proyecto nuevo para el dispositivo `dsPIC33EP256MC202`.
2. Seleccionar la frecuencia del proyecto de acuerdo con el reloj configurado en el código, aproximadamente **80 MHz**.
3. Para el firmware principal, agregar únicamente:
   - `nodo1_v2.c` o `nodo1.c` como archivo con `main()`.
   - `spiSD.c` y `spiSD.h`.
   - `sdcard.c` y `sdcard.h`.
4. Configurar o comprobar el mapeo PPS de SPI1:
   - SCK1 en RB7.
   - SDO1 en RB8.
   - SDI1 en RB9.
5. Compilar y grabar el firmware.

`nodo1_v2.c` configura PPS desde el código. En `nodo1.c`, revisar la versión del compilador y del dispositivo porque el código usa escritura directa de registros PPS.

## Orden recomendado de pruebas

### 1. `test_blink.c`

Agregar solamente este archivo al proyecto. El LED de RA2 debe parpadear aproximadamente cada 300 ms. Esta prueba confirma que el microcontrolador se graba, arranca y controla el LED.

### 2. `test_pines.c`

Agregar solamente este archivo. El resultado esperado es:

- Cinco parpadeos rápidos al arrancar.
- Después, parpadeo continuo de un segundo encendido y un segundo apagado.

Esta prueba comprueba el reloj, las direcciones de los pines y los estados iniciales de CS.

### 3. `test_sin_sd.c`

Agregar solamente este archivo y retirar `sdcard.c` y `spiSD.c` del proyecto. El resultado esperado es:

- Tres parpadeos rápidos al arrancar.
- Después, parpadeo continuo de 500 ms encendido y 500 ms apagado.

Si esta prueba funciona, el problema probablemente está en la inicialización o comunicación SPI con la SD.

### 4. `nodo1_v2.c`

Agregar el firmware principal y las dos librerías. La tarjeta se inicializa con hasta diez intentos. Después se realizan estas operaciones:

1. Escribir un patrón de 512 bytes en los sectores 2500, 100, 1 y 4.
2. Leer el sector 2500.
3. Escribir los datos leídos en el sector 2501.

## Diagnóstico mediante LED

El LED conectado a RA2 indica el resultado del proceso:

| Patrón | Significado |
| --- | --- |
| Tres parpadeos rápidos al arrancar | El programa comenzó a ejecutarse |
| LED fijo durante la inicialización | La SD fue inicializada correctamente |
| Parpadeo lento continuo | Fallo de inicialización de la SD |
| Un parpadeo largo | Escritura correcta |
| Dos parpadeos largos | Lectura correcta |
| Tres parpadeos largos | Escritura, lectura y reescritura correctas |
| Parpadeo muy rápido | Error de lectura o escritura |

En la versión `nodo1.c`, el número de parpadeos de error de inicialización identifica el fallo:

- 1: tarjeta no insertada o `CMD0` sin respuesta.
- 2: tarjeta no lista.
- 3: tarjeta no utilizable.
- 4: error de eco de `CMD8`.
- 5: voltaje incompatible.
- 6: otro error de inicialización.

## Funcionamiento de la librería SD

La librería utiliza el protocolo SPI y bloques de **512 bytes**. Durante la inicialización, el SPI comienza a baja velocidad y, si la tarjeta responde correctamente, cambia a la velocidad rápida definida en `spiSD.c`.

Funciones principales:

```c
unsigned char SD_Init_Try(unsigned char try_value);
unsigned char SD_Detect(void);
unsigned char SD_Read_Block(unsigned char *Buffer, unsigned long Address);
unsigned char SD_Write_Block(unsigned char *Buffer, unsigned long Address);
```

`SD_Read_Block` y `SD_Write_Block` reciben direcciones de sector y buffers de 512 bytes. Los resultados se comparan con las constantes de `sdcard.h`, especialmente `SUCCESSFUL_INIT` y `DATA_ACCEPTED`.

## Advertencias

- El firmware escribe directamente sectores físicos de la SD; no crea archivos FAT32. Una escritura puede sobrescribir información existente.
- Antes de probar, usar una tarjeta dedicada y realizar una copia de seguridad.
- Los sectores escritos pueden inspeccionarse con una herramienta de acceso hexadecimal como HxD.
- El código está pensado para tarjetas SD con bloques de 512 bytes y contempla direccionamiento SDSC/SDHC mediante el bit CCS.
- El socket utilizado no dispone de una señal física de Card Detect. Por eso `nodo1_v2.c` fuerza `sdflags.detected = true` y usa una resistencia pull-up interna en RA4.
- Revisar el valor de `DELTASECTOR`, `SIZESD` y los sectores de prueba antes de conectarlo a una tarjeta con datos importantes.
- En `nodo1_v2.c` conviene revisar que la variable `sector` tenga el valor `2500` antes de llamar a `SD_Read_Block`; la versión original sí realiza esa asignación explícitamente.
- Verificar en el manual del dsPIC y en mikroC los registros PPS usados por la versión del compilador instalada.

## Licencia

No se ha definido una licencia para este repositorio. Añade una licencia antes de publicar el código si deseas establecer formalmente las condiciones de uso y distribución.
