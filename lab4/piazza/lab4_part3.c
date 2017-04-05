#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include<time.h>
#include<math.h>
//#include<mraa.h>
//#include<mraa/gpio.h>
#include<pthread.h>
#include<netdb.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<errno.h>

#include <arpa/inet.h>
#include<openssl/ssl.h>
#include<openssl/err.h>
#include<openssl/evp.h>
//#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
//#include <openssl/x509.h>

/*
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
//const int R0 = 100000;            // R0 = 100k
//const int pinTempSensor = A5;     // Grove - Temperature Sensor connect to A5

struct tm my_result;
time_t ltime;
char buf_time[50];
/*
const int B=4275;                 // B value of the thermistor
#define TEMP_ADC_PIN 0
mraa_aio_context adcPin;
float adcValue;
float R;
*/
float temperature, temperature_c, temperature_f;

int sockfd; 
SSL *ssl;

char buf_write[256];
char buf_write2[256];
char buf_read[256];

FILE* part3_log_fd; 

unsigned int T = 3;
int temperature_format = 0;  //0 for f and 1 for c

int off_flag = 0;
int stop_flag = 0;	
/*
void* thread_temp(void* arg)
{
  while(1)
    {
      if(off_flag == 1)
	break;
      	  //adcValue = mraa_aio_read(adcPin);
          // R = (1023.0 / ((float)adcValue) - 1.0)*100000;
	  //temperature_c = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15;//convert to temperature via datasheet ;
	  //temperature_f = 9.0 / 5.0*(1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15) + 32.0; // convert from c to f
	  
	  temperature_f = 75;
	  temperature_c = 25;
	  sleep(T);
    }
  return NULL;
}
*/
void* thread_read(void* arg)
{
  int p_check = 0;
  while(1)
    {
      if (off_flag == 1)
	break;
	  //fcntl(sockfd,F_SETFL, O_NONBLOCK);
      //	  printf("start read loop\n");
	  memset(buf_read,0, sizeof(buf_read));
	  
	  int bytes_rec = 0;
	  
	 // bytes_rec = recv(sockfd, buf_read, sizeof(buf_read)-1, 0);
	// printf("before ssl_read\n");
	  bytes_rec = SSL_read(ssl,buf_read, sizeof(buf_read)-1);
	//  printf("after ssl_read\n");
	  if (bytes_rec < 0)
	    {
	      perror("ERROR recv:");
	      off_flag = 1;
	      break;
	    }
	  printf("buf_read: %s\n", buf_read);
	  //	  printf("bytes_rec: %d\n", bytes_rec);
	  //	  buf_read[bytes_rec] = '\0';
	  
     // printf("sizeof(buf_read): %d\n", sizeof(buf_read));
	  /*
	  int n = read(sockfd, buf_read, sizeof(buf_read)-1);
	  //buf_read[strlen(buf_read)+1] = '\0';
	 if (n < 0)
	{
	  perror("ERROR reading from socket");
	  exit(1);
	}
	*/
	if (bytes_rec == 0)
	  {
	    ;
	  }
       else if (strcmp(buf_read, "OFF") == 0)
	{
	  fwrite (buf_read , sizeof(char), strlen(buf_read), part3_log_fd);
	  fwrite ("\n" , sizeof(char), sizeof(char), part3_log_fd); 
	  off_flag = 1;
	  break;
	}
      else if (strcmp(buf_read, "STOP") == 0)
	{
	  stop_flag = 1;
	  fwrite (buf_read , sizeof(char), strlen(buf_read), part3_log_fd);
	  fwrite ("\n" , sizeof(char), sizeof(char), part3_log_fd);
	}
      else if(strcmp(buf_read,"START") == 0)
	{
	  stop_flag = 0;
	  fwrite (buf_read , sizeof(char), strlen(buf_read), part3_log_fd);
	  fwrite ("\n" , sizeof(char), sizeof(char), part3_log_fd);
	}
      else if (strcmp(buf_read,"SCALE=F") == 0)
	{
	  temperature_format = 0;
	  fwrite (buf_read , sizeof(char), strlen(buf_read), part3_log_fd);
	  fwrite ("\n" , sizeof(char), sizeof(char), part3_log_fd);
	}
      else if (strcmp(buf_read,"SCALE=C") == 0)
	{
	  temperature_format = 1;
	  fwrite (buf_read , sizeof(char), strlen(buf_read), part3_log_fd);
	  fwrite ("\n" , sizeof(char), sizeof(char), part3_log_fd);
	}
      else if (strncmp(buf_read, "PERIOD=", 7) == 0)
	{   
	  p_check = atoi(buf_read+7);
	  if ((p_check >= 0 && p_check <= 3600) && (strlen(buf_read) <= 11)) 
	    {
	      T = p_check;
	      printf("new period: %d\n", T);
	      fwrite (buf_read , sizeof(char), strlen(buf_read), part3_log_fd);
	      fwrite ("\n" , sizeof(char), sizeof(char), part3_log_fd);
	    }
	  else
	    {
	      fwrite (buf_read , sizeof(char), strlen(buf_read), part3_log_fd);
	      fwrite (" I\n" , sizeof(char), 3*sizeof(char), part3_log_fd);
	    }
	}
      else if (strcmp(buf_read, "DISP Y") == 0)
	{
	  fwrite (buf_read , sizeof(char), strlen(buf_read), part3_log_fd);
	  fwrite ("\n" , sizeof(char), sizeof(char), part3_log_fd);
	}
      else if (strcmp(buf_read, "DISP N") == 0)
	{
	  fwrite (buf_read , sizeof(char), strlen(buf_read), part3_log_fd);
	  fwrite ("\n" , sizeof(char), sizeof(char), part3_log_fd);
	}
	else
	{
	  fwrite (buf_read , sizeof(char), strlen(buf_read), part3_log_fd);
	  fwrite (" I\n" , sizeof(char), 3*sizeof(char), part3_log_fd);
	}
	//fwrite (buf_read , sizeof(char), sizeof(buf_read), stdout);
	//fwrite ("\n" , sizeof(char), sizeof(char), stdout);
	fflush(part3_log_fd);
	
       	//printf("finish read loop\n");

    }
  return NULL;
}

void* thread_write(void* arg)
{
  srand(time(NULL));
  while(1)
    {
	if (off_flag == 1)
		break;
	if (stop_flag == 0)
	{	
	  ltime = time(&ltime);
      localtime_r(&ltime,&my_result);
      strftime(buf_time, sizeof(buf_time), "%H:%M:%S ", &my_result);
	    // adcValue = mraa_aio_read(adcPin);
	  //R = 1023.0 / ((float)adcValue) - 1.0;
	  //R = 100000.0*R;
	  //temperature_c = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15;//convert to temperature via datasheet ;
	  //temperature_f = 9.0 / 5.0*temperature_c + 32.0; // convert from c to f
      /*	  
	  adcValue = mraa_aio_read(adcPin);
      R = (1023.0 / ((float)adcValue) - 1.0)*100000;
	  temperature_c = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15;//convert to temperature via datasheet ;
	  temperature_f = 9.0 / 5.0*(1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15) + 32.0; // convert from c to f
      */
      float random = ((float) rand()) / (float) RAND_MAX;
      float diff = 79.3 - 78.6;
      float r = random * diff;
      temperature_f = 78.6 + r;
      temperature_c = 5.0 / 9.0 * (temperature_f - 32.0);
	  if (temperature_format == 0)
	  {
		  temperature = temperature_f;
	  }
	  else
	  {
		  temperature = temperature_c;
	  }

	  memset(buf_write,0, sizeof(buf_write));
      sprintf(buf_write,"904582269 TEMP = %.1f",temperature);
	  strncpy(buf_write2, buf_write + 17, 4);
	  buf_write2[strlen(buf_write2)] = '\0';
      buf_write[strlen(buf_write)] = '\0';
	  fwrite(buf_time, sizeof(char), strlen(buf_time), part3_log_fd);
      fwrite (buf_write2 , sizeof(char), strlen(buf_write2), part3_log_fd);
      fwrite ("\n" , sizeof(char), sizeof(char), part3_log_fd); 
	 /*
	  int n = write(sockfd, buf_write, strlen(buf_write));
	  if (n < 0)
	  {
		  perror("ERROR writing to socket\n");
	  }
	  */
	//  printf("before send\n");
	  if (off_flag == 1)
		break;
	 // if(send(sockfd, buf_write, strlen(buf_write), 0)<0) //MSG_DONTWAIT);//MSG_OOB);
	 //printf("before ssl_write\n");
		if(SSL_write(ssl, buf_write, strlen(buf_write)+1) < 0)
	   {
	      perror("send");
	      off_flag = 1;
	      break;
	    }
		//printf("after ssl_write\n");
	  //printf("sizeof(buf_write): %d\n", (int)strlen(buf_write)+1);
	  // perror("send");
	 // printf("after send\n");
	  printf("%s\n", buf_write);
	  fflush(part3_log_fd);
       sleep(T);
	}
	else;

	  
    }
   
  return NULL;
}

int main(int argc, char** argv)
{

  //  mraa_init();
  //adcPin = mraa_aio_init(TEMP_ADC_PIN);
  
  
  SSL_CTX *ctx;
  int err;
  
  
  SSL_library_init();
  SSL_load_error_strings();
  ctx =  SSL_CTX_new(TLSv1_client_method());
  ssl = SSL_new(ctx);
  //SSL_CTX_load_verify_locations(ctx, "/path/to/TrustStore.pem", NULL);
  OpenSSL_add_all_algorithms();
  ssl =  SSL_new(ctx);

  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    getaddrinfo(/*"r01.cs.ucla.edu"*/"localhost", "17000", &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	printf("before connect\n");
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0)
	{
		perror("ERROR connecting");
        close(sockfd);
		exit(1);
	}
  freeaddrinfo(res);
 /*
  struct sockaddr_in server_addr;
  memset (&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family      = AF_INET;
  server_addr.sin_port        = htons((uint16_t)atoi("17000"));     
  server_addr.sin_addr.s_addr = inet_addr("r01.cs.ucla.edu"); 
 
  err = connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr));
  if (err < 0)
  {
	  perror("connect");
	  fclose(part3_log_fd);
	  SSL_shutdown(ssl);
      SSL_shutdown(ssl);
      close(sockfd);
	  exit(1);
  }
  */
  SSL_set_fd(ssl, sockfd);
  //printf("before ssl_connect\n");
  err = SSL_connect(ssl);
   if (err < 0)
  {
	  perror("SSL_connect");
	  fclose(part3_log_fd);
	  SSL_shutdown(ssl);
      //SSL_shutdown(ssl);
      close(sockfd);
	  exit(1);
  }
   printf("after ssl_connect\n");
  //unsigned int counter = 0;
  /*
  //attempt1**********************************************
  int portno = 16000;
  char server_name[] = "r01.cs.ucla.edu";
  struct sockaddr_in serv_addr;
  struct hostent *server;
  portno = (uint16_t)atoi("16000");
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
	  perror("ERROR opening socket");
	  exit(1);
  }

  server = gethostbyname(server_name);
  if (server == NULL)
    {
      perror("ERROR opening socket\n");
      exit(1);
    }
  memset((char*)&serv_addr,0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;//AF_INET;
  memcpy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
 
  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
    exit(1);
  }
  printf("connected\n");
  */
  /*
  //attempt2******************************************
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    getaddrinfo("r01.cs.ucla.edu", "16000", &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0)
	{
		perror("ERROR connecting");
        close(sockfd);
		exit(1);
	}
  freeaddrinfo(res);
  printf("connected\n");
 */
  /*
  //attempt3********************************************************
 
   struct hostent *host;
   struct sockaddr_in server_addr;  
   host = gethostbyname("r01.cs.ucla.edu");

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }

        server_addr.sin_family = AF_INET;     
        server_addr.sin_port = htons(16000);   
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        memset(&(server_addr.sin_zero),0,8); 

        if (connect(sockfd, (struct sockaddr *)&server_addr,
                    sizeof(struct sockaddr)) == -1) 
        {
            perror("Connect");
            exit(1);
        }
		 printf("connected\n");
	*/	 
		 
  //creating threads for recieve and send
  part3_log_fd = fopen("lab4_3.log","w");
  pthread_t write_thread_id;
  pthread_t read_thread_id;
  //pthread_t temp_thread_id;
  int check;
  /*  
  check = pthread_create(&temp_thread_id, NULL, thread_temp, NULL);
  if (check != 0)
    {
      fprintf(stderr, "ERROR: creating threads, return value: %d \n",check);
	  fclose(part3_log_fd);
      close(sockfd);
      exit(1);
    }
  */
  check = pthread_create(&write_thread_id, NULL, thread_write, NULL);
  if (check != 0)
    {
      fprintf(stderr, "ERROR: creating threads, return value: %d \n",check);
	  fclose(part3_log_fd);
	  SSL_shutdown(ssl);
      //SSL_shutdown(ssl);
      close(sockfd);
      exit(1);
    }

 check = pthread_create(&read_thread_id, NULL, thread_read, NULL);
  if (check != 0)
    {
      fprintf(stderr, "ERROR: creating threads, return value: %d \n",check);
	  fclose(part3_log_fd);
	  SSL_shutdown(ssl);
      //SSL_shutdown(ssl);
      close(sockfd);
      exit(1);
    }
 check = pthread_join(read_thread_id, NULL);
  if (check != 0)
    {
      fprintf(stderr, "ERROR: creating threads, return value: %d \n",check);
      fclose(part3_log_fd);
	  SSL_shutdown(ssl);
     // SSL_shutdown(ssl);
      close(sockfd);
      exit(1);
    }

 check = pthread_join(write_thread_id, NULL);
  if (check != 0)
    {
      fprintf(stderr, "ERROR: creating threads, return value: %d \n",check);
      fclose(part3_log_fd);
	  SSL_shutdown(ssl);
      //SSL_shutdown(ssl);
      close(sockfd);
      exit(1);
    }
  /*
check = pthread_join(temp_thread_id, NULL);
  if (check != 0)
    {
      fprintf(stderr, "ERROR: creating threads, return value: %d \n",check);
      fclose(part3_log_fd);
      close(sockfd);
      exit(1);
    }
  */
		
		
  fclose(part3_log_fd);
  SSL_shutdown(ssl);
  SSL_free(ssl);
  //SSL_shutdown(ssl);
  close(sockfd);
  printf("closed sockfd\n");
  exit(0);
}


