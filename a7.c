#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 4096


ssize_t read_chunk(int fd, char *buffer, int *eof)
{
    size_t total = 0;

    *eof = 0;

    while (total < BUFFER_SIZE)
    {
        ssize_t n = read(fd, buffer + total,
                         BUFFER_SIZE - total);

        if (n == -1)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        if (n == 0)
        {
            *eof = 1;
            break;
        }

        total += n;
    }

    return total;
}

int main()
{
    char path1[256];
    char path2[256];

    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];

     printf("Enter first file path: ");

    if (fgets(path1, sizeof(path1), stdin) == NULL)
        return 1;

    path1[strcspn(path1, "\n")] = '\0';

 
    printf("Enter second file path: ");

    if (fgets(path2, sizeof(path2), stdin) == NULL)
        return 1;

    path2[strcspn(path2, "\n")] = '\0';

 
    int fd1 = open(path1, O_RDONLY);

    if (fd1 == -1)
    {
        perror("Error opening first file");
        return 1;
    }

    int fd2 = open(path2, O_RDONLY);

    if (fd2 == -1)
    {
        perror("Error opening second file");
        close(fd1);
        return 1;
    }

    long byte_index = 0;

    while (1)
    {
        int eof1, eof2;

 
        ssize_t n1 = read_chunk(fd1, buffer1, &eof1);
        ssize_t n2 = read_chunk(fd2, buffer2, &eof2);

        if (n1 == -1 || n2 == -1)
        {
            perror("read");
            close(fd1);
            close(fd2);
            return 1;
        }

 
        ssize_t min_length = (n1 < n2) ? n1 : n2;

        for (ssize_t i = 0; i < min_length; i++)
        {
            if (buffer1[i] != buffer2[i])
            {
                printf("Files differ at byte %ld\n",
                       byte_index + i);

                close(fd1);
                close(fd2);

                return 1;
            }
        }

 
        if (n1 != n2)
        {
            printf("Files differ at byte %ld\n",
                   byte_index + min_length);

            close(fd1);
            close(fd2);

            return 1;
        }

 
        if (eof1 && eof2)
        {
            printf("Files are identical\n");

            break;
        }

 
        byte_index += min_length;
    }

    close(fd1);
    close(fd2);

    return 0;
}
