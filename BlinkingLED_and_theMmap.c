#define BCM2835_PERI_BASE        0x3F000000
#define GPIO_BASE                (BCM2835_PERI_BASE + 0x200000) /* Controlador GPIO */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>	//Funciones sleep(...) y close(...)

#define PAGE_SIZE (4*1024)//Pagina de memoria: 1024
#define BLOCK_SIZE (4*1024)//Bloques de memoria: 1024

int  mem_fd; //fd - File descriptor
void *gpio_map;//Apntador void puede apuntar a lo que sea

// I/O access
volatile unsigned *gpio;

void setup_io();

int main(int argc, char **argv) {
  int g,rep;
  int fselGPIO21;	//Utilizaremos un LED conectado a la terminal 21
  int fsetGPIO21;
  int fclrGPIO21;

  // Set up gpi pointer for direct register access
  setup_io(); //Se toma un pedazo de memoria para tener acceso sin permiso del sistema

  fselGPIO21 = *(gpio+2);
  fselGPIO21 = fselGPIO21 & ~(7<<3);	//Apagar los bits 5-3
  fselGPIO21 = fselGPIO21 | (1<<3);	//Prender el bit 3
  *(gpio+2) = fselGPIO21; //HW

  for (rep=0; rep<10; rep++) { //10veces
       fsetGPIO21 = *(gpio+7);//Me da registro de los bits para prender el puerto
       fsetGPIO21 = fsetGPIO21 | (1<<21); //Prender el bit
       *(gpio+7) = fsetGPIO21;//Prende led
       printf("SET\n");
       sleep(5);
       fclrGPIO21 = *(gpio+10);
       fclrGPIO21 = fclrGPIO21 | (1<<21);//Prender bit del registro clear
       *(gpio+10) = fclrGPIO21;
       printf("CLR\n");
       sleep(5);
  }

  return 0;

}


// Mapear la memoria de periféricos del GPIO en el espacio de memoria del proceso
void setup_io() {
   // Abrir el archivo /dev/mem
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) { //RDWR - Read&Write
      printf("can't open /dev/mem \n");//Imprime que hubo error
      exit(-1);
   }

   // Mapear el espacio del GPIO ... mmap
   gpio_map = mmap(
      NULL,             //Cualquier dirección en el epacio de memoria del proceso
      BLOCK_SIZE,       //Tamaño del bloque a mapear
      PROT_READ|PROT_WRITE,// Habilitar lecturas y escrituras en el bloque de memoria
      MAP_SHARED,       //Compartir el espacio con otros procesos o RESTRICTED
      mem_fd,           //Referencia al archivo que debemos mapear
      GPIO_BASE         //Desplazamiento ("offset") a la dirección del GPIO
   );

   close(mem_fd); //Ya mapeada la memoria podemos cerrar el archivo

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//Reportar error si falla - puede ser porque alguien ya utiliza gpio
      exit(-1);
   }

   // Asignar el espacio a un apuntador en memoria volátil
   gpio = (volatile unsigned *)gpio_map;
}
