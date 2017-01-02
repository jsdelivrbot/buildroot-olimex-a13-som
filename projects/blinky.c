
// Blinky via zelf geschreven user-space driver

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>

// Includes voor systemcals
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>

#include <errno.h>

#define OLI_GPIO_BASE 0x01C20800 			// Base adress van de gpio

#define OLI_PG_DAT (((OLI_GPIO_PORT - 1) * 0x24) + 0x10)
#define OLI_PG_CFG1 (((OLI_GPIO_PORT - 1) * 0x24) + 0x4)

#define OLI_GPIO_PIN 9
#define OLI_GPIO_PORT 7

//
// Functie om bij ctrl-c zelf proper te stoppen
//
volatile uint8_t run = 1;
void stopHandler()
{
	run = 0;
}

void delay(unsigned int millis);


int main(int argc, char * argv[])
{
	// Bij een ctrl-c willen we zelf het programma proper stoppen
	signal(SIGINT, stopHandler);

	printf("Programma is gestart.\n");

	int memory;	
	if (geteuid() == 0)
    {
      	// We openen het memory gemapt in "/dev/mem"
      	// Er wordt een integer gereturnt met de plaats waar het geopend is
      	if ((memory = open("/dev/mem", O_RDWR | O_SYNC) ) < 0) 
		{
			perror("Fout bij openen /dev/mem");
			exit(1);
		}
	}

	// We moeten lezen waar de periphiral begint in het geheugen
	uint32_t periphiral_base = 0x00000000;
	uint32_t periphiral_size = 0x01C20BFF;
	//getperiBaseRegisterAdress(&periphiral_base,&periphiral_size);

	// We printen even deze waardes af
	printf("Peri Base adress: %x \n",periphiral_base);
	printf("Peri Base size: %x \n",periphiral_size);

	// We hebben /dev/mem geopend, nu gaan we deze geheugenplaatsen mappen in user-space memory
	uint32_t * virtualMemory;
	virtualMemory = mmap(NULL,(size_t)periphiral_size,(PROT_READ | PROT_WRITE),MAP_SHARED,memory,(uint32_t)periphiral_base);
	if(virtualMemory == MAP_FAILED)
	{
		perror("Fout bij mappen memory.");
		exit(2);
	}

	// We hebben nu het begin-addres van het virtueel gemapte memory,
	// nu willen we nu naar de gpio registers springen
	volatile uint32_t * oli_gpio;
	oli_gpio = virtualMemory + OLI_GPIO_BASE/4;
	printf("mmap: %x, Oli_gpio: %x\n",virtualMemory,oli_gpio);

	volatile uint32_t * gpioPtr;	

	// We zetten pin 9 als output
	gpioPtr = oli_gpio + OLI_PG_CFG1/4;		// We zetten op bits 4-6: 001
	*gpioPtr &= ~(1<<5) & ~(1<<6);
	*gpioPtr |= (1<<4);

	// Pointer naar het data register
	gpioPtr = oli_gpio + (OLI_PG_DAT/4);

	while(run)
	{
		// Led pin aan
		*gpioPtr |= (1 << OLI_GPIO_PIN);

		delay(200);

		// Led pin uit
		*gpioPtr &= ~(1 << OLI_GPIO_PIN);

		delay(200);
	}

	printf("Programma is gestopt\n");
	
	munmap((void**) &virtualMemory, periphiral_size);
	close(memory);

	return 0;
}

void delay(unsigned int millis)
{
    struct timespec sleeper;
    
    sleeper.tv_sec  = (time_t)(millis / 1000);
    sleeper.tv_nsec = (long)(millis % 1000) * 1000000;
    nanosleep(&sleeper, NULL);
}
