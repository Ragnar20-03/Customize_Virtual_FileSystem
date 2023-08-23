#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>  
// #include<io.h>

#define MAXINODE 50

#define READ 1 
#define WRITE 2

#define MAXFILESIZE 2048

#define REGURAL 1 
#define SPECIAL 2

#define START 0 
#define CURRENT 1 
#define END 2

typedef struct superblock
{
    int TotalInodes;
    int FreeInode;
}SUPERBLOCK , *PSUPERBLOCK;

typedef struct inode 
{
    char FileName[256];
    int InodeNumber ; 
    int FileSize ; 
    int FileActualSize;
    int FileType ; 
    char * Buffer ; 
    int LinkCount ; 
    int ReferenceCount ; 
    int permission ; // 1  2  3 
    struct inode * next ; 
}INODE , *PINODE , *PPINODE;

typedef struct filetable
{
    int readoffset ; 
    int writeoffset ; 
    int count ; 
    int mode ;//1 2 3
    PINODE ptrinode; 
}FILETABLE , *PFILETABLE;

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];             //   GLobal Array
SUPERBLOCK SUPERBLOCKobj;  // GLobal structure Object ;
PINODE head = NULL;        // Gobal Linked List Head 

void man(char * name)
{
    if (name == NULL) return ;

    if (strcmp (name , "create") == 0 )
    {
        printf("Description : Used to create new regural file \n");
        printf("Usage : create File_name Permission \n");
    }
    else if (strcmp(name , "read" )  == 0)
    {
        printf("Description : Used to Read the data new regural file \n");
        printf("Usage : read File_name No_Of_Bytes_To_Read \n");
    }
    else if (strcmp(name , "write") == 0)
    {
        printf("Description : Used to write in regural file \n");
        printf("Usage : write File_Name\n After this enter the data that we want to write\n");
    }
    else if (strcmp(name , "ls") == 0 )
    {
        printf("Description : Used to list all files \n");
        printf("Usage : ls\n");
    }
    else if (strcmp(name , "fstat") == 0)
    {
        printf("Description : Used to display information of a file\n");
        printf("Usage : stat File_Name");
    }
    else if (strcmp(name , "fstat") == 0 )
    {
        printf("Description  : used to display information about File \n");
        printf("Usage : stat File_Descriptior\n");
    }
    else if (strcmp(name , "truncate") == 0)
    {
        printf("Description : used to remove data from file\n");
        printf("Usage : truncate File_Name\n");
    }
    else if (strcmp(name , "open") == 0 )
    {
        printf("Description : Used to open an Existiong file\n");
        printf("Usage : open _File_Name mode\n");
    }
    else if (strcmp(name , "close") == 0 )
    {
        printf("Description : Used to close an Exsitiong File\n");
        printf("Usage : close File_name\n");
    }
    else if (strcmp ( name , "closeall") == 0 )
    {
        printf("Description : Used to close all the opened files\n");
        printf("Usage : closeall\n");
    }
    else if (strcmp (name , "lseek") == 0 )
    {
        printf("Description : Used to change file offset\n");
        printf("Usage : lssek File_Name ChangeInOfset StartPoint\n");
    }
    else if (strcmp(name , "rm") == 0)
    {
        printf("Description : Used to delete Files \n " );
        printf("Usage : rm File_Name\n ");
    }
    else 
    {
        printf("No Manual Entery available !!\n");
    }
}

void DisplayHelp()
{
    printf("ls : To list out all files \n");
    printf("clear : To clear console \n");
    printf("open :To open the file \n");
    printf("close :To close the file \n");
    printf("closeall :To close all open files \n");
    printf("read :To read the  file \n");
    printf("write :To write into  the  file \n");
    printf("exit :To Terminate the file System \n");
    printf("stat :To Display information of a file using name \n");
    printf("fstat :To Display information of a file using file descriptor \n");
    printf("truncate :To remove the data from the file \n");
    printf("rm :To Delete the file \n");
}

int GetFdFromName ( char * name)
{
    int i = 0 ; 

    while (i < 50)
    {
        if (UFDTArr[i].ptrfiletable != NULL)
            if (strcmp ((UFDTArr[i].ptrfiletable->ptrinode->FileName) , name) == 0  )
                break;
        i++;
    }

    if (i == 50)    return -1;
    else        return i;
}

PINODE Get_Inode(char * name )
{
    PINODE temp = head ; 
    int i = 0 ;

    if (name == NULL)
        return NULL;
    
    while (temp != NULL)
    {
        if (strcmp(name , temp-> FileName) == 0)
            break;
        temp = temp -> next;
    }
    return temp;
}

void CreateDILB()
{
    int i = 1; 
    PINODE newn= NULL;
    PINODE temp = head;

    while (i<= MAXINODE)
    {
        newn = (PINODE)malloc(sizeof(INODE));

            newn -> LinkCount = 0 ; 
            newn -> ReferenceCount = 0 ; 
            newn -> FileType = 0 ; 
            newn -> FileSize = 0 ; 

                newn -> Buffer = NULL;
                newn -> next = NULL;
            
            newn -> InodeNumber = i;
           
            if (temp == NULL )
            {
                head = newn ;
                temp = head;
            }
            else 
            {
                temp -> next = newn;
                temp = temp -> next;
            }
            i++;
    }
    printf("DILB Created succesfully \n");
}

void InitialiseSuperBlock()
{
    int i = 0 ;
    while (i < MAXINODE)
    {
        UFDTArr[i].ptrfiletable == NULL;
        i++;
    }

    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInode = MAXINODE;
}

int CreateFile(char * name , int permission)
{
    int i = 0 ; 
    PINODE temp = head;

    if(name == NULL || (permission == 0 ) || (permission > 3))
        return -1;
    
    if (SUPERBLOCKobj.FreeInode == 0 )
        return -2;
    
    (SUPERBLOCKobj.FreeInode) -- ;

    if (Get_Inode(name) != NULL)
        return -3;
    
    while(temp != NULL)
    {
        if (temp -> FileType == 0)
            break;
        temp = temp -> next;
    }

    while (i<50)
    {
        if (UFDTArr[i].ptrfiletable == NULL)
            break;
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE) malloc(sizeof(FILETABLE));

    UFDTArr[i].ptrfiletable -> count = 1;
    UFDTArr[i].ptrfiletable -> mode = permission ;
    UFDTArr[i].ptrfiletable -> readoffset = 0 ;
    UFDTArr[i].ptrfiletable -> writeoffset = 0  ;

    UFDTArr[i].ptrfiletable -> ptrinode = temp;

    strcpy(UFDTArr[i].ptrfiletable-> ptrinode -> FileName , name);
    UFDTArr[i].ptrfiletable -> ptrinode -> FileType = REGURAL;
    UFDTArr[i].ptrfiletable-> ptrinode -> ReferenceCount = 1 ;
    UFDTArr[i].ptrfiletable-> ptrinode -> LinkCount = 1 ; 
    UFDTArr[i].ptrfiletable->ptrinode -> FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable -> ptrinode->FileActualSize = 0 ; 
    UFDTArr[i].ptrfiletable -> ptrinode -> permission = permission;
    UFDTArr[i].ptrfiletable -> ptrinode -> Buffer = (char * ) malloc(MAXFILESIZE);

    return i;  // Returning file Descriptor    
}

// Remove File i.e. rm
int rm_File(char * name)
{
    int fd = 0 ; 

    fd = GetFdFromName(name);
    if (fd == -1)
        return -1;
    
    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount) -- ;

    if (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
    {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0 ; 
        // freeall all fielda memory Ahead
        free(UFDTArr[fd].ptrfiletable); 
    }

    UFDTArr[fd].ptrfiletable = NULL;
    (SUPERBLOCKobj.FreeInode)--;
}

int ReadFile(int fd , char * arr , int isize)
{
    int read_size = 0 ;

    if (UFDTArr[fd].ptrfiletable == NULL)  return -1;

    if (UFDTArr[fd].ptrfiletable -> mode != READ && UFDTArr[fd].ptrfiletable->mode != READ + WRITE)   return -2;

    if (UFDTArr[fd].ptrfiletable->ptrinode->permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->permission != READ + WRITE)  return -2;

    if (UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize)     return -3;

    if (UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGURAL)    return -4;

    read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);
    if (read_size < isize)
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset) , read_size);

        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset+read_size;
    }
    else 
    {
        strncpy(arr , (UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset) , isize);

        (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + isize;
    }

    return isize;
}

int WriteFile(int fd  , char * arr  , int isize )
{
    if (((UFDTArr[fd].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd].ptrfiletable->mode ) != READ + WRITE ))  return -1;

    if (((UFDTArr[fd].ptrfiletable->ptrinode->permission) != WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->permission) != READ+WRITE))        return -1;

    if ((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE) return -2;

    if ((UFDTArr[fd].ptrfiletable->ptrinode->FileType)!= REGURAL) return -3;

    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->writeoffset) , arr , isize);

    (UFDTArr[fd].ptrfiletable->writeoffset ) = (UFDTArr[fd].ptrfiletable->writeoffset) + isize;

    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize ) + isize;

    return isize; 
}

int OpenFile(char * name , int mode)
{
    int i = 0 ; 
    PINODE temp = NULL;

    if (name == NULL || mode <= 0  )
        return -1;

        temp = Get_Inode(name);
        if (temp == NULL)
            return -2;
        
        if (temp -> permission < mode)
            return -3;
        
        while (i<50)
        {
            if (UFDTArr[i].ptrfiletable == NULL)
                break;
            i++;
        }

        UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
        if (UFDTArr[i].ptrfiletable == NULL)   return -1;
        UFDTArr[i].ptrfiletable->count = 1;
        UFDTArr[i].ptrfiletable->mode = mode;
        if (mode == READ + WRITE)
        {
            UFDTArr[i].ptrfiletable->readoffset=0;
            UFDTArr[i].ptrfiletable->writeoffset=0;            
        }
        else if (mode == READ)
        {
            UFDTArr[i].ptrfiletable->readoffset=0;            
        }        
        else if (mode == READ)
        {
            UFDTArr[i].ptrfiletable->writeoffset=0;            
        }
        UFDTArr[i].ptrfiletable->ptrinode = temp;
        (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

        return i;
}

void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset = 0  ;
    UFDTArr[fd].ptrfiletable->writeoffset = 0  ;
    (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
}

int CloseFileByName(char * name)
{
    int i = 0 ; 
    i = GetFdFromName(name);
    if ( i == -1)
            return -1;
        
    UFDTArr[i].ptrfiletable->readoffset = 0 ;
    UFDTArr[i].ptrfiletable->writeoffset = 0 ;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

    return 0;
}

void CloseAllFile()
{
    int i = 0 ; 
    while (i<50)
    {
        if (UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset = 0 ;
            UFDTArr[i].ptrfiletable->writeoffset = 0 ;
            (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
            // break;
        }
        i++;
    }
}

int LseekFile(int fd , int size  , int from)
{
    if ((fd < 0) || (from > 2))     return -1;
    if (UFDTArr[fd].ptrfiletable == NULL)   return -1;

    if ((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ+WRITE) )
    {
        if (from == CURRENT)
        {
            if (((UFDTArr[fd].ptrfiletable->readoffset)+ size) > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize )   return -1;
            if (((UFDTArr[fd].ptrfiletable->readoffset ) +size ) < 0  ) return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset)+size;
        }
        else if (from == START)
        {
            if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))    return -1;
            if (size < 0)   return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = size;
        }
        else if (from == END)
        {
            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)      return -1;
            if (((UFDTArr[fd].ptrfiletable->readoffset) + size ) < 0  )     return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
    else if (UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if (from == CURRENT )
        {
            if (((UFDTArr[fd].ptrfiletable->writeoffset)+size) > MAXFILESIZE)   return -1;
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size ) < 0 ) return -1;
            if (((UFDTArr[fd].ptrfiletable->writeoffset)+size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset ) +size;
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset)+size;  
        }
        else if (from == START)
        {
            if (size > MAXFILESIZE)     return -1;
            if (size < 0 )  return -1;
            if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
            (UFDTArr[fd].ptrfiletable->writeoffset) = size;
        }
        else if (from == END)
        {
            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size >MAXFILESIZE)      return -1;
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size ) < 0 ) return -1;
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
}

void ls_file()
{
    int i = 0 ; 
    PINODE temp = head;

    if (SUPERBLOCKobj.FreeInode == MAXINODE)
    {
        printf("Error : There are no files\n");
        return ;
    }

    printf("\nFile Name \t Inode number \t File Size \t Link Count\n");
    printf("--------------------------------------------------------------\n");
    while (temp != NULL)
    {
        if (temp ->FileType != 0 )
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount);
        }
        temp = temp -> next;
    }   
    printf("--------------------------------------------------------------\n");
}   

int fstat_file(int fd)
{
    PINODE temp = head;
    int i = 0 ; 

    if (fd < 0) return -1;

    if (UFDTArr[fd].ptrfiletable == NULL) return -2;

    temp = UFDTArr[fd].ptrfiletable -> ptrinode;

    printf("\n---------------Statical Information about File-------------\n");
    printf("File Name : %s\n",temp -> FileName);
    printf("Inode Number : %d\n",temp->InodeNumber);
    printf("File Size : %d\n",temp -> FileSize);
    printf("Actual File Size : %d\n",temp->FileActualSize);
    printf("Link Count : %d\n",temp->LinkCount);
    printf("Reference Count : %d\n",temp->ReferenceCount);

    if (temp -> permission == 1)
        printf("File Permission : Read only \n");
    else if (temp -> permission == 2)
        printf("File Permission : Write \n");
    else if (temp -> permission == 3)
        printf("File Permission : Read & Write\n");
    printf("--------------------------------------------------------------\n\n");

    return 0;
}

int stat_file(char * name)
{
    PINODE temp = head;
    int i = 0 ; 

    if (name == NULL)   return -1;

    while (temp != NULL)
    {
        if (strcmp(name , temp->FileName) == 0 )
            break;
        temp = temp -> next;
    }

    if (temp == NULL)  return -2;

    printf("\n---------------Statical Information about File-------------\n");
    printf("File Name : %s\n",temp -> FileName);
    printf("Inode Number : %d\n",temp->InodeNumber);
    printf("File Size : %d\n",temp -> FileSize);
    printf("Actual File Size : %d\n",temp->FileActualSize);
    printf("Link Count : %d\n",temp->LinkCount);
    printf("Reference Count : %d\n",temp->ReferenceCount);

    if (temp -> permission == 1)
        printf("File Permission : Read only \n");
    else if (temp -> permission == 2)
        printf("File Permission : Write \n");
    else if (temp -> permission == 3)
        printf("File Permission : Read & Write\n");
    printf("--------------------------------------------------------------\n\n");

    return 0;
}

int truncate_File(char * name)
{
    int fd = GetFdFromName(name);
    if (fd== -1)
        return -1;
    
    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer , 0 , 1024);
    UFDTArr[fd].ptrfiletable->readoffset = 0  ;
    UFDTArr[fd].ptrfiletable->writeoffset = 0  ;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0 ;    
}

int main()
{
    char * ptr = NULL;
    int ret = 0  , fd = 0   , count = 0 ; 
    char command[4][80] , str[80],arr[1024];

    InitialiseSuperBlock();
    CreateDILB();

    while (1)
    {
        fflush(stdin);
        strcpy(str , "");

        printf("\n _CVFS : > ");

        fgets(str , 80 , stdin);   //scanf("%[^\n]s" , str);

        count = sscanf(str , "%s %s %s %s " , command[0] , command[1] , command[2], command[3]);

        if (count == 1 )
        {
            if (strcmp(command[0] , "ls") == 0 )
            {
                ls_file();
            }
            else if (strcmp(command[0] , "closeall") == 0)
            {
                CloseAllFile();
                printf("All FIles Closed Succesfully\n");
                continue;
            }
            else if (strcmp(command[0] , "clear") == 0 )
            {
                system("cls");
                continue;
            }
            else if (strcmp(command[0] , "help") == 0 )
            {
                DisplayHelp();
                continue;
            }
            else if (strcmp(command[0] , "exit") == 0 )
            {
                printf("Terminating File Sysytem .....\n");
                break;
            }
            else 
            {
                printf("\n ERROR : Command not found !!!\n");
                continue;
            }
        }
        else if (count == 2)
        {
            if (strcmp(command[0] , "stat") == 0 )
            {
                ret = stat_file(command[1]);
                if (ret == -1)
                    printf("ERROR : Incorrect parameters \n");
                if (ret == -2)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0] , "fstat") == 0 )
            {
                ret = fstat_file(atoi(command[1]));
                if (ret == -1)
                    printf("ERROR : Incorrect parameters \n");
                if (ret == -2)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0] , "close") == 0 )
            {
                ret = CloseFileByName(command[1]);
                if (ret == -1)
                    printf("ERROR : There is no such File\n");
                continue;
            }
            else if (strcmp(command[0] , "rm") == 0 )
            {
                ret = rm_File(command[1]);
                if (ret == -1)
                    printf("There is no such file \n");
                continue;
            }
            else if (strcmp(command[0] , "man") == 0 )
            {
                man (command[1]);
            }
            else if (strcmp(command[0] , "write") == 0 )
            {
                fd = GetFdFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : Incorrect parameter \n");
                    continue;
                }
                printf("Enter Data \n");
                scanf("%[^\n]s" , arr);

                ret = strlen(arr);
                if (ret == 0 )
                {
                    printf("ERROR : Incorrect Paramter \n");
                    continue;
                }
                ret = WriteFile(fd , arr , ret);
                if (ret == -1)
                    printf("ERROR : Permission denied \n");
                if (ret == -2)
                    printf("ERROR : There is no such file \n");
                if (ret == -3)
                    printf("ERROR : It is not a Regural File \n");
            }
            else if (strcmp(command[0] , "truncate") == 0 )
            {
                ret = truncate_File(command[1]);
                if (ret == -1)
                    printf("ERROR : Incorrect Paramter\n");
            }
            else 
            {
                printf("ERROR : Command not found !!!\n");
                continue;
            }
        }
        else if (count == 3)
        {
            if (strcmp(command[0] , "create") == 0 )
            {
                ret = CreateFile(command[1] , atoi(command[2]));
                if (ret >= 0 )
                    printf("File is Succesfully Created with file descriptor : %d \n" , ret);
                if (ret == -1)
                    printf("ERROR : Incorrect parameters\n");
                if (ret == -2)
                    printf("ERROR : There is Not enough INODES \n");
                if (ret == -3)
                    printf("ERROR : File already exists \n");
                if (ret == -4)
                    printf("ERROR : Memoty Allocation Failure\n");
                continue;
            }
            else if (strcmp(command[0] , "open") == 0)
            {
                ret = OpenFile(command[1] , atoi(command[2]) == 0 );
                if (ret >=0)
                    printf("File is succesfully opened with file descriptor : %d \n" , ret);
                if (ret == -1)
                    printf("ERROR : Incorrect parameters \n");
                if (ret == -2)
                    printf("ERROR : File not present \n");
                if (ret == -3)
                    printf("ERROR : Permission Denied\n");
                continue;
            }
            else if (strcmp(command[0] , "read") == 0 )
            {
                fd = GetFdFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : Incorrrect parameters \n");
                    continue;
                }
                ptr = (char * ) malloc(sizeof (atoi(command[2])) + 1 );
                if (ptr == NULL)
                {
                    printf("ERROR : Memory allocation failure\n");
                    continue;
                }
                ret = ReadFile(fd , ptr , atoi(command[2]));
                if (ret == -1)
                    printf("ERROR : File not exists\n");
                if (ret == -2)
                    printf("ERROR : Permission Denied \n");
                if (ret == -3)
                    printf("ERROR : Reached at end of file \n ");
                if (ret == -4)
                    printf("ERROR : It is not a regutal file \n");
                if (ret > 0)
                {
                    write(2 , ptr , ret);
                }
                continue;

            
            }
            else 
            {
                printf("ERROR : Command not found !!\n");
                continue;
            }
        }
        else if (count == 4)
        {
            if (strcmp(command[0] , "lseek") == 0 )
            {
                fd = GetFdFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : Incorrect Parameters\n");
                    continue;
                }
                ret = LseekFile(fd , atoi(command[2]) , atoi(command[3]));
                if (ret == -1)
                {
                    printf("ERROR : Unable to perform lseek\n");
                }
            }
            else 
            {
                printf("\nERROR : Command not found !!!\n");
                continue;
            }
        }
        else 
        {
            printf("\nERROR : Command Not Found !!!\n");
            continue;
        }
}
    return 0;
}