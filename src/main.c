
#define _POSIX_C_SOURCE 200112L
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define error_several(x) fprintf(stderr,x); exit(EXIT_FAILURE) 

#define control_negative(y,x) if(x == -1){\
                        fprintf(stderr,"%s failed at line %d of file %s (function %s)\n",y,__LINE__,__FILE__,__func__);\
                        exit(EXIT_FAILURE);}

#define control_null(y,x) if(x == NULL){ \
                        fprintf(stderr,"%s failed at line %d of file %s (function %s)\n",y,__LINE__,__FILE__,__func__);\
                        exit(EXIT_FAILURE);}

#define MAX_ARG_AMOUNT 6
#define MAX_PATH_LENGTH 128
#define DEBUG_MODE 0


typedef struct CommandInfo
{
    unsigned int NrArguments;
    bool PathIsThere;
    char PathName[MAX_PATH_LENGTH];
    char Arguments[MAX_ARG_AMOUNT];
    
}CommandInfo;

typedef struct VariablesforSearch
{
    DIR *DirStream;
    struct dirent* DirInfo; 
    struct stat buf;
    char PathNameAux[MAX_PATH_LENGTH];

}VariablesforSearch;

typedef struct CheckForArguments
{
    bool a_isThere;
    bool o_isThere;
    bool l_isThere;
    bool g_isThere;

}CheckForArguments;

void parse_arguments(CommandInfo* Info,int argc,char** argv);
void getFilesInfo(VariablesforSearch* forSearch, CommandInfo* Info, CheckForArguments *Arguments);

int main(int argc, char *argv[])
{ 
    CommandInfo Info;
    VariablesforSearch forSearch;
    CheckForArguments Arguments = {.a_isThere = false, .o_isThere = false, .l_isThere = false, .g_isThere = false};

    for(int i = 0; i < MAX_ARG_AMOUNT; i++)
    {
        Info.Arguments[i] = 0;
    }

    parse_arguments(&Info,argc,argv);

    getFilesInfo(&forSearch, &Info, &Arguments);

    exit(EXIT_SUCCESS);
}

void parse_arguments(CommandInfo* Info,int argc,char** argv)
{
    if(argc == 1)
    {
        Info->NrArguments = 0;
        Info->PathIsThere = false;
        strcpy(Info->PathName, ".");
    }
    else
    {
        unsigned int cnt;
        unsigned int loopLimit;

        //There was no path given
        if (strchr(argv[argc-1],'/') == NULL && strchr(argv[argc-1],'.') == NULL)
        {  
            Info->PathIsThere = false;
            strcpy(Info->PathName, ".");
            loopLimit = argc - 1;
        }
        //There is a path 
        else 
        {
            Info->PathIsThere = true;
            strcpy(Info->PathName, argv[argc-1]);
            strcat(Info->PathName,"/");
            loopLimit = argc - 2;

            //This is the case, when only the path is available
            if(argc == 2)
            {
                Info->NrArguments = 0;
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
        Info->NrArguments = strlen(Info->Arguments);

        if(Info->NrArguments > 4)
        {
            error_several("Too much arguments given, maximal 4 are allowed\n");
        }   
    }

#if DEBUG_MODE == 1
    printf("Number Arguments : %u\n", Info->NrArguments);
    printf("Destination path there : %s\n", Info->PathIsThere ? "TRUE" : "FALSE");
    printf("Path name: %s\n", Info->PathName);
    printf("Arguments type: %s\n\n", Info->Arguments);
#endif

}

void getFilesInfo(VariablesforSearch* forSearch, CommandInfo* Info, CheckForArguments *Arguments)
{
    
    char* Argaux = Info->Arguments;

    for(int i = 0; i < Info->NrArguments; i++)
    {
        switch (*Argaux)
        {
        case 'a':  Arguments->a_isThere = true; break;
        case 'l':  Arguments->l_isThere = true; break;
        case 'g':  Arguments->g_isThere = true; break;
        case 'o':  Arguments->o_isThere = true; break;
        default:
            break;
        }
        Argaux++;
    }

    forSearch->DirStream = opendir(Info->PathName);
    control_null("opendir()",forSearch->DirStream);
        
    errno = 0;

    //every file and directory that is contained in the given path is going to be shown
    while((forSearch->DirInfo = readdir(forSearch->DirStream)) != NULL)
    {
        if(Info->PathIsThere == true)
        {
            strcpy(forSearch->PathNameAux,Info->PathName);
            strcat(forSearch->PathNameAux,forSearch->DirInfo->d_name);
            control_negative("lstat()",lstat(forSearch->PathNameAux,&forSearch->buf));
                
        }
        else
        {
            control_negative("lstat()",lstat(forSearch->DirInfo->d_name,&forSearch->buf));
        }

        //if a .* file there and -a is no set, then continue to the next iteration
        if( *forSearch->DirInfo->d_name == '.' && Arguments->a_isThere == false )
        {
            continue;
        }

        char* FileType = " ";

        switch (forSearch->buf.st_mode & __S_IFMT) 
        {
        case __S_IFDIR:  FileType = "d";  break;
        case __S_IFLNK:  FileType = "l";  break;
        case __S_IFREG:  FileType = "-";  break;
        default:                        break;
        }
        //printf("Seriennummer der Datei: %ld\n",forSearch->DirInfo->d_ino);

        Argaux = Info->Arguments;

        if(Arguments->a_isThere)
        {
            if(*Argaux == 'a')
                Argaux++;
                    
        }

        if(Arguments->l_isThere || Arguments->g_isThere || Arguments->o_isThere)
        {
            bool PrintIDOwner = true;
            bool PrintIDGroup = true;

            printf("%s\t",FileType);                 //Type of file 
            printf("%d\t",forSearch->buf.st_mode);   //Protection: 
            printf("%ld\t",forSearch->buf.st_nlink); //Number of hard links : 
            printf("%ld\t", forSearch->buf.st_size);  //Total size, in bytes : 
            //printf("Blocksize for file system I/O : %ld\n", forSearch->buf.st_blksize);
            printf("%ld\t", forSearch->buf.st_atime);  //Time of last access : 
            printf("%ld\t", forSearch->buf.st_mtime);    //Time of last modification : 
            printf("%ld\t", forSearch->buf.st_ctime);    //time of last status change : 

            switch (*Argaux)
            {
            case 'g':   PrintIDOwner = false; break;
            case 'o':   PrintIDGroup = false;  break;
            default:     break;
            }

            if(PrintIDGroup)
                printf("%d\t", forSearch->buf.st_uid);   //User ID of owner : 
            
            if(PrintIDOwner)
                printf("%d\t", forSearch->buf.st_gid);   //Group ID of owner : 
                
        }

        printf("%s\t",forSearch->DirInfo->d_name);   //Name der Datei

        printf("\n");
    }

    control_negative("closedir()",closedir(forSearch->DirStream));

    if(errno != 0)
        error_several("errno unequal 0, error at readdir()");
}
