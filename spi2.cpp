#include <fcntl.h>            //Needed for SPI port
#include <sys/ioctl.h>         //Needed for SPI port
#include <linux/spi/spidev.h>   //Needed for SPI port
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h> 

using namespace std;

int fd;
unsigned char spi_mode;
unsigned char spi_bitsPerWord;
unsigned int spi_speed;

void SpiOpenPort (int spi_device)
{ 
    //----- SET SPI MODE -----
    //SPI_MODE_0 (0,0)    CPOL=0 , CPHA=0 
    //SPI_MODE_1 (0,1)    CPOL=0 , CPHA=1 
    //SPI_MODE_2 (1,0)    CPOL=1 , CPHA=0 
    //SPI_MODE_3 (1,1)    CPOL=1 , CPHA=1 
    spi_mode = SPI_MODE_3;
    
    //----- SET BITS PER WORD -----
    spi_bitsPerWord = 8;
    
    //----- SET SPI BUS SPEED -----
    spi_speed = 500000;      //1000000 = 1MHz  

   fd = open(std::string("/dev/spidev0.0").c_str(), O_RDWR);

    ioctl(fd, SPI_IOC_WR_MODE, &spi_mode);
    ioctl(fd, SPI_IOC_RD_MODE, &spi_mode);
    ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
    ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
    ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
}

int SpiWriteAndRead (unsigned char data)
{
   unsigned char rxDat;
   struct spi_ioc_transfer spi;
 
   spi.tx_buf        = (unsigned long)&data; // transmit from "data"
   spi.rx_buf        = (unsigned long)&rxDat; // receive into "rxDat"
   spi.len           = 1 ;
   spi.delay_usecs   = 0 ;
   spi.speed_hz      = spi_speed ;
   spi.bits_per_word = spi_bitsPerWord ;
   spi.cs_change = 0;
   spi.pad = 0;
   
   ioctl(fd, SPI_IOC_MESSAGE(1), &spi) ;

   return rxDat;
}

int main(void)
{
   int i,h,j;
   unsigned char data;
    SpiOpenPort (0); 
   
   data= 0x91;		// Volt/Div setting
   SpiWriteAndRead (data);
   data= 0x03;		// 1V/div
   SpiWriteAndRead (data);
   data= 0x92;		// Coupling setting
   SpiWriteAndRead (data);
   data= 0x02;		// DC coupling
   SpiWriteAndRead (data);
   data= 0x93;          // Time/Div setting
   SpiWriteAndRead (data);
   data= 0x0c;          // 1MS/s
   SpiWriteAndRead (data);
   data= 0x94;          // Trigger setting
   SpiWriteAndRead (data);
   data= 0x02;          // Trigger Off
   SpiWriteAndRead (data);
   
    for (j=0; j<20; j++)
	{
   data= 0x81; 
   SpiWriteAndRead (data);
   h = 0;
   i = 0;
   j = 0;
   

		
   do
   {
      data= 0x82; 
      usleep (20000);
      h = SpiWriteAndRead (data); 
      //cout << "The Result is: " << int(h) << endl;
      i++;
   }
   while (h != 2  && i < 1000);
   
   for (i=0; i<802; i++)
   {
      data= 0x01; 
      h = SpiWriteAndRead (data); 
      if (i<10)
         cout << "Data [" << int(i) << "] is: " << int(h) << endl;   
   }
   
   /*
   for (i=0; i<802; i++)
   {
      data= 0x02; 
      h = SpiWriteAndRead (data);
      if (i<10)      
         cout << "Data [" << int(i) << "] is: " << int(h) << endl;      
   }
   
   for (i=0; i<802; i++)
   {
      data= 0x03; 
      h = SpiWriteAndRead (data);          
   }
   */
	}
    close(fd);
    return 0;
}  
