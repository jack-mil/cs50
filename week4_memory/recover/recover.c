#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// #include <stdint.h>

typedef unsigned char BYTE;
BYTE buffer[512];

const int BLOCKSIZE = 512;

bool foundimage = false;

int main(int argc, char *argv[])
{
    int count = 0;
    char out[10];
    // Check for exactly 2 arguments
    if (argc != 2)
    {
        printf("Usage: ./recover image");
        return 1;
    }

    FILE *infile = fopen(argv[1], "rb");
    if (infile == NULL)
    {
        printf("Could not open %s. Are you sure the file exists? \n", argv[1]);
        return 1;
    }

    // Placeholder output stream
    FILE *image = NULL;
    int i = 0;

    // Until we fail to read 512B
    while (fread(buffer, sizeof(buffer), 1, infile) == 1)
    {
        // Read one 512B block into buffer
        // fread(buffer, sizeof(BYTE), BLOCKSIZE, infile);

        // Check if buffer contains beginnning of new JPEG image
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && buffer[3] >> 4 == 0xe)
        {

            // Close old image
            if (image != NULL)
            {
                fclose(image);
            }

            // Trigger that we have found an image
            foundimage = true;

            // Create new filename and open a new image
            sprintf(out, "./new/%03i.jpg", count);
            image = fopen(out, "wb");
            if (image == NULL) // Check for image error
            {
                fclose(infile);
                printf("Could not create %s.\n", out);
                return 1;
            }

            // Increment image count
            count++;
        }

        if (foundimage)
        {
            fwrite(buffer, sizeof(buffer), 1, image);
        }

        i++;
    }

    fclose(image);
    fclose(infile);
    printf("EOF reached! %i blocks read; %i images recovered\n", i, count);
    return 0;

}
