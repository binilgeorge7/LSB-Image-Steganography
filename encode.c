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
#include "encode.h"
#include "types.h"
#include<string.h>
#include"common.h"


/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;   //3 is size of pixel
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);   //prints file opening error

    	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->src_image_fname );

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);  //prints file opening error

    	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->secret_fname );

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");   //opens stego image
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);   //prints file opening error

    	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->stego_image_fname );
    printf("INFO: Done\n");

    // No failure return e_success
    return e_success;
}

//function definition for argument validation
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo, int argc)
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
        encInfo->src_image_fname = argv[2];
    }

    //to check argv[3] extension is .txt or .c or .sh
    char* ptr2 = strstr(argv[3], ".txt");
    char* ptr3 = strstr(argv[3], ".c");
    char* ptr4 = strstr(argv[3], ".sh");
    if(!((ptr2 != NULL && strcmp(ptr2, ".txt") == 0) || (ptr3 != NULL && strcmp(ptr3, ".c") == 0) || (ptr4 != NULL && strcmp(ptr4, ".sh") == 0)))  //checks if ptr is not NULL and ptr text is extension, if so returns 0 because of '!'
    {
        printf("%s is not a .txt/.c/.sh file\n", argv[3]);
        return e_failure;
    }
    else
    {
        encInfo->secret_fname = argv[3];   //to store secter file name to structure

        //to store secret file extension to structure.
        if(ptr2 != NULL)
        {
            strcpy(encInfo->extn_secret_file, ptr2);
        }
        else if(ptr3 != NULL)
        {
            strcpy(encInfo->extn_secret_file, ptr3);
        }
        else if(ptr4 != NULL)
        {
            strcpy(encInfo->extn_secret_file, ptr4);
        }
    }

    //to check argv[4] passed if yes checks if it is bmp file else passes default file name to structure
    if(argv[4] == NULL)
    {
        encInfo->stego_image_fname = "stego.bmp";
        printf("INFO: Output File not mentioned creating stego.bmp as default\n");
        return e_success;
    }
    else
    {
        //to check if argv[4] is a bmp file
        char* ptr5 = strstr(argv[4], ".bmp");
        if(ptr5 == NULL || strcmp(ptr5, ".bmp"))  //if not bmp file
        {
            printf("%s is not a bmp file\n", argv[4]);
            return e_failure;
        } 
        else   //if is bmp file
        {
            encInfo->stego_image_fname = argv[4];
            return e_success;
        }  
    }   
}

OperationType check_operation_type(char *argv[])
{
    //to check argv[1] is -e or -d
    if(!strcmp(argv[1], "-e") && argv[3] != NULL)
    {
        return e_encode;
    }
    else if(!strcmp(argv[1], "-d"))
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

//encoding function that contains all sub function calls
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        printf("INFO: ## Encoding Procedure Started ##\n");
        if(check_capacity(encInfo) == e_success)
        {
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
            {
                if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
                    {
                        if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            if(encode_secret_file_size(encInfo->secret_file_size, encInfo) == e_success)
                            {
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("INFO: ## Encoding Done Successfully ##\n");
                                        return e_success;
                                    }
                                    else
                                    {
                                        return e_failure;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    }
    return e_failure;
}

Status check_capacity(EncodeInfo *encInfo)
{
    int image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    int magic_string_len = strlen(MAGIC_STRING);
    int extension_len = strlen(encInfo->extn_secret_file);

    printf("INFO: Checking for %s size\n", encInfo->secret_fname);

    //stores secret file size
    encInfo->secret_file_size = get_file_size(encInfo->fptr_secret);
    if(encInfo->secret_file_size != 0)
    {
        printf("INFO: Done. Not empty\n");
    }

    int encoding_things = 54 + ((magic_string_len + 4 + extension_len + 4 + encInfo->secret_file_size) * 8);

    printf("INFO: Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);
    if(encoding_things < image_capacity)
    {

        //just  for display below prompt
        printf("INFO: Done. Found OK\n");
        return e_success;
    }
    else
    {
        printf("INFO: %s doesn't have the capacity to encode %s\n", encInfo->src_image_fname, encInfo->secret_fname);
        return e_failure;
    }

}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying Image Header\n");
    fseek(fptr_src_image, 0, SEEK_SET);  //moves the file pointer offset to 0th index

    char buffer[54];   //buffer to store 54 bytes of data
    if(fread(buffer, 1, 54, fptr_src_image) != 54)   //reads 54 bytes of data and store in buffer and check if 54 bytes is read properly.
    {
        printf("Error while reading header\n");   //prints error message
        return e_failure;
    }

    if(fwrite(buffer, 1, 54, fptr_dest_image) != 54)    //writes the 54 bytes to destination / stego.bmp and checks if 54 bytes are witten properly.
    {
        printf("Error while writing header\n");
        return e_failure;
    }
    printf("INFO: Done\n");
    return e_success;   
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encode Magic String Signature\n");
    if(encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    for(int i = 0; i < size; i++)
    {
        if(fread(buffer, 1, 8, fptr_src_image) != 8)   //reads 8 bytes of data and store in buffer and check if 8 bytes is read properly.
        {
            printf("Error while reading data\n");   //prints error message
            return e_failure;
        }
        if(encode_byte_to_lsb(data[i], buffer) == e_success)
        {
            if(fwrite(buffer, 1, 8, fptr_stego_image) != 8)    //writes the 8 bytes to destination / stego.bmp and checks if 8 bytes are witten properly.
            {
                printf("Error while writing data\n");
                return e_failure;
            }
        }
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & (~1)) | ((data & (1 << (7 - i))) >> (7 - i));  //encode each bits of data to 8 bytes of image buffer.
    }
    return e_success;
}

Status encode_int_to_lsb(int data, char *image_buffer)
{
    for(int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & (~1)) | ((data & (1 << (31 - i))) >> (31 - i));  //encode each bits of data to 32 bytes of image buffer.
    }
    return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File extension size\n", encInfo->secret_fname);
    char buffer[32];
    if(fread(buffer, 1, 32, encInfo->fptr_src_image) != 32)   //reads 32 bytes of data and store in buffer and check if 32 bytes is read properly.
    {
        printf("Error while reading data for encoding secret file extension size\n");   //prints error message
        return e_failure;
    }
    if(encode_int_to_lsb(size, buffer) == e_success)
    {
        if(fwrite(buffer, 1, 32, encInfo->fptr_stego_image) != 32)    //writes the 32 bytes to destination / stego.bmp and checks if 32 bytes are witten properly.
        {
            printf("Error while writing data for encoding secret file extension size\n");
            return e_failure;
        }
    }
    printf("INFO: Done\n");
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File extension\n", encInfo->secret_fname);
    if(encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)  //function call for encoding file extn data
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }

}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File size\n", encInfo->secret_fname);
    char buffer[32];
    if(fread(buffer, 1, 32, encInfo->fptr_src_image) != 32)   //reads 32 bytes of data and store in buffer and check if 32 bytes is read properly.
    {
        printf("Error while reading data for storing file size\n");   //prints error message
        return e_failure;
    }
    if(encode_int_to_lsb(file_size, buffer) == e_success)
    {
        if(fwrite(buffer, 1, 32, encInfo->fptr_stego_image) != 32)    //writes the 32 bytes to destination / stego.bmp and checks if 32 bytes are witten properly.
        {
            printf("Error while writing data for storing file size\n");
            return e_failure;
        }
    }
    printf("INFO: Done\n");
    return e_success; 
}

//for encoding file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File data\n", encInfo->secret_fname);
    char secret_file_data[encInfo->secret_file_size];   //buffer
    if(fread(secret_file_data, 1, encInfo->secret_file_size, encInfo->fptr_secret) != encInfo->secret_file_size)   //reads secretfile data and store in buffer and check if  read properly.
    {
        printf("Error while reading secret file data\n");   //prints error message
        return e_failure;
    }
    if(encode_data_to_image(secret_file_data, encInfo->secret_file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)  //function call for encoding file extn data
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

//to copy remaining data
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying Left Over Data\n");
    char ch;
    while (fread(&ch, 1, 1, fptr_src) == 1) 
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }

    fclose(fptr_src); //close sample.mp3
    fclose(fptr_dest); //close temp.mp3
    printf("INFO: Done\n");
    return e_success;
}
 
//to get file size
uint get_file_size(FILE *fptr)
{
    //for moving file pointer to the end of file
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);  //to return file pointer to 0th index
    return size;  //to return current fp location
}
