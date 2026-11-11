#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int write_all(int fd, const char *buffer, size_t size)
{
    size_t total = 0;

    while (total < size)
    {
        ssize_t n = write(fd, buffer + total, size - total);

        if (n == -1)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        if (n == 0)
            return -1;

        total += n;
    }

    return 0;
}

int main()
{

    int fd = open("numbers.txt",
                  O_WRONLY | O_CREAT | O_TRUNC,
                  0644);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    char initial_content[100];
    int length = 0;

    for (int i = 1; i <= 10; i++)
    {
        length += snprintf(initial_content + length,
                           sizeof(initial_content) - length,
                           "%d\n", i);
    }

    if (write_all(fd, initial_content, length) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }

    close(fd);


    fd = open("numbers.txt", O_RDWR);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }


    char ch;
    int newline_count = 0;
    off_t line4_start = -1;

    while (1)
    {
        ssize_t n = read(fd, &ch, 1);

        if (n == -1)
        {
            if (errno == EINTR)
                continue;

            perror("read");
            close(fd);
            return 1;
        }

        if (n == 0)
            break;

        if (ch == '\n')
        {
            newline_count++;

            if (newline_count == 3)
            {
                line4_start = lseek(fd, 0, SEEK_CUR);

                if (line4_start == -1)
                {
                    perror("lseek");
                    close(fd);
                    return 1;
                }

                break;
            }
        }
    }

    if (line4_start == -1)
    {
        fprintf(stderr, "Could not find line 4\n");
        close(fd);
        return 1;
    }

    printf("Start of line 4: byte %ld\n", (long)line4_start);

    if (lseek(fd, line4_start + 2, SEEK_SET) == -1)
    {
        perror("lseek");
        close(fd);
        return 1;
    }

    char remainder[100];
    size_t remainder_length = 0;

    while (remainder_length < sizeof(remainder))
    {
        ssize_t n = read(fd,
                         remainder + remainder_length,
                         sizeof(remainder) - remainder_length);

        if (n == -1)
        {
            if (errno == EINTR)
                continue;

            perror("read");
            close(fd);
            return 1;
        }

        if (n == 0)
            break;

        remainder_length += n;
    }

 
    if (lseek(fd, line4_start, SEEK_SET) == -1)
    {
        perror("lseek");
        close(fd);
        return 1;
    }

 
    if (write_all(fd, "100\n", 4) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }

 
    if (write_all(fd, remainder, remainder_length) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }

 
    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        close(fd);
        return 1;
    }

    printf("\nFinal file content:\n");

    char buffer[100];
    ssize_t n;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (write_all(STDOUT_FILENO, buffer, n) == -1)
        {
            perror("write");
            close(fd);
            return 1;
        }
    }

    if (n == -1)
    {
        perror("read");
        close(fd);
        return 1;
    }

    close(fd);

    return 0;
}
