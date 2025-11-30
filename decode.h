#ifndef DECODE_H
#define DECODE_H

#include<stdio.h>
#include "types.h" // Contains user defined types

typedef struct _DecodeInfo
{
    char *stego_image_fname;   //store encoded stego image file name
    FILE *fptr_stego_image;

    //secret file
    char output_secret_fname[20];     //store output secret file name
    FILE *fptr_output_secret;          //file ptr for output secret file
    int secret_extn_size;
    char secret_extn[5];
    int secret_file_size;

} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo, int argc);

Status do_decoding(DecodeInfo *decInfo);

Status open_img_file(DecodeInfo *decInfo);

Status skip_bmp_header(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

char decode_byte_from_lsb(char* buffer);

int decode_int_from_lsb(char* buffer);

#endif