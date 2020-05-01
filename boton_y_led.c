/*
  Maria Fernanda Hernandez Enriquez A01329383
  Nestor Maldonado Pámanes A01381058
*/

#define BCM2835_PERI_BASE        0x3F000000
#define GPIO_BASE                (BCM2835_PERI_BASE + 0x200000) /* Controlador GPIO */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>	//Funciones sleep(...) y close(...)

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;

#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUTP_GPIO(g) *(gpio+((g)/10)) |= (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g, a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
#define GPIO_SET(g) (g) <= 32 ? (*(gpio+7) |= (1<<(g%32))) : (*(gpio+8) |= (1<<(g%32)))
#define GPIO_CLR(g) (g) <= 32 ? (*(gpio+10) |= (1<<(g%32))) : (*(gpio+11) |= (1<<(g%32)))
#define GPIO_PULL(g) *(gpio+37) = (1 << g)  // g == 0 Se acctiva pull down, g == 1 se activa pull up.
#define GPIO_PULL_PIN(g)  (g) < 32 ? (*(gpio+38) |= (1<<(g%32))) : (*(gpio+39) |= (1<<(g%32)))

// Mapear la memoria de periféricos del GPIO en el espacio de memoria del proceso
void setup_io() {
   // Abrir el archivo /dev/mem
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   // Mapear el espacio del GPIO ... mmap
   gpio_map = mmap(
      NULL,             //Cualquier dirección en el epacio de memoria del proceso
      BLOCK_SIZE,       //Tamaño del bloque a mapear
      PROT_READ|PROT_WRITE,// Habilitar lecturas y escrituras en el bloque de memoria
      MAP_SHARED,       //Compartir el espacio con otros procesos
      mem_fd,           //Referencia al archivo que debemos mapear
      GPIO_BASE         //Desplazamiento ("offset") a la dirección del GPIO
   );

   close(mem_fd); //Ya mapeada la memoria podemos cerrar el archivo

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//Reportar error si falla
      exit(-1);
   }

   // Asignar el espacio a un apuntador en memoria volátil
   gpio = (volatile unsigned *)gpio_map;
}

int main(int argc, char **argv) {
  int g;

  // Set up gpi pointer for direct register access
  setup_io();

  INP_GPIO(20);
  INP_GPIO(21);
  OUTP_GPIO(21);
  GPIO_PULL(1);
  GPIO_PULL_PIN(20);
  
  int fgetGPIO20;
  for (;;) {
    fgetGPIO20 = *(gpio+13);
	  if( (fgetGPIO20 & (1<<20)) !=0){
      GPIO_SET(21);
    } else {
      GPIO_CLR(21);
    }
  }

  return 0;

}