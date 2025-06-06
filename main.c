#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFER_SIZE 512  // Adjust based on file size

// Function prototypes
void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t skip, uint64_t offset);
void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset);

int main() {
    const char *vdi = "./good-fixed-1k.vdi";
    uint8_t buffer[BUFFER_SIZE];

    // Step 1: Check if the file exists and open it
    if (access(vdi, F_OK) == -1) {
        perror("File does not exist");
        return 1;
    }

    int fd = open(vdi, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    // Step 2: Read the file into buffer
    ssize_t bytesRead = read(fd, buffer, BUFFER_SIZE);
    if (bytesRead == -1) {
        perror("Error reading file");
        close(fd);
        return 1;
    }

    // Step 3: Display the buffer contents
    displayBuffer(buffer, bytesRead, 0);

    // Step 4: Close the file
    close(fd);

    return 0;
}

// Function Definitions
void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t skip, uint64_t offset) {
    count = (count > 256) ? 256 : count;  // Limit to 256 bytes
    printf("Offset: 0x%lx\n", offset);
    printf("  00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 0...4...8...c...\n");
    printf("  +------------------------------------------------+   +----------------+\n");

    for (uint32_t i = 0; i < 16; i++) {  // 16 lines of 16 bytes
        printf("%02x|", (unsigned int)(offset + i * 16));

        // Hexadecimal display
        for (uint32_t j = 0; j < 16; j++) {
            size_t pos = i * 16 + j;
            if (pos >= skip && pos < skip + count) {
                printf("%02x ", buf[pos]);
            } else {
                printf("   ");
            }
        }

        printf("|%02x|", (unsigned int)(offset + i * 16));

        // Character display
        for (uint32_t j = 0; j < 16; j++) {
            size_t pos = i * 16 + j;
            if (pos >= skip && pos < skip + count) {
                printf("%c", isprint(buf[pos]) ? buf[pos] : '.');
            } else {
                printf(" ");
            }
        }
        printf("|\n");
    }
    printf("  +------------------------------------------------+   +----------------+\n\n");
}

void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset) {
    for (uint32_t i = 0; i < count; i += 256) {
        uint32_t chunk_size = (count - i > 256) ? 256 : count - i;
        displayBufferPage(buf + i, chunk_size, 0, offset + i);
    }
}
