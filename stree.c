
#define _XOPEN_SOURCE 600 /* Get nftw() */

#include <stdlib.h>
#include <ftw.h>
#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>

#include <dirent.h>

int check = 0;
int count = 0;
int directorys = 0;
int files = 0;
int previous_level = 0;
// int max_level = 0;
  
#define WHITE printf("\033[0;37m");
#define BLUE  printf("\033[1;34m");  
#define GREEN printf("\033[1;32m");
#define CYAN  printf("\033[1;36m");

#define RESET WHITE

void listFilesRecursively(char *basePath)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("%s\t", dp->d_name);
            printf("%u\n", dp->d_type);

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            listFilesRecursively(path);
        }
    }

    closedir(dir);
}


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

        if (ftwb->level != 0 )
        {
            for (int i = 0; i < (ftwb->level - 1); i++){
                if (i == 0){ printf("│   ");}
                else{ printf("   "); }    
            }
            if (previous_level == 0 && count > (directorys + files)){ printf("├── ");}
            else if (ftwb->level  <  previous_level ){ printf("└── ");}
            // else if ((directorys + files) == count){ printf("└── ");}
            else{ printf("├── ");}
        }

        printf("\033[0;32m%d, ",count);
        printf("\033[0;35mf=%d, ", ftwb->level);
        printf("p=%d\033[0;37m", previous_level);

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
    if (argc == 1){dir = ".";}
    else if (argc == 2){dir = argv[1];}
    else{ printf("worng usage: %s <directory> OR %s \n", argv[0], argv[0]);}

    listFilesRecursively(dir);
    int flags = 0 /*| FTW_CHDIR | FTW_DEPTH | FTW_PHYS */;
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

/*

├ 
─
└
│

*/
