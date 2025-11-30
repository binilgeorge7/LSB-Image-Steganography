# 🔐 LSB Image Steganography – Encoding & Decoding

This project implements **Least Significant Bit (LSB) Image Steganography** in **C**, enabling secure storage and retrieval of confidential data inside BMP images **without affecting image quality**.

---

## 📌 Features
- 🔹 Encode any `.txt`, `.c` or `.sh` file inside a BMP image  
- 🔹 Recover the exact file from the stego image during decoding  
- 🔹 High security using **Magic String validation**  
- 🔹 **Capacity check** to avoid overflow before encoding  
- 🔹 Clean and modular design with **proper logging & error handling**  
- 🔹 Command line support:
  - `-e` → Encoding
  - `-d` → Decoding

---

## ⚙️ Tech Stack
| Component | Technology |
|----------|------------|
| Language | C |
| Concepts | File Handling, Bitwise Operations, Strings, Pointers, BMP Headers |

---

## 🧠 How It Works
| Operation | Input | Output |
|----------|--------|--------|
| Encoding | BMP image + Secret file | Stego (encoded) BMP image |
| Decoding | Stego BMP image | Original secret file restored |

LSB encoding modifies only the **least significant bit** of pixel data, keeping visible image quality unchanged.

---

## 📂 Project Structure
├── main.c
├── encode.c / encode.h
├── decode.c / decode.h
├── common.h
├── types.h
└── README.md

yaml
Copy code

---

## 🚀 Usage

### 🔸 Encoding
./stego -e <input_image.bmp> <secret_file.txt/c/sh> [output_image.bmp]

makefile
Copy code
Example:
./stego -e sample.bmp secret.txt hide.bmp

shell
Copy code

### 🔸 Decoding
./stego -d <stego_image.bmp> [output_filename]

makefile
Copy code
Example:
./stego -d hide.bmp output

yaml
Copy code

---

## 📌 Logs Preview
INFO: Opening required files
INFO: ## Encoding Procedure Started ##
INFO: Encode Magic String Signature
INFO: Encoding secret.txt File extension size
INFO: Encoding secret.txt File extension
INFO: Encoding secret.txt File size
INFO: Encoding secret.txt File data
INFO: ## Encoding Done Successfully ##

yaml
Copy code

---

## 🖼️ Screenshots (Optional)
execution screenshots here for better visibility_
<img width="1920" height="1080" alt="Screenshot (644)" src="https://github.com/user-attachments/assets/442ca3d2-d4d5-44b1-b22f-ea44824fddd1" />


📌 Example:

Encoding terminal output

Decoding terminal output

Original & Stego image preview

yaml
Copy code

---

## 📎 Future Enhancements
✔ Support for JPG/PNG  
✔ GUI interface  
✔ Password-based protection  
✔ AES encryption before encoding  

---

## 🙌 Acknowledgements
Special thanks to my mentors **[Mentor 1 Name]** and **[Mentor 2 Name]** for their guidance and constant support throughout the development of this project.

---

## 🧑‍💻 Author
**BINIL GEORGE**  
📌 GitHub: https://github.com/binilgeorge7
📌 LinkedIn: www.linkedin.com/in/binil-george-9a0552186

---

## ⭐ Support
If you like this project, don’t forget to **star ⭐ the repository** — it motivates me to build more!

---

### 🔖 Hashtags (for repo visibility)
`#Steganography #CyberSecurity #LSB #CProgramming #ImageProcessing #OpenSource`

