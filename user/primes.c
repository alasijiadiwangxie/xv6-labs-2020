#include "kernel/types.h"
#include "user/user.h"

/**
 * @brief 读取左邻居的第一个数据
 * @param lpipe 左邻居的管道符
 */
int lpipe_first_data(int lpipe[2], int *dst) {
    if (read(lpipe[0], dst, sizeof(int)) == sizeof(int)) {
        printf("prime %d\n", *dst); //是*dst
        return 0;
    }
    return -1;
}

void transmit_data(int lpipe[2], int rpipe[2], int first) {
    int data;
    while (read(lpipe[0], &data, sizeof(int)) == sizeof(int)) {
        if (data % first) {
            write(rpipe[1], &data, sizeof(int));
        }
    }
    close(lpipe[0]);
    close(rpipe[1]);
}

void primes(int lpipe[2])
{
    close(lpipe[1]);
    int first;
    if (lpipe_first_data(lpipe, &first) == 0) {
        int p[2];
        pipe(p);
        transmit_data(lpipe, p, first);//传到右邻居

        if (fork() == 0) {
            primes(p);
        } else {
            close(p[0]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char const *argv[]) {
    int p[2];
    pipe(p);

    for (int i = 2; i <= 35; i++)
        write(p[1], &i, sizeof(int));
    
    if (fork() == 0) {
        primes(p);
    }
    else {
        close(p[0]);
        close(p[1]); // 可以在写完35个数据就关闭，也可以在这里关闭
        wait(0);
    }
    exit(0);
}