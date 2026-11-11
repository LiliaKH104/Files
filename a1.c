#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

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

        total += n;
    }

    return 0;
}

int main()
{
    char source[256];
    char destination[256];
    char buffer[BUFFER_SIZE];

  
    printf("Enter source file path: ");
    if (fgets(source, sizeof(source), stdin) == NULL)
        return 1;

    source[strcspn(source, "\n")] = '\0';

  
    printf("Enter destination file path: ");
    if (fgets(destination, sizeof(destination), stdin) == NULL)
        return 1;

    destination[strcspn(destination, "\n")] = '\0';

  
    int src = open(source, O_RDONLY);

    if (src == -1)
    {
        perror("Error opening source file");
        return 1;
    }

  
    int dest = open(destination,
                    O_WRONLY | O_CREAT | O_TRUNC,
                    0644);

    if (dest == -1)
    {
        perror("Error opening destination file");
        close(src);
        return 1;
    }

    ssize_t bytes_read;
    long total_bytes = 0;

  
    while ((bytes_read = read(src, buffer, BUFFER_SIZE)) > 0)
    {
        if (write_all(dest, buffer, bytes_read) == -1)
        {
            perror("Error writing to destination");
            close(src);
            close(dest);
            return 1;
        }

        total_bytes += bytes_read;
    }

    if (bytes_read == -1)
    {
        perror("Error reading source file");
        close(src);
        close(dest);
        return 1;
    }

    printf("File copied successfully!\n");
    printf("Total bytes copied: %ld\n", total_bytes);

    close(src);
    close(dest);

    return 0;
}
