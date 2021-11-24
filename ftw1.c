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
      char *permissions = "----------";
      char file_name[path_len];

      // get user name
      struct passwd *pws = getpwuid(user_id);
      char *user_name = pws->pw_name;

      // get group name
      struct group *grp = getgrgid(gid);
      char *group_name = grp->gr_name;

      //getting file name
      int name_start_index = 0;
      for(size_t i = path_len-1; name[i] != '/' ; --i){
         name_start_index++;
      }
      name_start_index = path_len - name_start_index;
      strncpy(file_name, name+name_start_index, path_len-1);
      printf("0%3o\t%s\t%s\t%s\n", status->st_mode & 0777, file_name, user_name, group_name);
   }
      
      
 
   else if(type == FTW_D && strcmp(".", name) != 0){
      uid_t user_id = status->st_uid;
      gid_t gid = status->st_gid;
      off_t size = status->st_size;
      char *permissions = "d---------";

      printf("0%3o\t%s/\n", status->st_mode & 0777, name);
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