# TP C-Comenta

## Check point 2

### Objetivos cumplidos:
**Módulo Kernel:**
* ~~Es capaz de crear un PCB y planificarlo por FIFO~~ y RR. **(SOLO FIFO)**
* ~~Es capaz de enviar un proceso a la CPU para que sea procesado~~

**Módulo CPU:**
* ~~Se conecta a Kernel y recibe un PCB.~~ 
* ~~Es capaz de conectarse a la memoria y solicitar las instrucciones.~~
* ~~Es capaz de ejecutar un ciclo básico de instrucción.~~
* ~~Es capaz de resolver las operaciones: SET, SUM, SUB, JNZ e IO_GEN_SLEEP.~~
  
**Módulo Memoria:**
* ~~Se encuentra creado y acepta las conexiones.~~
* ~~Es capaz de abrir los archivos de pseudocódigo y envía las instrucciones al CPU.~~
  
**Módulo Interfaz I/O:**
* ~~Se encuentra desarrollada la Interfaz Genérica.~~

### Orden de ejecucion de los modulos
1. Memoria
2. CPU
3. Kernel
4. IO (Conexion dinámica en tiempo de ejecución)

### Operaciones admitidas por la Terminal:
+ INICIAR_PROCESO archivo_de_instrucciones.txt
+ INICIAR_PLANIFICACION ( ⚠️ Hay que arrancar manualmente la planificacion, arranca detenida )
+ DETENER_PLANIFICACION
+ PROCESO_ESTADO

### Operaciones admitidas por la CPU:
[ SET, SUM, SUB, JNZ, IO_GEN_SLEEP, EXIT ]


------

## Dependencias

Para poder compilar y ejecutar el proyecto, es necesario tener instalada la
biblioteca [so-commons-library] de la cátedra:

```bash
git clone https://github.com/sisoputnfrba/so-commons-library
cd so-commons-library
make debug
make install
```

## Compilación

Cada módulo del proyecto se compila de forma independiente a través de un
archivo `makefile`. Para compilar un módulo, es necesario ejecutar el comando
`make` desde la carpeta correspondiente.

El ejecutable resultante se guardará en la carpeta `bin` del módulo.

## Importar desde Visual Studio Code

Para importar el workspace, debemos abrir el archivo `tp.code-workspace` desde
la interfaz o ejecutando el siguiente comando desde la carpeta raíz del
repositorio:

```bash
code tp.code-workspace
```

## Checkpoint

Para cada checkpoint de control obligatorio, se debe crear un tag en el
repositorio con el siguiente formato:

```
checkpoint-{número}
```

Donde `{número}` es el número del checkpoint.

Para crear un tag y subirlo al repositorio, podemos utilizar los siguientes
comandos:

```bash
git tag -a checkpoint-{número} -m "Checkpoint {número}"
git push origin checkpoint-{número}
```

Asegúrense de que el código compila y cumple con los requisitos del checkpoint
antes de subir el tag.

## Entrega

Para desplegar el proyecto en una máquina Ubuntu Server, podemos utilizar el
script [so-deploy] de la cátedra:

```bash
git clone https://github.com/sisoputnfrba/so-deploy.git
cd so-deploy
./deploy.sh -r=release -p=utils -p=kernel -p=cpu -p=memoria -p=entradasalida "tp-{año}-{cuatri}-{grupo}"
```

El mismo se encargará de instalar las Commons, clonar el repositorio del grupo
y compilar el proyecto en la máquina remota.

Ante cualquier duda, podés consultar la documentación en el repositorio de
[so-deploy], o utilizar el comando `./deploy.sh -h`.

[so-commons-library]: https://github.com/sisoputnfrba/so-commons-library
[so-deploy]: https://github.com/sisoputnfrba/so-deploy
