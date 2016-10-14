#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>

#define MAX_BUF 512
#define SUCCESS 1
#define FAIL 0

int dmesg_check(char *buf){
    const char* tok=" ";
    const char* ima="type=1800";
    char *str;
    int count=0;

    if(!buf)
        return 0;

    strtok(buf,tok);
    while(str = strtok(NULL,tok)){
        count++;
        if(count == 3)
            break;
    }
    if(count == 3){
        if(!strncmp(str,ima,9)){
            return 1;
        }
    }

    return 0;
}

int sudo_check(){
    FILE *fp_sudo;
    char buf[MAX_BUF]={0,};
    char *str = NULL;
    const char *key = "\n";
    int count, ret=0;
    
    fp_sudo = popen("pgrep sudo | wc -l", "r");
    if(fp_sudo == NULL)
        goto out;

    while(fgets(buf, MAX_BUF, fp_sudo)){
        fprintf(stdout,"%s",buf);
        if(count = atoi(buf)){
            ret = count;
            break;
        }
        memset(buf,0,MAX_BUF);
    }
  
out:
   if(fp_sudo) 
       pclose(fp_sudo);

    return ret;
}

int dmesg_IO(){
    FILE *fp_dmesg, *fp_log;
    char buf[MAX_BUF]={0,};
    char buf_cpy[MAX_BUF]={0,};
    int ret=FAIL;

    fp_dmesg = popen("dmesg", "r");
    if(fp_dmesg == NULL){
        goto out;
    }

    fp_log = fopen("/usr/ima-appraisal/log.txt", "wt");
    if(fp_log == NULL){
        goto out;
    }

    while(fgets(buf, MAX_BUF, fp_dmesg) != NULL){

        strcpy(buf_cpy,buf);
        if(dmesg_check(buf_cpy)){
            fprintf(fp_log,"%s",buf);
            memset(buf,0,sizeof(buf));
            memset(buf_cpy,0,sizeof(buf_cpy));
        } 
    }
      
    ret = SUCCESS; 
    fflush(fp_log);
out:
    if(fp_dmesg)
        pclose(fp_dmesg);
    if(fp_log)
        fclose(fp_log);

    return ret;
}

int main(){
    int pid, ret=1;
    int size=0;

    if((pid=fork()) <0){
        return 1;
    }else if(pid > 0){
        _exit(0);
    }

    if(setsid() == -1)
        _exit(0);

    if(chdir("/") == -1)
        _exit(0);
  
    umask(0);

    signal(SIGHUP, SIG_IGN);

    close(0);
    close(1);
    close(2);
    stdin = freopen("/dev/null", "r", stdin);
    stdout = freopen("/dev/null", "w", stdout);
    stderr = freopen("/dev/null", "w", stderr);

    while(!sudo_check()){}

    while(1){
        dmesg_IO(); 
        sleep(10);
    }
    
    return 0;
}
