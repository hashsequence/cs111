#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include<openssl/ssl.h>
#include<openssl/err.h>
#include<openssl/evp.h>
//use   struct tm *localtime_r(const time_t *timep, struct tm *result);
// use  size_t strftime(char *s, size_t max, const char *format,
//const struct tm *tm);
/*
port number 16000
port number 17000

port mumber corresponds to progress

use memcpy and memset instead of bzero and bcopy

use 2 threads to read and write

if stop then keep sleeping

if not stop then send temp


part 3

SSL_library_init();      
SSL_load_error_string();
OpenSSL_add_all_algorithms();
SSL_CTX_new(TLSv1_client_method())
SSL_new();
SSL_set_fd(); 
SSL_connect(); insead of original socket command
SSL_write();
SSL_read();

read from the server to check if you got it 

DISP is extracredit

need i2c
analog input

do not need to anything regarding certificate
 */

const int B=4275;                 // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A5;     // Grove - Temperature Sensor connect to A5

void setup()
{
  Serial.begin(9600);
}


int main(int argc, char** argv)
{
  unsigned int counter = 0;
  struct tm my_result;
  time_t ltime;
  char buf[50];
  FILE* part1_log;
  part1_log_fd = fopen("temp.log","w");

  while(1)
    {
      ltime = time(&ltime);
      localtime_r(&ltime,&my_result);
      strftime(buf, 50, "%H:%M:%S", &my_result);
      int a = analogRead(pinTempSensor);

      float R = 1023.0/((float)a)-1.0;
      R = 100000.0*R;

      float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
      Serial.print(temperature);
      Serial.print(" ");
      Serial.print(buf);
      Serial.print('\n');
      // printf("%s\n", buf);
      fprintf(part1_log_fd,"%f %s\n",temperature,buf);
      if (counter > 100)
	break;
      counter++;
      sleep(1);
    }
  fclose(part1_log_fd);
  exit(0);
}
