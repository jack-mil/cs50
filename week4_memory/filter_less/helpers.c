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
            // Average the Red, Green, and Blue color values into one.
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

            int sepiaRed =
                round(.393 * image[i][j].rgbtRed + .769 * image[i][j].rgbtGreen +
                      .189 * image[i][j].rgbtBlue);
            if (sepiaRed > 255)
            {
                sepiaRed = 255;
            }
            int sepiaGreen =
                round(.349 * image[i][j].rgbtRed + .686 * image[i][j].rgbtGreen +
                      .168 * image[i][j].rgbtBlue);
            if (sepiaGreen > 255)
            {
                sepiaGreen = 255;
            }
            int sepiaBlue =
                round(.272 * image[i][j].rgbtRed + .534 * image[i][j].rgbtGreen +
                      .131 * image[i][j].rgbtBlue);
            if (sepiaBlue > 255)
            {
                sepiaBlue = 255;
            }

            image[i][j].rgbtRed = (BYTE)sepiaRed;
            image[i][j].rgbtGreen = (BYTE)sepiaGreen;
            image[i][j].rgbtBlue = (BYTE)sepiaBlue;
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
            swap(&image[i][j], &image[i][width - 1 - j]);
        }
    }
    return;
}

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
    RGBTRIPLE(*orgimg)
    [width] = calloc(height, width * sizeof(RGBTRIPLE));

    memcpy(orgimg, image, width * height * sizeof(RGBTRIPLE));

    int sumred, sumgreen, sumblue, count;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            count = sumred = sumgreen = sumblue = 0;

            for (int h = -1; h < 2; h++)
            {
                for (int k = -1; k < 2; k++)
                {
                    if (i + h > height - 1 || i + h < 0 || j + k > width - 1 || j + k < 0)
                    {
                        continue;
                    }
                    sumred += orgimg[i + h][j + k].rgbtRed;
                    sumgreen += orgimg[i + h][j + k].rgbtGreen;
                    sumblue += orgimg[i + h][j + k].rgbtBlue;
                    count++;
                }
            }

            RGBTRIPLE blurred = {(BYTE)round((float)sumblue / count),
                                (BYTE)round((float)sumgreen / count),
                                (BYTE)round((float)sumred / count)
                                };
            image[i][j] = blurred;
        }
    }

    free(orgimg);
    return;
}
