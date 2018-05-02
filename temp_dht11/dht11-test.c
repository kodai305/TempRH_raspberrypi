/*
 *  dht11.c:
 *	Simple test program to test the wiringPi functions
 *	DHT11 test
 */

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#define MAXTIMINGS	85
#define DHTPIN		15
#ifndef Boolean         /* if Boolean is not defined */
#define Boolean int
#endif
#ifndef TRUE            /* if TRUE is not defined */
#define TRUE 1
#endif
#ifndef FALSE           /* if FALSE is not defined */
#define FALSE 0
#endif

int dht11_dat[5] = { 0, 0, 0, 0, 0 };

Boolean read_dht11_dat()
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	float	f; /* fahrenheit */

	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

	/* pull pin down for 18 milliseconds */
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 18 );
	/* then pull it up for 40 microseconds */
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 40 );
	/* prepare to read the pin */
	pinMode( DHTPIN, INPUT );

	/* detect change and read data */
	for ( i = 0; i < MAXTIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHTPIN );

		if ( counter == 255 )
			break;

		/* ignore first 3 transitions */
		if ( (i >= 4) && (i % 2 == 0) )
		{
			/* shove each bit into the storage bytes */
			dht11_dat[j / 8] <<= 1;
			if ( counter > 16 )
				dht11_dat[j / 8] |= 1;
			j++;
		}
	}

	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	if ( (j >= 40) &&
	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
	{
	  /*f = dht11_dat[2] * 9. / 5. + 32;*/
		time_t t = time(NULL);

		FILE *outputfile;         
		outputfile = fopen("/var/pi/log/dht.txt", "w");  
		if (outputfile == NULL) {          
		  printf("cannot open\n");         
		  exit(1);                         
		}
		fprintf(outputfile, "Humidity = %d.%d %% Temperature = %d.%d *C Date =  %ld\n",
			dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], t  );
		fclose(outputfile);    

		return (TRUE);
	}else  {
		printf( "Data not good, skip\n" );
		return (FALSE);
	}
}

int main( void )
{
	printf( "Raspberry Pi wiringPi DHT11 Temperature test program\n" );

	if ( wiringPiSetup() == -1 )
		exit( 1 );
	int i=0;
	while ( i < 10 )
	{
	  if (read_dht11_dat() == TRUE){
	    break;
	    delay( 1000 ); /* wait 1sec to refresh */
	  }
	  delay( 1000 ); /* wait 1sec to refresh */
	}

	return(0);
}
