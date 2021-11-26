#include <ftw.h>
#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>

int list(const char *name, const struct stat *status, int type);
int list_1(const char *name, const struct stat *status, int type);

int main(int argc, char *argv[])
{
    if (argc == 1)
        ftw(".", list, 1);
    else
        ftw(argv[1], list, 1);
    return 0;
}

int list_1(const char *name, const struct stat *status, int type)
{

    // if file is of unknown type, return
    if (type == FTW_NS)
        return 0;

    // if file is hidden or originating in a hidden folder, return
    if (name[2] == '.')
        return 0;

    if (type == FTW_F || type == FTW_D)
    {
        // declare necessary variables
        uid_t user_id = status->st_uid;
        gid_t gid = status->st_gid;
        off_t size = status->st_size;
        size_t path_len = strlen(name);
        char permissions[10];

        // get user name
        struct passwd *pws = getpwuid(user_id);
        char *user_name = pws->pw_name;

        // get group name
        struct group *grp = getgrgid(gid);
        char *group_name = grp->gr_name;

        permissions[0] = type == FTW_F ? '-' : 'd';
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
        int i = path_len-1;
        while(name[i] != '/' && i >= 0)
        {
            name_len++;
            i--;
        }

        // get file nae
        char file_name[name_len];
        i = path_len-1;
        int diff = path_len-name_len;
        strcpy(file_name, name + diff);

        printf("[%s %s %s\t\t%lld]  %s\n", permissions, user_name, group_name, size, file_name);

    }
    return 0;
}