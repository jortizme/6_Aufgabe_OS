#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define error_fatal(x) fprintf(stderr,x)

//enum Argument {YES, NO, UNKNOWN};


typedef struct
{
    unsigned int numberArguments;
    bool DestinationPathThere;
    char* PathName;
    char* Arguments;
    
}CommandInfo;


//void book_example();

int main(int argc, char *argv[])
{
    if(argc < 1)
        error_fatal("Too few arguments");   //Swachsinn
    
    CommandInfo Info;
    int Argcnt = 0;

    if(argc == 1)
    {
        Info.numberArguments = 0;
        Info.DestinationPathThere = false;
        Info.PathName = NULL;
        Info.Arguments = NULL;
    }
    else if (argc == 2)
    {
        char* Argvaux;

        if((Argvaux = strchr(argv[1],'-')) == NULL)
        {
            Info.numberArguments = 0;
            Info.DestinationPathThere = true;
            Info.PathName = argv[1];
            Info.Arguments = NULL;
        }
        else
        {
            //no path entered
            Argcnt = strlen(argv[1]) - 1;

            Info.Arguments = (char*)malloc((sizeof(char)*Argcnt) + 1); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            *Info.Arguments = 0;
            Argvaux++;
            
            strcat(Info.Arguments,Argvaux);
            //Info.Arguments = Argvaux;
        
            Info.numberArguments = Argcnt;
            Info.DestinationPathThere = false;
            Info.PathName = NULL;
        }
        
    }
    else
    {
        if (strchr(argv[argc-1],'/') == NULL && strchr(argv[argc-1],'.') == NULL)
        {  
            int cnt;
            Info.DestinationPathThere = false;
            Info.PathName = NULL;
            Argcnt = argc - 1;

            Info.Arguments = (char*)malloc((sizeof(char)*Argcnt) + 1); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            *Info.Arguments = 0;
            
            for(cnt = 1; cnt <= argc - 1; cnt++)
            {
                strcat(Info.Arguments,++argv[cnt]);
            }

            Info.numberArguments = strlen(Info.Arguments);
        }

        else 
        {
            int cnt;
            Info.DestinationPathThere = true;
            Info.PathName = argv[argc-1];
            int CharstoAllocate;

            if((argc - 2) == 1)
                CharstoAllocate = 6;
            else
                CharstoAllocate = argc - 2;


            Info.Arguments = (char*)malloc((sizeof(char)*CharstoAllocate) + 1); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            *Info.Arguments = 0;

            for(cnt = 1; cnt <= argc - 2; cnt++)
            {
                //Info.Arguments[cnt-1] = ++argv[cnt - 1];
                strcat(Info.Arguments,++argv[cnt]);
            }

            Info.numberArguments = strlen(Info.Arguments);
        }
        
    }

    return 0;

}

/*
void book_example()
{
 char *basename;
    char *process_cwd;
    char *pointer;
    long path_limit;

    if((basename = strrchr(argv[0],'/')) == NULL)
        basename = argv[0];
    else
        basename++;

    if (argc != 2)
        fprintf(stderr,"Usage: %s <dir>\n", basename);

    if((path_limit = pathconf(argv[1],_PC_PATH_MAX)))
    {
        if(errno != 0)
            fprintf(stderr, "pathconfig failed!\n");
    }
    
    if ((process_cwd = (char*)malloc((size_t)path_limit)) == NULL)
        fprintf(stderr,"malloc error for pathname");
    
    if(chdir(argv[1]) < 0)
        printf("chdir failed\n");
    else
    {
        if((pointer = getcwd(process_cwd, (size_t)path_limit)) == NULL)
            fprintf(stderr,"getcwd failed\n");
        
        printf("%s: %s [%ld]\n",basename, pointer, path_limit);
    }
    
    free(process_cwd);
}
*/