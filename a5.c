#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int write_all(int fd, const char *buffer, size_t size)
{
    size_t total = 0;

    while (total < size)
    {
        ssize_t n = write(fd, buffer + total, size - total);

        if (n == -1)
            return -1;

        if (n == 0)
            return -1;

        total += n;
    }

    return 0;
}

int main()
{

    int fd = open("sparse.bin",
                  O_WRONLY | O_CREAT | O_TRUNC,
                  0644);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }


    if (write_all(fd, "START", 5) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }


    off_t position = lseek(fd, 1024 * 1024, SEEK_CUR);

    if (position == -1)
    {
        perror("lseek");
        close(fd);
        return 1;
    }

    if (write_all(fd, "END", 3) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }

    close(fd);

   
    fd = open("sparse.bin", O_RDONLY);

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

    printf("Apparent file size: %ld bytes\n", (long)size);

   
    close(fd);

    return 0;
}
