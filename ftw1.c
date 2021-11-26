#include <ftw.h>
#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>

int list(const char *name, const struct stat *status, int type);

int main(int argc, char *argv[]) {
   if(argc == 1)
      ftw(".", list, 1);
   else
      ftw(argv[1], list, 1);
   return 0;
}

int list(const char *name, const struct stat *status, int type) {

   // if file is of unknown type, return 
   if(type == FTW_NS)
      return 0;
   
   // if file is hidden or originating in a hidden folder, return 
   if(name[2] == '.')
      return 0;


   if(type == FTW_F){
      
      // declare necessary variables
      uid_t user_id = status->st_uid;
      gid_t gid = status->st_gid;
      off_t size = status->st_size;
      size_t path_len = strlen(name);
      char permissions[11];
      char file_name[path_len];

      // get user name
      struct passwd *pws = getpwuid(user_id);
      char *user_name = pws->pw_name;

      // get group name
      struct group *grp = getgrgid(gid);
      char *group_name = grp->gr_name;

      // get file name
      int name_start_index = 0;
      for(size_t i = path_len-1; name[i] != '/' ; --i){
         name_start_index++;
      }
      name_start_index = path_len - name_start_index;
      strncpy(file_name, name+name_start_index, path_len-1);

      
      // get permissions
      permissions[0] = '-';
      permissions[1] = status->st_mode & S_IRUSR ? 'r' : '-';
      permissions[2] = status->st_mode & S_IWUSR ? 'w' : '-';
      permissions[3] = status->st_mode & S_IXUSR ? 'x' : '-';
      permissions[4] = status->st_mode & S_IRGRP ? 'r' : '-';
      permissions[5] = status->st_mode & S_IWGRP ? 'w' : '-';
      permissions[6] = status->st_mode & S_IXGRP ? 'x' : '-';
      permissions[7] = status->st_mode & S_IROTH ? 'r' : '-';
      permissions[8] = status->st_mode & S_IWOTH ? 'w' : '-';
      permissions[9] = status->st_mode & S_IXOTH ? 'x' : '-';


      printf("[%s %s %s\t\t%lld]  %s\n", permissions, user_name, group_name, size, file_name);
   }

   else if(type == FTW_D){
      // declare necessary variables
      uid_t user_id = status->st_uid;
      gid_t gid = status->st_gid;
      off_t size = status->st_size;
      size_t path_len = strlen(name);
      char permissions[11];
      char file_name[path_len];

      // get user name
      struct passwd *pws = getpwuid(user_id);
      char *user_name = pws->pw_name;

      // get group name
      struct group *grp = getgrgid(gid);
      char *group_name = grp->gr_name;

      // get file name
      int name_start_index = 1;
      for (size_t i = path_len - 2; name[i] != '/'; --i)
      {
         name_start_index++;
      }
      name_start_index = path_len - name_start_index;
      strncpy(file_name, name + name_start_index, path_len - 2);

      // get permissions
      permissions[0] = 'd';
      permissions[1] = status->st_mode & S_IRUSR ? 'r' : '-';
      permissions[2] = status->st_mode & S_IWUSR ? 'w' : '-';
      permissions[3] = status->st_mode & S_IXUSR ? 'x' : '-';
      permissions[4] = status->st_mode & S_IRGRP ? 'r' : '-';
      permissions[5] = status->st_mode & S_IWGRP ? 'w' : '-';
      permissions[6] = status->st_mode & S_IXGRP ? 'x' : '-';
      permissions[7] = status->st_mode & S_IROTH ? 'r' : '-';
      permissions[8] = status->st_mode & S_IWOTH ? 'w' : '-';
      permissions[9] = status->st_mode & S_IXOTH ? 'x' : '-';

      printf("[%s %s %s\t\t%lld]  %s\n", permissions, user_name, group_name, size, file_name);
   }
      

   return 0;
}

// FTW_F    The object is a  file
// FTW_D    ,,    ,,   ,, ,, directory
// FTW_DNR  ,,    ,,   ,, ,, directory that could not be read
// FTW_SL   ,,    ,,   ,, ,, symbolic link
// FTW_NS   The object is NOT a symbolic link and is one for 
//          which stat() could not be executed


/*
   condition ? if true : else
*/