#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#define BUFFER_SIZE 4096

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
 
    int fd = open("log.txt",
                  O_WRONLY | O_CREAT | O_APPEND,
                  0644);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    char line[BUFFER_SIZE];
    char result[BUFFER_SIZE + 100];

    printf("Enter a log message: ");
    fflush(stdout);

 
    size_t length = 0;
    char ch;

    while (length < BUFFER_SIZE - 1)
    {
        ssize_t n = read(STDIN_FILENO, &ch, 1);

        if (n == -1)
        {
            if (errno == EINTR)
                continue;

            perror("read");
            close(fd);
            return 1;
        }

        if (n == 0 || ch == '\n')
            break;

        line[length++] = ch;
    }

    line[length] = '\0';

 
    int prefix_length = snprintf(result,
                                 sizeof(result),
                                 "PID=%ld: ",
                                 (long)getpid());

    if (prefix_length < 0 ||
        (size_t)prefix_length >= sizeof(result))
    {
        fprintf(stderr, "Formatting error\n");
        close(fd);
        return 1;
    }

    memcpy(result + prefix_length, line, length);

    size_t total_length = prefix_length + length;

 
    if (total_length == 0 ||
        result[total_length - 1] != '\n')
    {
        result[total_length++] = '\n';
    }

 
    if (write_all(fd, result, total_length) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }

 
    off_t position = lseek(fd, 0, SEEK_CUR);

    if (position == -1)
    {
        perror("lseek");
        close(fd);
        return 1;
    }

    printf("Final file offset: %ld\n", (long)position);

    close(fd);

    return 0;
}
