#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
 
int http_process() 
{
    pid_t pid = fork(); // 创建子进程
 
    if (pid == 0) {
        // 子进程代码
        printf("This is the http process, PID = %d\n", getpid());
        // 尝试切换当前工作目录
        if (chdir("./httpserver/") != 0) {
            perror("chdir failed");
            return -1;
        }

        execl("./x86_64-pc-linux-gnu-httpserver", "x86_64-pc-linux-gnu-httpserver", NULL);
        // 如果execl成功返回，则不会执行到这里
        perror("execl failed");
    } else if (pid > 0) {
        // 父进程代码
        printf("This is the parent process, PID = %d\n", getpid());
        //wait(NULL); // 等待子进程结束
    } else {
        // fork失败的情况处理
        perror("fork failed");
        return 1;
    }
 
    return 0;
}
