#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include<mraa.h>
#include<mraa/gpio.h>
#include<pthread.h>

const int B=4275;                 // B value of the thermistor
#define TEMP_ADC_PIN 0


int main(int argc, char** argv)
{
 
  mraa_aio_context adcPin; 
  float adcValue;
  float R, temperature;
  
  unsigned int counter = 0;
  struct tm my_result;
  time_t ltime;
  char buf[50];
  FILE* part1_log_fd;
  

  mraa_init();
  adcPin = mraa_aio_init(TEMP_ADC_PIN);
  part1_log_fd = fopen("lab4_1.log","w");
      
  while(1)
    {
      ltime = time(&ltime);
      localtime_r(&ltime,&my_result);
      strftime(buf, 50, "%H:%M:%S", &my_result);
      adcValue = mraa_aio_read(adcPin);
      R = 1023.0/((float)adcValue)-1.0;
      R = 100000.0*R;
      temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
      temperature = 9.0/5.0*temperature + 32.0; // convert from c to f
      fprintf(part1_log_fd,"%s %.1f\n",buf, temperature);
	  fprintf(stdout,"%.1f\n",temperature);
      if (counter > 70)
	break;
      counter++;
      sleep(1);
    }
  fclose(part1_log_fd);
  exit(0);
}
