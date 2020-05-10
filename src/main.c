#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define error_several(x) fprintf(stderr,x); exit(EXIT_FAILURE);
#define MAXARGAMOUNT 6

#define DEBUG_MODE 0

typedef struct
{
    unsigned int numberArguments;
    bool DestinationPathThere;
    char* PathName;
    char* Arguments;
    
}CommandInfo;

void parse_arguments(CommandInfo* Info,int argc,char** argv);

//void book_example();

int main(int argc, char *argv[])
{ 
    CommandInfo Info;

    parse_arguments(&Info,argc,argv);

#if DEBUG_MODE == 1
    printf("Number Arguments : %u\n", Info.numberArguments);
    printf("Destination path there : %s\n", Info.DestinationPathThere ? "TRUE" : "FALSE");
    printf("Path name: %s\n", Info.PathName);
    printf("Arguments type: %s\n", Info.Arguments);
#endif

    free(Info.Arguments);
    exit(EXIT_SUCCESS);
}

void parse_arguments(CommandInfo* Info,int argc,char** argv)
{
    if(argc == 1)
    {
        Info->numberArguments = 0;
        Info->DestinationPathThere = false;
        Info->PathName = NULL;
        Info->Arguments = NULL;
    }
    else
    {
        unsigned int cnt;
        Info->Arguments = (char*)malloc(sizeof(char)*MAXARGAMOUNT);
        *Info->Arguments = 0;
        unsigned int loopLimit;

        //There was no path given
        if (strchr(argv[argc-1],'/') == NULL && strchr(argv[argc-1],'.') == NULL)
        {  
            Info->DestinationPathThere = false;
            Info->PathName = NULL;
            loopLimit = argc - 1;
        }
        //There is a path 
        else 
        {
            Info->DestinationPathThere = true;
            Info->PathName = argv[argc-1];
            loopLimit = argc - 2;

            //This is the case, when only the path is available
            if(argc == 2)
            {
                Info->numberArguments = 0;
                Info->Arguments = NULL;
                return;
            }
        }
        for(cnt = 1; cnt <= loopLimit; cnt++)
            {
                if(strchr(argv[cnt],'-') == NULL)
                {
                    error_several("Arguments must be started by the '-' symbol\n");
                }

                strcat(Info->Arguments,++argv[cnt]);
            }
        Info->numberArguments = strlen(Info->Arguments);   
    }

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