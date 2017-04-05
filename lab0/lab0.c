#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<signal.h>
#include<fcntl.h>
#include<getopt.h>

void rw(int ifd,int ofd,int size)
{
  char  c;
  int check = read(ifd,&c,size);
  while(check > 0)
    {
      write(ofd,&c,size);
      check = read(ifd,&c,size);
    }
}

void sigint_handler(int sig)
{
      fprintf(stderr, "caught signal error for segmentation fault\n");
      exit(3);
}

int main(int argc, char* argv[])
{
  /*
   if (argc == 1)
    {
      rw(0,1,sizeof(char));
    }
  */
 int d;
 int ifd = 0;
 int ofd = 1;
 int* nullptr = NULL;
 int segfaultflag = 0;

      static struct option long_options[]=
	{
	  {"input", required_argument, 0, 'i'},
	  {"output", required_argument, 0, 'o'},
	  {"segfault", no_argument, 0, 's'},
	  {"catch", no_argument, 0, 'c'},
	  {0,0,0,0}
	};

 
      int option_index = 0;
      while( (d = getopt_long(argc, argv, "i:o:sc", long_options, &option_index)) != -1)
	{
	  // printf("%s\n", optarg);
      switch(d)
	{
	case 'i':
	  ifd = open(optarg, O_RDONLY);
	  if (ifd >= 0)
	    {
	      close(0);
	      dup(ifd);
	      close(ifd);
	    }
	  else
	    {
	      fprintf(stderr, "error reading input from %s\n", optarg);
	      perror("error reading input");
	      exit(1);
	    }
	  break;
	case 'o':
	  ofd = creat(optarg, 0666);
	  if (ofd >= 0)
	  {
	    close(1);
	    dup(ofd);
	    close(ofd);
	  }
	  else
	    {
	      fprintf(stderr, "error creating output file %s\n", optarg);
	      perror("error creating output");
	      exit(2);
	    }
	  break;
	case 's':
	  segfaultflag = 1;
	  break;
	case 'c':
	  //	  printf("I see this\n");
	  signal(SIGSEGV, sigint_handler);
	  break;
	default:
	  break;
	}
      }
   
      //printf("ifd is %d\n", ifd);

  if (segfaultflag == 1)
    {
      *nullptr = 1;
    }
  rw(0,1,sizeof(char));
  //close(ifd);
  //close(ofd);
  exit(0);
}
