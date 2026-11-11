#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
 
    int fd = open("data.txt",
                  O_WRONLY | O_CREAT | O_TRUNC,
                  0644);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    char text[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (write(fd, text, 26) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }

    close(fd);

 
    fd = open("data.txt", O_RDWR);

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

    printf("Original file size: %ld bytes\n", (long)size);

 
    if (ftruncate(fd, 10) == -1)
    {
        perror("ftruncate");
        close(fd);
        return 1;
    }

 
    size = lseek(fd, 0, SEEK_END);

    if (size == -1)
    {
        perror("lseek");
        close(fd);
        return 1;
    }

    printf("New file size: %ld bytes\n", (long)size);

 
    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        close(fd);
        return 1;
    }

 
    char buffer[11];

    ssize_t n = read(fd, buffer, 10);

    if (n == -1)
    {
        perror("read");
        close(fd);
        return 1;
    }

    buffer[n] = '\0';

    printf("Remaining content: %s\n", buffer);

    close(fd);

    return 0;
}
