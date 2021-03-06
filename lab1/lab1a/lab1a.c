#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<getopt.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
#include<string.h>


int finalexit = 0;

static int verbose_flag = 0;


//file descriptr variables

static int  maxfd = 10;
static int  fdind = 0;
static int* fdtable;

//processes variables

static int maxcld = 2;
static int cldind = 0;
static int childargcounter = 0;
struct childname
{
  pid_t cldpid;
  char** cldarg;
  int maxarglen;
};

static struct childname* cldptr;

int startcmd(char* cmd, int* fdstream, char** argv);

void freemem();

char** copystrarr(char** argv);
int main(int argc, char** argv)
{  

  int option_index;
  int d;
  fdind = 0;
  fdtable = (int*)malloc(maxfd*sizeof(int));
  if (fdtable == NULL)
    {
      fprintf(stderr, "error allocating memory for fdtable\n");
    }
  //command variables
  int commandfd[3];
  int streamind = 0;
  int argarrsize = 1;
  char** argarrptr = (char**)malloc(sizeof(char*)*argarrsize);

   if (argarrptr == NULL)
    {
      fprintf(stderr, "error allocating memory\n");
      exit(-1);
    }
   argarrptr[0] = NULL;
cldptr = (struct childname*)malloc(sizeof(struct childname)*maxcld);

if( cldptr == NULL)
  {
      fprintf(stderr, "error allocating memory\n");
      exit(-1);
  }

  int argarrind = 0;
  int argcounter = 0;

  static struct option long_options[] = 
    {
      {"verbose", no_argument, &verbose_flag,1},
      {"rdonly", required_argument, 0, 'r' },
      {"wronly", required_argument, 0, 'w' },
      {"command",required_argument,0,'c'},
      {0,0,0,0}
    };

  while( (d = getopt_long(argc, argv, "", long_options, &option_index)) != -1)
    {
      switch(d)
	{
	case 0:
	  	  if (verbose_flag)
		    printf("--%s\n",long_options[option_index].name);		 
	       
	  break;
	case 'r':
	  	  if (verbose_flag)
		    printf("--%s %s\n",long_options[option_index].name, optarg);
		  fdtable[fdind] = open(optarg,O_RDONLY);
		   if ( fdtable[fdind] < 0)
		    {
		      fprintf(stderr, "Error opening file\n");
		      finalexit = 1;
		    }
		  fdind++;
	     
		  if (fdind >= maxfd)
		    {
		      maxfd*=2;
		      fdtable=(int*)realloc(fdtable,sizeof(int)*maxfd);
		    }
		  if (fdtable == NULL)
		    {
		      fprintf(stderr,"error in realloc fdtable");
		      freemem();
		      exit(1);
		    }
	  break;
	case 'w':
	  	  if (verbose_flag)
		    printf("--%s %s\n",long_options[option_index].name, optarg);


		  fdtable[fdind] = open(optarg,O_WRONLY);
		  if ( fdtable[fdind] < 0)
		    {
		      fprintf(stderr, "Error opening file\n");
		      finalexit = 1;
		    }

		  fdind++;
		  if (fdind >= maxfd)
		    {
		      maxfd*=2;
		      fdtable=(int*)realloc(fdtable,sizeof(int)*maxfd);
		    }
		  if (fdtable == NULL)
		    {
		      fprintf(stderr,"error in realloc fdtable");
		      freemem();
		      exit(1);
		    }
	  break;
	case 'c':

		  argcounter = optind;
		  
	    while (argv[argcounter-1] != '\0' && (argv[argcounter-1][0] == '-' && argv[argcounter-1][1] == '-') == 0)
	    {

	      if (streamind < 3)
		{
		  if((commandfd[streamind] = atoi(argv[argcounter-1])) < 0)
		    {
		      fprintf(stderr, "file descriptors for --command must be positive integers\n");
		      finalexit = 1;
		    }  
		   if (fdind >= maxfd)
		    {
		      maxfd*=2;
		      fdtable=(int*)realloc(fdtable,sizeof(int)*maxfd);
		    }
		  if (fdtable == NULL)
		    {
		      fprintf(stderr,"error in realloc fdtable");
		      freemem();
		      exit(1);
		    }
		    streamind++;
		}
	      else if (streamind == 3)
		{
		   if((commandfd[0] >= fdind) || (commandfd[1] >= fdind) || (commandfd[2] >= fdind))
		    {
		      fprintf(stderr, "file descriptors past current file descriptor count\n");
		      finalexit = 1;
		    }
		  argarrptr[argarrind] = &argv[argcounter-1][0];
		  argarrind++;;
		  streamind++;
		  argarrptr[argarrind] = NULL;
		}
	      else if (streamind > 3)
		{
		  if ( streamind == argarrsize + 3)
		    {
		      argarrsize*=2;
		      argarrptr = (char**)realloc(argarrptr, sizeof(char*)*argarrsize);
		      if (argarrptr == NULL)
			{
			  fprintf(stderr, "error allocating memory\n");
			  freemem();
			  exit(1);
			}
		    }
		  argarrptr[argarrind] = &argv[argcounter-1][0];
		  streamind++;
		  argarrind++;
		  argarrptr[argarrind] = NULL;
		}
	      argcounter++;
	    }
	    childargcounter = argarrind + 1;
	    if (streamind < 4)
	      {
		fprintf(stderr, "need at least four arguments for --command\n");
		finalexit = 1;
	      }
	    
	    if (verbose_flag)
	      {
	       	printf("--%s %d %d %d %s",long_options[option_index].name, commandfd[0],commandfd[1],commandfd[2],argarrptr[0]);
		int i = 1;
		while (argarrptr[i] != '\0')
		  {
		  printf(" %s",argarrptr[i]);
		  i++;
		  }
		printf("\n");
		
	      }
	  startcmd(argv[0],commandfd,argarrptr);
	  argcounter--;
	  optind=argcounter;
	  free(argarrptr);
	  streamind = 0;
	  argarrind = 0;
	  argarrsize = 2;
	  argcounter = 0;
	  break;
	default:
	  fprintf(stderr, "unrecognized option %s\n", argv[option_index]);
	  finalexit = 1;
	}
    }
    int j = 0;
   for (j=0; j < fdind; j++)
     close(fdtable[j]);
    freemem();
    exit(finalexit);
}

int startcmd(char* cmd, int* fdstream, char* argv[])
{
  int dup_status;
  int exec_status;
  pid_t pid = fork();
  //int pid_status;
  /*
      printf("\nfile descriptors %d %d %d\n", fdtable[fdstream[0]], fdtable[fdstream[1]], fdtable[fdstream[2]]);
      int i = 1;
      printf("cmd: %s\n", argv[0]);
      while(argv[i] != NULL)
	{
	  printf("argv[%d]: %s\n", i, argv[i]);
	  i++;
	}
  */
   if (pid  < 0)
    {
      fprintf(stderr, "error forking\n");
      finalexit = 1; 
    }
  else if (pid == 0)
    {
      //child process
      dup_status = dup2(fdtable[fdstream[0]],0); 
      if (dup_status == -1)
	{
	  fprintf (stderr, "error in redirection input\n");
	  freemem();
	  exit(-1);
	}
      dup_status = dup2(fdtable[fdstream[1]],1); 
      if (dup_status == -1)
	{
	  fprintf (stderr, "error in redirection input\n");
	  freemem();
	  exit(-1);
	}
      dup_status = dup2(fdtable[fdstream[2]],2); 
      if (dup_status == -1)
	{
	  fprintf (stderr, "error in redirection input\n");
	  freemem();
	  exit(-1);
	}
      //need to close file descriptor in child process otherwise there will be a hang
      int i = 0;
      for (i = 0; i < fdind; i++)
	{
	  close(fdtable[i]);
	}
      exec_status = execvp(argv[0],argv);
      if (exec_status < 0 )
	{
	  fprintf(stderr, "error in execvp\n");
	  freemem();
	  exit(-1);
	}

    }
   
  else if (pid > 0)
    {
      //parent process
      cldptr[cldind].cldpid = pid;
      cldptr[cldind].cldarg = copystrarr(argv);
      int k = 0;
      while(argv[k] != NULL)
	{
	  k++;
	}
      cldptr[cldind].maxarglen = k;
      cldind++;
      /*
      int f = 0;
      while(cldptr[cldind-1].cldarg[f] != NULL)
	{
	  printf("cldptr[%d].cldarg[%d]: %s\n",cldind-1,f,cldptr[cldind-1].cldarg[f]);
	  f++;
	}
      f = 0;
      printf("cldptr[%d].maxarglen: %d\n",cldind-1,cldptr[cldind-1].maxarglen);
      */
      if(cldind >= maxcld)
	{
	  maxcld*=2;
	  cldptr = (struct childname*)realloc(cldptr, sizeof(struct childname)*maxcld);
	}
      if (cldptr == NULL)
	{
	  fprintf(stderr, "error in realloc cldptr\n");
	  freemem();
	  exit(1);
	}
      
    }
  return 0;

}

void freemem()
{
  int i = 0;
  for (i = 0; i < (*cldptr).maxarglen; i++)
    {
      free((*cldptr).cldarg[i]);
    }
  free(cldptr);
  free(fdtable);
}

 char** copystrarr( char** argv)
{
  char** temparr;
  int maxarrlen=0;
  int currstrlen=0;
  
  
  
  int i = 0;  
  while(argv[i] != NULL)
    {
      //    printf("argv[%d]: %s\n", i, argv[i]);
      maxarrlen++;
      i++;
    }
  
  maxarrlen++;
  //printf("maxarrlen: %d\n", maxarrlen);
  temparr = (char**)malloc(sizeof(char*)*maxarrlen);
  int j = 0;
  for (i = 0; argv[i] != NULL; i++)
    {
      if (argv[i] != NULL)
	{
	  currstrlen = strlen(argv[i]);	  
	  temparr[i] = (char*)malloc(sizeof(char)*(currstrlen + 1));
	  for (j = 0; j <= currstrlen; j++)
	    {
		 temparr[i][j] = argv[i][j];
	    }
	  //	  printf("currstrlen: %d\n temparr[%d]: %s\n", currstrlen, i, temparr[i]);
	}
      else
	{
	  temparr[i] = NULL;
	}
    }

    
return temparr;
}

