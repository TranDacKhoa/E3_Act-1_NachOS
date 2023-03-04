#include "openfile.h"
#include "sysdep.h"
class file{
    public:
        OpenFile *openFile=NULL;
        char name[100];
        int fileOpenMode;
        file(char name[100],int mode){
            strcpy(this->name,name);    
            this->fileOpenMode=mode;
        }
};