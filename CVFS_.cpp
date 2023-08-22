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

void Display()
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



int main(int argc ,  char * argv[])
{
    // man(argv[1]);
    // Display();
    // CreateDILB();

    return 0;
}