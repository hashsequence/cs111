#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<getopt.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<sys/resource.h>


static int profile_flag = 0;
//			  printf("%ld.%06ld\n", usage[0].ru_stime.tv_sec, usage[0].ru_stime.tv_usec);
static int finalexit = 0;
static int verbose_flag = 0;

static int cloexec_flag = 0;
static int nonblock_flag = 0;
static int append_flag = 0;
static int creat_flag = 0;
static int directory_flag = 0;
static int dsync_flag = 0;
static int excl_flag = 0;
static int nofollow_flag = 0;
static int rsync_flag = 0;
static int sync_flag = 0;
static int trunc_flag = 0;

//profile variables
static struct rusage usage[5];
void printoptions(const char* name, int usageindex);

//file descriptr variables

static int  maxfd = 10;
static int  fdind = 0;
static int* fdtable;

//processes variables

static int maxcld = 5;
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

//pipe functions

int pipeflags();
void resetflags();

int setflags();

//abort functios
void sigsegvhandler(int signal);
int segret;

//catch functions
void catchhandler(int signal);
void ignorehandler(int signal);
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


  //pipe variables
  int pipe_flag = 0;
  int pipe_error = 0;
  int pipefd[2];

  //wait variables
  pid_t  w;
  int status;
  int waitflag = 0;

  //close variables
  int N;

  //profile variables
  int getrusagecheck = 1;
  int getrusageret;
  static struct option long_options[] = 
    {
      {"verbose", no_argument, &verbose_flag,1},
      {"profile", no_argument, &profile_flag,1},
 
      {"nonblock", no_argument, &nonblock_flag, -1},
      {"cloexec", no_argument, &cloexec_flag,-1},
      {"append",    no_argument, &append_flag,    -1},
      {"creat",     no_argument, &creat_flag,     -1},
      {"directory", no_argument, &directory_flag, -1},
      {"dsync",     no_argument, &dsync_flag,     -1},
      {"excl",      no_argument, &excl_flag,      -1},
      {"nofollow",  no_argument, &nofollow_flag,  -1},
      {"rsync",     no_argument, &rsync_flag,     -1},
      {"sync",      no_argument, &sync_flag,      -1},
      {"trunc",     no_argument, &trunc_flag,     -1},

      {"rdonly", required_argument, 0, 'r' },
      {"wronly", required_argument, 0, 'w' },
      {"rdwr", required_argument, 0, 'f' },
      {"pipe", no_argument, 0, 'p'},

      {"command",required_argument,0,'c'},
      {"wait", no_argument, 0, 'z'},

      {"close", required_argument, 0, 's'},
      {"abort",no_argument, 0, 'a'},
      {"catch",required_argument, 0 ,'h'},
      {"ignore", required_argument, 0, 'i'},
      {"default", required_argument, 0, 'd'},
      {"pause", no_argument, 0, 'q'},
      {0,0,0,0}
    };

  while( (d = getopt_long(argc, argv, "", long_options, &option_index)) != -1)
    {
      if (profile_flag)
	{
	  if (getrusage(RUSAGE_SELF, &usage[0]) < 0)
	    {
	      fprintf(stderr, "Error: --profile failed for.\n");
	      finalexit = 1;
	    }
	}
      switch(d)
	{
	case 0:
	  if(verbose_flag)
	    {
		    printf("--%s\n",long_options[option_index].name);
	    }	    
	  break;
	case 'r':
	  	  if (verbose_flag)
		    printf("--%s %s\n",long_options[option_index].name, optarg);
		  fdtable[fdind] = open(optarg,O_RDONLY | setflags());
		  resetflags();
		   if ( fdtable[fdind] < 0)
		    {
		      fprintf(stderr, "Error opening file %s:%s \n", optarg, strerror(errno));
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
		  if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --rdonly.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(long_options[option_index].name, 1);
			}
		    }
	  break;
	case 'w':
	  	  if (verbose_flag)
		    printf("--%s %s\n",long_options[option_index].name, optarg);


		  fdtable[fdind] = open(optarg,O_WRONLY|setflags());
		  resetflags();
		  if ( fdtable[fdind] < 0)
		    {
		      fprintf(stderr, "Error opening file %s:%s \n", optarg, strerror(errno));
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
		  if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --wronly.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(long_options[option_index].name, 1);
			}
		    }
	  break;
	  case 'f':
	  if (verbose_flag)
	    printf("--%s %s\n",long_options[option_index].name, optarg);
	  fdtable[fdind] = open(optarg,O_RDWR|setflags());
	  resetflags();
	  if ( fdtable[fdind] < 0)
	    {
	      fprintf(stderr, "Error opening file %s:%s \n", optarg, strerror(errno));
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
	  if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --rdwr.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(long_options[option_index].name, 1);
			}
		    }
	  break;
	case 'p':
          if (verbose_flag)
	    printf("--%s\n",long_options[option_index].name);
          
	  pipe_flag = pipeflags();
	  resetflags();
	  pipe_error = pipe2(pipefd,pipe_flag);
	  
	  
      	  if (2 + fdind >= maxfd)
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
	  fdtable[fdind] = pipefd[0];
	  fdind++;
	  fdtable[fdind] = pipefd[1];
	  fdind++;

	  if (pipe_error == -1)
	    {
	    perror("error: pipe creation failed\n");
	    finalexit = 1;
	    }
	  if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --pipe.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(long_options[option_index].name, 1);
			}
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
		      fprintf(stderr, "file descriptors passed current file descriptor count\n");
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
	  streamind = 0;
	  argarrind = 0;
	  argarrsize = 2;
	  argcounter = 0;
	  if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --command.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(argarrptr[0],-1);
			}
		    }
	  break;
	case 'z':
               if (verbose_flag)
		 printf("--%s\n",long_options[option_index].name);
	       if (waitflag == 0)
		 {  
		   waitflag = 1;
		   for (int j=0; j < fdind; j++)
		     close(fdtable[j]);
		 }
	       while ((w = waitpid(-1, &status, 0)) > 1)
		 {
		 // if (w > 0)
		 //break;
		 if (w == -1) 
		   {
		   perror("waitpid");
		   finalexit = 1;
		   break;
		   }
		 if (WIFEXITED(status)) 
		   {
		   printf("%d", WEXITSTATUS(status));
		   }
		 for (int k = 0; k < cldind; k++)
		   {
		     if (cldptr[k].cldpid == w)
		       {
			 for(int f = 0; cldptr[k].cldarg[f] != NULL; f++)
			   {
			     printf(" %s",cldptr[k].cldarg[f]);
			   }
			 printf("\n");
			 break;
		       }
		   }
		 }
	       if (profile_flag)
		    {
		      if ((getrusageret = getrusage(RUSAGE_CHILDREN, &usage[2])) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --wait.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusageret == 0)
			{ 
			  printoptions("total for all children", 2);
			}
		    }
	  break;
	case 's':
	  if (verbose_flag)
	    printf("--%s %s\n",long_options[option_index].name,optarg);
	   N = atoi(optarg);
	  if (N >= fdind || fdind < 0)
	    {
	      fprintf(stderr, "invalid N, unable to close file descriptor N\n");
	      finalexit = 1;
	      break;
	    }
	  else
	    {
	      close(fdtable[N]);
	      fdtable[N]= -1;
	    }
	  if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --close.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(long_options[option_index].name, 1);
			}
		    }
	  break;
	case 'a':
	  if (verbose_flag)
		 printf("--%s\n",long_options[option_index].name);
	  //      signal(SIGSEGV,&sigsegvhandler);
      segret = raise(SIGSEGV);
      if (segret != 0)
	{
	  fprintf(stderr,"unable to raise segmementation fault");
	  finalexit = 1;
	}
      if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --abort.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(long_options[option_index].name, 1);
			}
		    }
      break;
        case 'h':
	       if (verbose_flag)
		 printf("--%s %s\n",long_options[option_index].name, optarg);
	       N = atoi(optarg);
	       if (N <= 0 || N > 64)
		 {
		   fprintf(stderr,"signal %d does not exist\n", N);
		   finalexit = 1;
		   break;
		 }
	      N = atoi(optarg);
	      signal(N,&catchhandler); 
	      if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --catch.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(long_options[option_index].name, 1);
			}
		    }
	       break;
	case 'i':
	       if (verbose_flag)
		 printf("--%s %s\n",long_options[option_index].name, optarg);
	       
	       N = atoi(optarg);
	       if (N <= 0 || N > 64)
		 {
		   fprintf(stderr,"signal %d does not exist\n", N);
		   finalexit = 1;
		   break;
		 }
	       if (signal(N,&ignorehandler) == SIG_ERR)
		 {
		   fprintf(stderr, "ignore error: %s\n",strerror(errno));
		   finalexit = 1;
		   break;
		 }
	       if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --ignore.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(long_options[option_index].name, 1);
			}
		    }
	       break;
	case 'd':
	  	if (verbose_flag)
		 printf("--%s %s\n",long_options[option_index].name, optarg);
	       
	       N = atoi(optarg);
	       if (N <= 0 || N > 64)
		 {
		   fprintf(stderr,"signal %d does not exist\n", N);
		   finalexit = 1;
		   break;
		 }
	       if (signal(N,SIG_DFL) == SIG_ERR)
		 {
		   fprintf(stderr, "ignore error: %s\n",strerror(errno));
		   finalexit = 1;
		   break;
		 }
	       if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --default.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(long_options[option_index].name, 1);
			}
		    }
	       break;
	case 'q':
	  pause();
	  if (profile_flag)
		    {
		      if (getrusage(RUSAGE_SELF, &usage[1]) < 0)
			{
			  fprintf(stderr, "Error: --profile failed for --pause.\n");
			  finalexit = 1;
			  getrusagecheck = 0;
			}
		      if (getrusagecheck)
			{ 
			  printoptions(long_options[option_index].name, 1);
			}
		    }
	  break;
        case '?':
	default:
	  fprintf(stderr, "unrecognized option %s\n", argv[option_index]);
	  finalexit = 1;
	}
    }

  if (waitflag == 0)
     {  
        waitflag = 1;
        for (int j=0; j < fdind; j++)
	   close(fdtable[j]);
      } 

      if (profile_flag)
	{
	  if (getrusage(RUSAGE_SELF, &usage[3]) < 0)
	    {
	      fprintf(stderr, "Error: --profile failed for.\n");
	      finalexit = 1;
	    }
	  else
	    {
	      printoptions("parent", 3);
	      printoptions("total for parent and children processes", 4);
	    }

	}

     freemem();
     free(argarrptr);
     exit(finalexit);
}

int startcmd(char* cmd, int* fdstream, char* argv[])
{
  int dup_status;
  int exec_status; 
  pid_t pid = fork();

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
	  fprintf (stderr, "error in redirection stdin \n");
	  freemem();
	  exit(-1);
	}
      dup_status = dup2(fdtable[fdstream[1]],1); 
      if (dup_status == -1)
	{
	  fprintf (stderr, "error in redirection stdout\n");
	  freemem();
	  exit(-1);
	}
      dup_status = dup2(fdtable[fdstream[2]],2); 
      if (dup_status == -1)
	{
	  fprintf (stderr, "error in redirection stderr\n");
	  freemem();
	  exit(-1);
	}
      //close file descriptors otherwise pipes will hang accordin to the TA Bu Z.
   for (int j=0; j < fdind; j++)
     close(fdtable[j]);

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
      /*
      printf("----------------------------\n");      
      int f = 0;
      while(cldptr[cldind-1].cldarg[f] != NULL)
	{
	  printf("cldptr[%d].cldarg[%d]: %s\n",cldind-1,f,cldptr[cldind-1].cldarg[f]);
	  f++;
	}
      f = 0;
      printf("cldptr[%d].maxarglen: %d\n",cldind-1,cldptr[cldind-1].maxarglen);
      printf("----------------------------\n");            
      */    
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
      //      printf("in coppystrarr argv[%d]: %s\n", i, argv[i]);
      maxarrlen++;
      i++;
    }  

  maxarrlen++;
  // printf("maxarrlen: %d\n", maxarrlen);
  temparr = (char**)malloc(sizeof(char*)*maxarrlen);

  

  
  for (int i = 0; argv[i] != NULL; i++)
    {
      /*
      for(int g = 0; argv[g] != NULL; g++)
	{
	   printf("in coppystrarr 2  argv[%d]: %s\n", g, argv[g]);
	}
      */
	  currstrlen = strlen(argv[i]);	  
	  temparr[i] = (char*)malloc(sizeof(char)*(currstrlen + 1));
	  strcpy(temparr[i],argv[i]);
	  /*
	  for (int j = 0; j <= currstrlen; j++)
	    {
		 temparr[i][j] = argv[i][j];
		 printf("inside for loop temparr[%d][%d]: %c\n",i,j, temparr[i][j]);
	    }
	  */
	  //	  	  printf("currstrlen: %d\n temparr[%d]: %s\n", currstrlen, i, temparr[i]);
	
    }
  
  temparr[maxarrlen-1]=NULL;    
  return temparr;
}


// in the man pages I found this 
//In addition, zero or more file creation flags and file status flags
//can be bitwise-or'd in flags
int pipeflags()
{
 return  (cloexec_flag & O_CLOEXEC)| (nonblock_flag & O_NONBLOCK);
}

//apparently flags can be used once by the next option
void resetflags()
{
   cloexec_flag = 0;
   nonblock_flag = 0;
   append_flag = 0;
   creat_flag = 0;
   directory_flag = 0;
   dsync_flag = 0;
   excl_flag = 0;
   nofollow_flag = 0;
   rsync_flag = 0;
   sync_flag = 0;
   trunc_flag = 0;

}

int setflags()
{
  return (append_flag & O_APPEND)     | (cloexec_flag & O_CLOEXEC)     |
    (creat_flag & O_CREAT)       | (directory_flag & O_DIRECTORY) |
    (dsync_flag & O_DSYNC)       | (excl_flag & O_EXCL)           |
    (nofollow_flag & O_NOFOLLOW) | (nonblock_flag & O_NONBLOCK)   |
    (rsync_flag & O_RSYNC)       | (sync_flag & O_SYNC)           |
    (trunc_flag & O_TRUNC);
}

void sigsegvhandler(int signal)
{
  int* nullpoint = NULL;
  char errormessage[] = "aborted with segmentation error\n";
  write(2,errormessage,strlen(errormessage));
  *nullpoint = 1;
}

void catchhandler(int signal)
{
  char buf[100];
  int rem;
  int base = 10;
  int i = 0;
  int num = signal;


  while(num != 0)
    {
      rem = num % base;
      buf[i++] = (rem > 9)? (rem-10)+'A':rem+'0';
      num = num/base;
    }
  buf[i] = '\0';
  int start = 0;
  int end = i - 1;
  char temp;    
       while(start < end)
	 {
	   temp = buf[start];
	   buf[start] = buf[end];
	   buf[end]=temp;
	   start++;
	   end--;
	 }
  strcat(buf," caught");
  write(2,buf,strlen(buf));
  exit(signal);
}

void ignorehandler(int signal)
{
  if (signal)
    {
      ;
    }
}


 void printoptions(const char* name, int usageindex)
{
  if (usageindex == 1) //
    {
      printf("Profile of --%s\n", name);
      printf("user cpu time:                   %ld.%06lds\n",usage[1].ru_utime.tv_sec - usage[0].ru_utime.tv_sec, usage[1].ru_utime.tv_usec - usage[0].ru_utime.tv_usec);
      printf("system cpu time:                 %ld.%06ld\n", usage[1].ru_stime.tv_sec - usage[0].ru_stime.tv_sec, usage[1].ru_stime.tv_usec - usage[0].ru_stime.tv_usec);
      printf("max resident size:               %ldkB\n", usage[1].ru_maxrss);
      printf("page reclaims(soft page faults): %ld\n", usage[1].ru_minflt);
      printf("page faults(hard page faults:    %ld\n", usage[1].ru_majflt);
      printf("block input operations:          %ld\n", usage[1].ru_inblock);
      printf("block output operations:         %ld\n", usage[1].ru_oublock);
      printf("voluntary context switches:      %ld\n", usage[1].ru_nvcsw);
      printf("involuntary context switches:    %ld\n\n", usage[1].ru_nivcsw);
    }
  else if (usageindex == -1) // printf for --command
    {
      printf("Profile of --command %s\n", name);
      printf("user cpu time:                   %ld.%06lds\n",usage[1].ru_utime.tv_sec - usage[0].ru_utime.tv_sec, usage[1].ru_utime.tv_usec - usage[0].ru_utime.tv_usec);
      printf("system cpu time:                 %ld.%06ld\n", usage[1].ru_stime.tv_sec - usage[0].ru_stime.tv_sec, usage[1].ru_stime.tv_usec - usage[0].ru_stime.tv_usec);
      printf("max resident size:               %ldkB\n", usage[1].ru_maxrss);
      printf("page reclaims(soft page faults): %ld\n", usage[1].ru_minflt);
      printf("page faults(hard page faults:    %ld\n", usage[1].ru_majflt);
      printf("block input operations:          %ld\n", usage[1].ru_inblock);
      printf("block output operations:         %ld\n", usage[1].ru_oublock);
      printf("voluntary context switches:      %ld\n", usage[1].ru_nvcsw);
      printf("involuntary context switches:    %ld\n\n", usage[1].ru_nivcsw);
    }
  else if (usageindex == 2 || usageindex == 3)
    {
      printf("Profile of %s processes\n", name); 
      printf("user cpu time:                   %ld.%06lds\n", usage[usageindex].ru_utime.tv_sec, usage[usageindex].ru_utime.tv_usec);
      printf("system cpu time:                 %ld.%06lds\n", usage[usageindex].ru_stime.tv_sec, usage[usageindex].ru_stime.tv_usec);
      printf("max resident size:               %ldkB\n", usage[usageindex].ru_maxrss);
      printf("page reclaims(soft page faults): %ld\n", usage[usageindex].ru_minflt);
      printf("page faults(hard page faults:    %ld\n", usage[usageindex].ru_majflt);
      printf("block input operations:          %ld\n", usage[usageindex].ru_inblock);
      printf("block output operations:         %ld\n", usage[usageindex].ru_oublock);
      printf("voluntary context switches:      %ld\n", usage[usageindex].ru_nvcsw);
      printf("involuntary context switches:    %ld\n\n", usage[usageindex].ru_nivcsw);
    }
  else if (usageindex == 4)
    {
      printf("Profile of %s\n", name); 
      printf("user cpu time:                   %ld.%06lds\n", usage[2].ru_utime.tv_sec + usage[3].ru_utime.tv_sec, usage[2].ru_utime.tv_usec + usage[3].ru_utime.tv_usec);
      printf("system cpu time:                 %ld.%06lds\n", usage[2].ru_stime.tv_sec + usage[3].ru_stime.tv_sec, usage[2].ru_stime.tv_usec + usage[3].ru_stime.tv_usec);
      printf("max resident size:               %ldkB\n", usage[2].ru_maxrss + usage[3].ru_maxrss);
      printf("page reclaims(soft page faults): %ld\n", usage[2].ru_minflt + usage[3].ru_minflt);
      printf("page faults(hard page faults:    %ld\n", usage[2].ru_majflt + usage[3].ru_majflt);
      printf("block input operations:          %ld\n", usage[2].ru_inblock + usage[3].ru_inblock);
      printf("block output operations:         %ld\n", usage[2].ru_oublock + usage[3].ru_oublock);
      printf("voluntary context switches:      %ld\n", usage[2].ru_nvcsw + usage[3].ru_nvcsw);
      printf("involuntary context switches:    %ld\n\n", usage[2].ru_nivcsw + usage[3].ru_nivcsw);
    }
  
}

