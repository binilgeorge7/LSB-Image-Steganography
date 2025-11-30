/*
Name        : Binil George  
Date        : 17-11-2025
Project     : LSB Image Steganography (Encoding & Decoding)

Description : This project implements LSB (Least Significant Bit) based Steganography
              to securely hide and retrieve confidential data inside a BMP image.

              Features:
              1. Encodes any text/script file (.txt / .c / .sh) inside a BMP image.
              2. Uses LSB bit-level encoding without affecting the visible image quality.
              3. Supports decoding to extract the hidden secret data from the stego image.
              4. Ensures data integrity by using a Magic String based validation.
              5. Capacity check is performed before encoding to avoid data overflow.
              6. Includes informative logs, argument validation and error handling.

              Components:
              ▪ Encoding  — Hides secret data into the image.
              ▪ Decoding  — Recovers the hidden data back from the image.
              ▪ Custom CLI interface supporting:
                    -e  for encoding operation
                    -d  for decoding operation

              Output:
              Generates a new BMP file (stego image) with encoded data during encoding
              and restores the original secret file during decoding.
*/

#include <stdio.h>
#include "decode.h"
#include "types.h"
#include<string.h>
#include"common.h"

//function definition for argument validation
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo, int argc)
{
    //to check if argv[2] is a bmp file
    char* ptr1 = strstr(argv[2], ".bmp");
    if(ptr1 == NULL || strcmp(ptr1, ".bmp"))
    {
        printf("%s is not a bmp file\n", argv[2]);
        return e_failure;
    }
    else
    {
        decInfo->stego_image_fname = argv[2];
    }

    //to check if secret file name is passed or not
    if(argv[3] != NULL)
    {
        strcpy(decInfo->output_secret_fname, argv[3]);
    }
    else
    {
        printf("INFO: Output File not mentioned creating secret_op as default file name\n");
        strcpy(decInfo->output_secret_fname, "secret_op");
    }
    return e_success;
}

//decoding function that calls all sub functions
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started ##\n");
    if(open_img_file(decInfo) == e_success)
    { 
        if(skip_bmp_header(decInfo) == e_success)
        {
            if(decode_magic_string(decInfo) == e_success)
            {
                if(decode_secret_file_extn_size(decInfo) == e_success)
                {
                    if(decode_secret_file_extn(decInfo) == e_success)
                    {
                        if(decode_secret_file_size(decInfo) == e_success)
                        {
                            if(decode_secret_file_data(decInfo) == e_success)
                            {
                                printf("INFO: ## Decoding Done Successfully ##\n");
                                return e_success;
                            }
                        }
                    }
                }
            }
        }
    }
    return e_failure;
}

Status open_img_file(DecodeInfo *decInfo)
{
    printf("INFO: Opening Required Files\n");
    // stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }
    printf("INFO: Opened %s\n", decInfo->stego_image_fname);

    // No failure return e_success
    return e_success;
}

Status skip_bmp_header(DecodeInfo *decInfo)
{
    //skips 54 bytes
    if(fseek(decInfo->fptr_stego_image, 54, SEEK_SET) == 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    printf("INFO: Decoding Magic String Signature\n");
    char magic_string[3];
    char buffer[8];
    for(int i = 0; i < 2; i++)
    {
        if(fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)   //reads 8 bytes of data and store in buffer and check if 8 bytes is read properly.
        {
            printf("Error while reading\n");   //prints error message
            return e_failure;
        }
        magic_string[i] = decode_byte_from_lsb(buffer);
    }
    magic_string[2] = '\0';
    if(!strcmp(magic_string, MAGIC_STRING))
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }

}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    printf("INFO: Decoding Output File Extension Size\n");
    char buffer[32];
    if(fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)   //reads 32 bytes of data and store in buffer and check if 32 bytes is read properly.
    {
        printf("Error while reading\n");   //prints error message
        return e_failure;
    }
    decInfo->secret_extn_size = decode_int_from_lsb(buffer);  //stores file extension size in buffer
    printf("INFO: Done\n");
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    printf("INFO: Decoding Output File Extension\n");
    char extn[(decInfo->secret_extn_size) + 1];   //char array for storing secret file extension
    char buffer[8];
    for(int i = 0; i < decInfo->secret_extn_size; i++)
    {
        if(fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)   //reads 8 bytes of data and store in buffer and check if 8 bytes is read properly.
        {
            printf("Error while reading\n");   //prints error message
            return e_failure;
        }
        extn[i] = decode_byte_from_lsb(buffer);   //stores extension character by character after decoding
    }
    extn[decInfo->secret_extn_size] = '\0';   //adds null character at the end
    strcat(decInfo->output_secret_fname, extn);   //joins extension with secret file name
    printf("INFO: Done\n");

    decInfo->fptr_output_secret = fopen(decInfo->output_secret_fname, "w");   //opens secret fileto store datra in write mode
    printf("INFO: Opened %s\n", decInfo->output_secret_fname);
    printf("INFO: Done, Opened all require Files\n");
    return e_success;
    
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
   

    printf("INFO: Decoding File Size\n");
    char buffer[32];
    if(fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)   //reads 32 bytes of data and store in buffer and check if 32 bytes is read properly.
    {
        printf("Error while reading\n");   //prints error message
        return e_failure;
    }
    decInfo->secret_file_size = decode_int_from_lsb(buffer);  // stores decoded secret file size to buffer
    printf("INFO: Done\n");
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{

    printf("INFO: Decoding File Data\n");
    char buffer[8];
    for(int i = 0; i < decInfo->secret_file_size; i++)
    {
        if(fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)   //reads 8 bytes of data and store in buffer and check if 8 bytes is read properly.
        {
            printf("Error while reading\n");   //prints error message
            return e_failure;
        }
        fputc(decode_byte_from_lsb(buffer), decInfo->fptr_output_secret);  //stres char by char in output file
    }
    fclose(decInfo->fptr_output_secret);
    fclose(decInfo->fptr_stego_image);
    printf("INFO: Done\n");
    return e_success;

}

//to decode byte
char decode_byte_from_lsb(char* buffer)
{
    char ch = 0;
    for(int i = 0; i < 8; i++)
    {
        ch = ch | ((buffer[i] & 1) << (7-i));
    }
    return ch;
}

//to decode int
int decode_int_from_lsb(char* buffer)
{
    int num = 0;
    for(int i = 0; i < 32; i++)
    {
        num = num | ((buffer[i] & 1) << (31-i));
    }
    return num;
}