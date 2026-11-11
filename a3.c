#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main()
{
    char path[256];
    char ch;

    printf("Enter file path: ");

    if (fgets(path, sizeof(path), stdin) == NULL)
        return 1;

    path[strcspn(path, "\n")] = '\0';

 
    int fd = open(path, O_RDONLY);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }

 
    off_t size = lseek(fd, 0, SEEK_END);

    if (size == -1)
    {
        perror("lseek");
        close(fd);
        return 1;
    }

 
    for (off_t i = size - 1; i >= 0; i--)
    {
        if (lseek(fd, i, SEEK_SET) == -1)
        {
            perror("lseek");
            close(fd);
            return 1;
        }

        ssize_t n;

        do
        {
            n = read(fd, &ch, 1);
        } while (n == -1 && errno == EINTR);

        if (n == -1)
        {
            perror("read");
            close(fd);
            return 1;
        }

        if (n == 0)
        {
            fprintf(stderr, "Unexpected end of file\n");
            close(fd);
            return 1;
        }

        if (write(STDOUT_FILENO, &ch, 1) == -1)
        {
            perror("write");
            close(fd);
            return 1;
        }
    }

 
    if (write(STDOUT_FILENO, "\n", 1) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }

    close(fd);

    return 0;
}
