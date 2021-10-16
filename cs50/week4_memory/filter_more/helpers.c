#include "helpers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(RGBTRIPLE *a, RGBTRIPLE *b);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    BYTE avg;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Average the Red, Green, and Blue color values into one value
            avg = round((image[i][j].rgbtRed + image[i][j].rgbtBlue + image[i][j].rgbtGreen) / 3.0);
            image[i][j] = (RGBTRIPLE){avg, avg, avg};
        }
    }
    return;
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Calculate sepia values based on specific equations for R, G, B

            int sepiaRed =
                round(.393 * image[i][j].rgbtRed + .769 * image[i][j].rgbtGreen +
                      .189 * image[i][j].rgbtBlue);
            sepiaRed = (sepiaRed > 255) ? 255 : (BYTE)sepiaRed;

            int sepiaGreen =
                round(.349 * image[i][j].rgbtRed + .686 * image[i][j].rgbtGreen +
                      .168 * image[i][j].rgbtBlue);
            sepiaGreen = (sepiaGreen > 255) ? 255 : (BYTE)sepiaGreen;

            if (sepiaGreen > 255)
                sepiaGreen = 255;

            int sepiaBlue =
                round(.272 * image[i][j].rgbtRed + .534 * image[i][j].rgbtGreen +
                      .131 * image[i][j].rgbtBlue);
            sepiaBlue = (sepiaBlue > 255) ? 255 : (BYTE)sepiaBlue;

            image[i][j] = (RGBTRIPLE){sepiaBlue, sepiaGreen, sepiaRed};
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            // Reflect the pixle
            swap(&image[i][j], &image[i][width - 1 - j]);
        }
    }
    return;
}

// Swap two pixels
void swap(RGBTRIPLE *a, RGBTRIPLE *b)
{
    RGBTRIPLE tmp = *a;
    *a = *b;
    *b = tmp;
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Allocate memory for a copy of the image array
    RGBTRIPLE(*orgimg)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    // Copy the values to the new memory locations
    memcpy(orgimg, image, width * height * sizeof(RGBTRIPLE));

    int sumred, sumgreen, sumblue, count;

    // Loop through entire image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            count = sumred = sumgreen = sumblue = 0;

            // use vertical and horizontal offset to index 
            // a 3x3 area around each pixel
            for (int vo = -1; vo < 2; vo++)
            {
                for (int ho = -1; ho < 2; ho++)
                {
                    // If we are on a pixel out of range, skip to the next pixel
                    if (i + vo > height - 1 || i + vo < 0 || j + ho > width - 1 || j + ho < 0)
                        continue;

                    // Add up the total of each color in the 3x3 grid
                    sumred += orgimg[i + vo][j + ho].rgbtRed;
                    sumgreen += orgimg[i + vo][j + ho].rgbtGreen;
                    sumblue += orgimg[i + vo][j + ho].rgbtBlue;
                    count++;
                }
            }

            RGBTRIPLE blurred = {round((float)sumblue / count),
                                 round((float)sumgreen / count),
                                 round((float)sumred / count)};
            image[i][j] = blurred;
        }
    }

    // Free copied memory
    free(orgimg);
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Define x and y kernels;
    const int Gx[3][3] = {{-1, 0, 1},
                          {-2, 0, 2},
                          {-1, 0, 1}};
    const int Gy[3][3] = {{-1, -2, -1},
                          { 0,  0,  0},
                          { 1,  2,  1}};

    // Allocate memory for a copy of the image array
    RGBTRIPLE(*orgimg)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    // Copy the values to the new memory location
    memcpy(orgimg, image, width * height * sizeof(RGBTRIPLE));

    int gxr, gxg, gxb;
    int gyr, gyg, gyb;
    int gr, gg, gb;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Reset sums
            gxr = gxg = gxb = 0;
            gyr = gyg = gyb = 0;

            // For a 3x3 area around each pixel
            // define vertical and horizontal offset (vo, ho)
            for (int vo = -1; vo < 2; vo++)
            {
                for (int ho = -1; ho < 2; ho++)
                {
                    // Skip pixels on the edge
                    if (i + vo > height - 1 || i + vo < 0 || j + ho > width - 1 || j + ho < 0)
                        continue;

                    // Use Gx kernel to find edges in x-direction
                    gxr += orgimg[i + vo][j + ho].rgbtRed * Gx[vo + 1][ho + 1];
                    gxg += orgimg[i + vo][j + ho].rgbtGreen * Gx[vo + 1][ho + 1];
                    gxb += orgimg[i + vo][j + ho].rgbtBlue * Gx[vo + 1][ho + 1];

                    // Use Gy kernel to find edges in y-direction
                    gyr += orgimg[i + vo][j + ho].rgbtRed * Gy[vo + 1][ho + 1];
                    gyg += orgimg[i + vo][j + ho].rgbtGreen * Gy[vo + 1][ho + 1];
                    gyb += orgimg[i + vo][j + ho].rgbtBlue * Gy[vo + 1][ho + 1];
                }
            }

            // Combine Gx and Gy values
            gr = round(sqrt(pow(gxr, 2) + pow(gyr, 2)));
            gg = round(sqrt(pow(gxg, 2) + pow(gyg, 2)));
            gb = round(sqrt(pow(gxb, 2) + pow(gyb, 2)));

            // Set the pixel to new values. Cap at 255
            RGBTRIPLE p = (RGBTRIPLE){((gb > 255) ? 255 : gb),
                                      ((gg > 255) ? 255 : gg),
                                      ((gr > 255) ? 255 : gr)};
            image[i][j] = p;
        }
    }

    // Free copied memory
    free(orgimg);
    return;
}
