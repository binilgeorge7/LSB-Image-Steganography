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
#include "decode.h"
#include "types.h"

int main(int argc, char* argv[])
{
    //to validate argument count
    if(!(argc == 3 || argc == 4 || argc == 5))  //count should be 4 or 5
    {
        printf("Invalid number of arguments");
        return e_failure;
    }

    
    OperationType op_type = check_operation_type(argv);
    
    if(op_type == e_encode)
    {
        EncodeInfo encInfo;  //structure variable declaration
        if(read_and_validate_encode_args(argv, &encInfo, argc) == e_success)
        {
            do_encoding(&encInfo);
        }
    }
    else if(op_type == e_decode)
    {
        DecodeInfo decInfo; //struct variable declaration
        if(read_and_validate_decode_args(argv, &decInfo, argc) == e_success)
        {
            do_decoding(&decInfo);
        }
    }
    else if(op_type == e_unsupported)
    {
        printf("Unsupported cmd arguments\n");
    }

}
