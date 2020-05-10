
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

#define error_several(x) fprintf(stderr,x); exit(EXIT_FAILURE) 

#define control_negative(y,x) if(x == -1){\
                        fprintf(stderr,"%s failed at line %d of file %s (function %s)\n",y,__LINE__,__FILE__,__func__);\
                        exit(EXIT_FAILURE);}

#define control_null(y,x) if(x == NULL){ \
                        fprintf(stderr,"%s failed at line %d of file %s (function %s)\n",y,__LINE__,__FILE__,__func__);\
                        exit(EXIT_FAILURE);}

#define MAXARGAMOUNT 6
#define DEBUG_MODE 1


typedef struct
{
    unsigned int NrArguments;
    bool PathIsThere;
    char* PathName;
    char* Arguments;
    
}CommandInfo;

typedef struct 
{
    DIR *DirStream;
    struct dirent* DirInfo; 
    struct stat buf;
    char* PathNameAux;

}VariablesforSearch;


void parse_arguments(CommandInfo* Info,int argc,char** argv);
void getFilesInfo(VariablesforSearch* forSearch, CommandInfo* Info);

//void book_example();

int main(int argc, char *argv[])
{ 
    CommandInfo Info;
    VariablesforSearch forSearch;

    forSearch.PathNameAux = (char*)malloc(sizeof(char)*60);

    parse_arguments(&Info,argc,argv);

    getFilesInfo(&forSearch, &Info);

    free(forSearch.PathNameAux);
    free(Info.Arguments);
    exit(EXIT_SUCCESS);
}

void parse_arguments(CommandInfo* Info,int argc,char** argv)
{
    if(argc == 1)
    {
        Info->NrArguments = 0;
        Info->PathIsThere = false;
        Info->PathName = ".";
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
            Info->PathIsThere = false;
            Info->PathName = ".";
            loopLimit = argc - 1;
        }
        //There is a path 
        else 
        {
            Info->PathIsThere = true;
            Info->PathName = argv[argc-1];
            strcat(Info->PathName,"/");
            loopLimit = argc - 2;

            //This is the case, when only the path is available
            if(argc == 2)
            {
                Info->NrArguments = 0;
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
        Info->NrArguments = strlen(Info->Arguments);   
    }

#if DEBUG_MODE == 1
    printf("Number Arguments : %u\n", Info->NrArguments);
    printf("Destination path there : %s\n", Info->PathIsThere ? "TRUE" : "FALSE");
    printf("Path name: %s\n", Info->PathName);
    printf("Arguments type: %s\n\n", Info->Arguments);
#endif

}

void getFilesInfo(VariablesforSearch* forSearch, CommandInfo* Info)
{
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
            

#if DEBUG_MODE == 1
        printf("Name der Datei: %s\n",forSearch->DirInfo->d_name);
        printf("Seriennummer der Datei: %ld\n",forSearch->DirInfo->d_ino);
        printf("ID of device containing file: %ld\n",forSearch->buf.st_dev);
        printf("Inode number: %ld\n", forSearch->buf.st_ino);
        printf("Protection: %d\n",forSearch->buf.st_mode);
        printf("Number of hard links : %ld\n",forSearch->buf.st_nlink);
        printf("User ID of owner : %d\n", forSearch->buf.st_uid);
        printf("Group ID of owner : %d\n", forSearch->buf.st_gid);
        printf("Total size, in bytes : %ld\n", forSearch->buf.st_size);
        printf("Blocksize for file system I/O : %ld\n", forSearch->buf.st_blksize);
        printf("Time of last access : %ld\n", forSearch->buf.st_atime);
        printf("Time of last modification : %ld\n", forSearch->buf.st_mtime);
        printf("time of last status change : %ld\n\n", forSearch->buf.st_ctime);
#endif
        
    }
        
    control_negative("closedir()",closedir(forSearch->DirStream));

    if(errno != 0)
        error_several("errno unequal 0, error at readdir()");
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