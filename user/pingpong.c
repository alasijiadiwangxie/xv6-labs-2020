#include "kernel/types.h"
#include "user/user.h"

#define RD 0 //管道的读端
#define WR 1 //管道的写端

int main(int argc, char const *argv[]) {
    int pipe_p2c[2], pipe_c2p[2];
    char buf[1];

    if ((pipe(pipe_p2c) < 0) || (pipe(pipe_c2p) < 0))
    {
        fprintf(2, "pingpong: pipe creat failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "pingpong: fork failed\n");
        exit(1);
    }
    if (pid == 0) { // 子进程
        // 子进程不需要“父→子管道”的写端、“子→父管道”的读端，关闭避免资源泄漏
        close(pipe_p2c[WR]);
        close(pipe_c2p[RD]);

        // 从“父→子管道”读1字节
        if (read(pipe_p2c[RD], buf, 1) != 1) {
            fprintf(2, "pingpong: child read error\n");
            exit(1);
        }

        printf("%d: received ping\n", getpid());

        // 向“子→父管道”写1字节
        if (write(pipe_c2p[WR], buf, 1) != 1) {
            fprintf(2, "pingpong: child write error\n");
            exit(1);
        }

        close(pipe_p2c[RD]);
        close(pipe_c2p[WR]);
        exit(0);
    }
    else
    {
        // 父进程不需要“父→子管道”的读端、“子→父管道”的写端
        close(pipe_p2c[RD]);
        close(pipe_c2p[WR]);

        // 先向“父→子管道”写1字节
        if (write(pipe_p2c[WR], buf, 1) != 1)
        {
            fprintf(2, "pingpong: parent write error\n");
            exit(1);
        }

        // 从“子→父管道”读1字节
        if (read(pipe_c2p[RD], buf, 1) != 1)
        {
            fprintf(2, "pingpong: parent read error\n");
            exit(1);
        }

        printf("%d: received pong\n", getpid());

        close(pipe_p2c[WR]);
        close(pipe_c2p[RD]);
        exit(0);
    }
}