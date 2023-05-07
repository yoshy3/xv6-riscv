#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../kernel/fcntl.h"
#include "../kernel/fs.h"
#include "user.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: touch [files]...\n");
        exit(1);
    }
    int i, err;
    for (i = 1; i < argc; i++)
    {
        if ((err = open(argv[i], O_CREATE | O_RDWR)) < 0)
        {
            printf("touch: error where creating %s\n", argv[i]);
            exit(1);
        }
        close(err);
    }
    exit(0);
}