
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

#define ErrorSeveral(x) fprintf(stderr,x); exit(EXIT_FAILURE) 

#define ControlReturnNegative(y,x) if(x == -1){\
                        fprintf(stderr,"%s failed at line %d of file %s (function %s)\n",y,__LINE__,__FILE__,__func__);\
                        exit(EXIT_FAILURE);}

#define ControlReturnNULL(y,x) if(x == NULL){ \
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

    printf("\n\n");

    exit(EXIT_SUCCESS);
}

void parse_arguments(CommandInfo* Info,int argc,char** argv)
{
    //if no arguments and no path given
    if(argc == 1)
    {
        Info->NrArguments = 0;
        Info->PathIsThere = false;
        strcpy(Info->PathName, ".");
    }
    //if there was something given
    else
    {
        unsigned int cnt;
        unsigned int loopLimit;

        //if there was no path given
        if (strchr(argv[argc-1],'/') == NULL && strchr(argv[argc-1],'.') == NULL)
        {  
            Info->PathIsThere = false;
            strcpy(Info->PathName, ".");
            loopLimit = argc - 1;
        }
        //if there is a path 
        else 
        {
            Info->PathIsThere = true;
            strcpy(Info->PathName, argv[argc-1]);
            strcat(Info->PathName,"/");
            loopLimit = argc - 2;

            //This is the case, when only the path is available and no arguments
            if(argc == 2)
            {
                Info->NrArguments = 0;
                return;
            }
        }
        for(cnt = 1; cnt <= loopLimit; cnt++)
            {   
                //make sure the arguments start with the '-' symbol
                if(strchr(argv[cnt],'-') == NULL)
                {
                    ErrorSeveral("Arguments must be started by the '-' symbol\n");
                }
                
                //the ++ allows us to ignore the '-' symbol from the argument
                strcat(Info->Arguments,++argv[cnt]);
            }
        Info->NrArguments = strlen(Info->Arguments);

        if(Info->NrArguments > 4)
        {
            ErrorSeveral("Too much arguments given, maximal 4 are allowed\n");
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
    //Argaux created in order to let Info->Arguments unchanged for later use
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

    //open directory
    forSearch->DirStream = opendir(Info->PathName);
    ControlReturnNULL("opendir()",forSearch->DirStream);
        
    errno = 0;
    int iterator = 0;

    //every file/directory contained in the path will be shown
    while((forSearch->DirInfo = readdir(forSearch->DirStream)) != NULL)
    {
        iterator++;
        if(Info->PathIsThere == true)
        {
            strcpy(forSearch->PathNameAux,Info->PathName);
            strcat(forSearch->PathNameAux,forSearch->DirInfo->d_name);
            ControlReturnNegative("lstat()",lstat(forSearch->PathNameAux,&forSearch->buf));
        }
        //if there is no path, it means we look for infos in the current directory, this is 
        //why the d_name is given in lstat() directly and not with a followed concatenated path
        else
        {
            ControlReturnNegative("lstat()",lstat(forSearch->DirInfo->d_name,&forSearch->buf));
        }

        //ignore the .* files when -a argument not set
        if( *forSearch->DirInfo->d_name == '.' && Arguments->a_isThere == false )
        {
            iterator--;
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
            //Argaux points to the l,o or g argument when a available
            while(*Argaux == 'a')
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

        

        if(!Arguments->l_isThere && !Arguments->g_isThere && !Arguments->o_isThere)
        {
            printf("%-16s\t\t",forSearch->DirInfo->d_name);   //Name der Datei

            if(iterator % 3 == 0)
                printf("\n");
        }
        else
        {
            printf("%s\t",forSearch->DirInfo->d_name);   //Name der Datei
            printf("\n");
        }
    }

    printf("\n");

    ControlReturnNegative("closedir()",closedir(forSearch->DirStream));

    //check if the rvalue NULL from readdir() was an error
    if(errno != 0)
        ErrorSeveral("errno unequal 0, error at readdir()");
    
}
