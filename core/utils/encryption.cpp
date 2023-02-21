/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: high level data encryption and decryption wrappers
 */

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include "encryption.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

int encrypt_data (
    uint8_t* key,
    uint8_t* enc_data, 
    uint32_t* enc_data_size,  // return by reference 
    uint8_t* plain_data, 
    uint32_t plain_data_size)
{ 
    OpenSSL_add_all_ciphers();

    // Encrypted data:      | MAC | IV | AES128(data)
    // Buffer size:           16    12   size(data)
    // MAC reference:         &data       :   &data+16
    // IV reference:          &data+16    :   &data+16+12
    // AES128(data) ref:      &data+12+16 : 
    size_t mac_size = 16;
    size_t iv_size = 12;
    unsigned char mac[mac_size];
    unsigned char iv[iv_size];

    std::vector<unsigned char> output;
    int actual_size = 0, final_size = 0;

    // Compute result size and verify if the memory will be enough for storing the result
    uint32_t real_size = mac_size + iv_size + plain_data_size;
    if(*enc_data_size < real_size) 
        return -1;
    *enc_data_size = real_size;
    output.resize(real_size, '\0');
    
    // Generate nonce (initialization vector IV)
    //srand(time(NULL));
    for(int i=0;i<12;i++)
        //iv[i] = static_cast<uint8_t>(rand()%10) + 48;
        iv[i] = 0;      // use 0 for testing
    // Encrypted data: MAC | IV | AES128(data)
    std::copy(iv, iv + iv_size, output.begin() + mac_size);
    
    // Create a new encryption context using the EVP_CIPHER_CTX_new function
    EVP_CIPHER_CTX* e_ctx = EVP_CIPHER_CTX_new();

    // Perform the encryption of the plaintext
    // Encrypted data: MAC | IV | AES128(data)
    EVP_EncryptInit(e_ctx, EVP_aes_128_gcm(), (const unsigned char*)key, iv);
    EVP_EncryptUpdate(e_ctx, &output[mac_size + iv_size], &actual_size, (const unsigned char*)plain_data, plain_data_size);
    EVP_EncryptFinal(e_ctx, &output[mac_size + iv_size + actual_size], &final_size);
    //printf("actual = %d and final = %d\n", actual_size, final_size);
    
    // Retrieve the authentication tag using EVP_CIPHER_CTX_ctrl function
    EVP_CIPHER_CTX_ctrl(e_ctx, EVP_CTRL_GCM_GET_TAG, mac_size, mac);
    std::copy(mac, mac + mac_size, output.begin());

    memcpy(enc_data, output.data(), real_size);

    EVP_CIPHER_CTX_free(e_ctx);
    return 0;
}

int decrypt_data (
    uint8_t* key,
    uint8_t* enc_data, 
    uint32_t enc_data_size,  
    uint8_t* plain_data, 
    uint32_t* plain_data_size) // return by reference 
{ 
    OpenSSL_add_all_ciphers();

    // Encrypted data:      | MAC | IV | AES128(data)
    // Buffer size:           16    12   size(data)
    // MAC reference:         &data       :   &data+16
    // IV reference:          &data+16    :   &data+16+12
    // AES128(data) ref:      &data+12+16 : 
    size_t mac_size = 16;
    size_t iv_size = 12;
    unsigned char mac[mac_size];
    unsigned char iv[iv_size];
    memcpy(mac, enc_data, mac_size);
    memcpy(iv, enc_data + mac_size, iv_size);

    std::vector<unsigned char> plaintext;
    int actual_size = 0, final_size = 0;

    // Compute result size and verify if the memory will be enough for storing the result
    uint32_t real_size = enc_data_size - mac_size - iv_size;
    if(*plain_data_size < real_size)
        return -1;
    *plain_data_size = real_size;
    plaintext.resize(real_size, '\0');

    // Create a new decryption context using the EVP_CIPHER_CTX_new function
    EVP_CIPHER_CTX *d_ctx = EVP_CIPHER_CTX_new();

    // Perform the decryption of the ciphertext
    EVP_DecryptInit(d_ctx, EVP_aes_128_gcm(), (const unsigned char*)key, iv);
    EVP_DecryptUpdate(d_ctx, &plaintext[0], &actual_size, &enc_data[mac_size+iv_size], real_size);
    EVP_CIPHER_CTX_ctrl(d_ctx, EVP_CTRL_GCM_SET_TAG, mac_size, mac);
    EVP_DecryptFinal(d_ctx, &plaintext[actual_size], &final_size);

    memcpy(plain_data, plaintext.data(), real_size);

    EVP_CIPHER_CTX_free(d_ctx);
    return 0;
}


void quick_decrypt_debug (uint8_t* key, uint8_t* enc, uint32_t enc_size) {
    uint32_t plain_size = enc_size - 12 - 16;
    uint8_t plain[plain_size+1];
    decrypt_data(key, enc, enc_size, plain, &plain_size);
    plain[plain_size] = '\0';
    printf("Decrypted: %s\n", (char*)plain);
}