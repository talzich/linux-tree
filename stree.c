
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"

#define _XOPEN_SOURCE 600 /* Get nftw() */

#include <stdlib.h>
#include <ftw.h>
#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>

/* Total */
int total = 0;
int directorys = 0;
int files = 0;

int check = 0;
int previous_level = 0;

/* ├ ─ └ │ */  // Spcial characters for tree like print

#define WHITE    printf("\033[0;37m");
#define BLUE     printf("\033[1;34m");  
#define GREEN    printf("\033[1;32m");
#define CYAN     printf("\033[1;36m");
#define YELLOW   printf("\033[1;33m");

#define RESET WHITE

int flags = 0 /*| FTW_CHDIR | FTW_DEPTH | FTW_PHYS */;

char file_type_unmask(mode_t bitmasks)
{  
    switch (bitmasks & S_IFMT) /* These bits determine file type.  */
    {
    case S_IFDIR: /* Directory.  */
        return 'd';
    case S_IFCHR: /* Character device.  */
        return 'c';
    case S_IFBLK: /* Block device.  */
        return 'b';
    case S_IFREG: /* Regular file.  */
        return '-';
    case S_IFLNK: /* Symbolic link.  */
        return 'l';
    case S_IFSOCK: /* Socket.  */
        return 's';
    default:
        return '?';
    }
    return '-';
}

int count_all(const char *name, const struct stat *status, int type, struct FTW *ftwb)
{  
    // if file is of unknown type, return
    if (type == FTW_NS) return 0;
    // if file is hidden or originating in a hidden folder, return
    if (name[2] == '.') return 0;
    
    // get level of indentation
    if (ftwb->level == 0){ return 0; }

    total++;    
    return 0;
}

int list(const char *name, const struct stat *status, int type, struct FTW *ftwb)
{
    
    // if file is of unknown type, return
    if (type == FTW_NS) return 0;

    // if file is hidden or originating in a hidden folder, return
    if (name[2] == '.') return 0;

        lstat(name,status);

        // declare necessary variables
        uid_t user_id = status->st_uid;
        gid_t gid = status->st_gid;
        off_t size = status->st_size;
        size_t path_len = strlen(name);
        char permissions[10];
        // get user name
        struct passwd *pws = getpwuid(user_id);
        char *user_name = pws->pw_name;
        // get group nameclear
        struct group *grp = getgrgid(gid);
        char *group_name = grp->gr_name;

        permissions[0] = file_type_unmask(status->st_mode);
        permissions[1] = status->st_mode & S_IRUSR ? 'r' : '-';
        permissions[2] = status->st_mode & S_IWUSR ? 'w' : '-';
        permissions[3] = status->st_mode & S_IXUSR ? 'x' : '-';
        permissions[4] = status->st_mode & S_IRGRP ? 'r' : '-';
        permissions[5] = status->st_mode & S_IWGRP ? 'w' : '-';
        permissions[6] = status->st_mode & S_IXGRP ? 'x' : '-';
        permissions[7] = status->st_mode & S_IROTH ? 'r' : '-';
        permissions[8] = status->st_mode & S_IWOTH ? 'w' : '-';
        permissions[9] = status->st_mode & S_IXOTH ? 'x' : '-';

        // get file_name size
        int name_len = 0;
        int i = path_len - 1;
        while (name[i] != '/' && i >= 0){ name_len++; i--; }

        // get file name
        char file_name[name_len];
        i = path_len - 1;
        int diff = path_len - name_len;
        strcpy(file_name, name + diff);


        if (ftwb->level != 0 ){
            YELLOW
            check++;
            if (check == 1 && total > 1){ printf("├── ");}
            else if (total == 1){ printf("└── ");}
            else{
                
                for (int i = 1; i < ftwb->level; i++){ printf("│   ");}

                if (previous_level == 0 && total  > (directorys + files))
                    { printf("├── ");}
                else if (ftwb->level  <  previous_level )
                    { printf("└── ");}
                else if ((directorys + files) == total - 1)
                    { printf("└── ");}
                else
                    { printf("├── ");}
            }
            RESET
        }
    
    /*  // Used for debugging 

        printf("\033[0;32m%d, ",total);
        printf("\033[0;35mf=%d, ", ftwb->level);
        printf("p=%d\033[0;37m", previous_level);
    */

        // get level of indentation
        int level = previous_level = ftwb->level;
        if (level != 0)
        {
            if (permissions[0] == 'd'){ directorys++; }
            else { files++;}
        }            
        
        if (level == 0)
        { 
            BLUE
            printf("%s\n", file_name); 
            RESET
            return 0; 
        }
        printf("[%s %s %s\t\t%ld]  ", permissions, user_name, group_name, size);

        // pretty print [different color for some types] 
        if (permissions[9] == 'x')
        {
            switch (permissions[0])
            {
            case 'd':
                BLUE
                printf("%s\n",file_name);
                RESET
                break;
            case 'l':
                CYAN
                printf("%s\n",file_name);
                RESET
                break;
            default:
                GREEN
                printf("%s\n",file_name);
                RESET
                break;
            }
        }else{
            WHITE
            printf("%s\n",file_name);
            RESET
        }
        
    return 0;
}


int main(int argc, char *argv[])
{   

    char* dir;
    if (argc == 1)
        {dir = ".";}
    else if (argc == 2)
        {dir = argv[1];}
    else
        { printf("worng usage: %s <directory> OR %s \n", argv[0], argv[0]);}

    nftw(dir, count_all, 10, flags);
    nftw(dir, list, 10, flags);

    char* args0;
    char* args1;

    if (directorys == 1){ args0 = "directory";}
    else { args0 = "directories";}

    if (files == 1) {args1 = "file";}
    else {args1 = "files";}

    printf("\n\n%d %s, %d %s\n", directorys, args0, files, args1);
    
    return 0;
}


