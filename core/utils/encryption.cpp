/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: high level data encryption and decryption wrappers
 */

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

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
    // Buffer size:           32    16   size(data)
    // MAC reference:         &data       :   &data+32
    // IV reference:          &data+32    :   &data+32+16
    // AES128(data) ref:      &data+32+16 : 
    size_t mac_size = 32;
    size_t iv_size = 16;
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
    for(int i=0;i<iv_size;i++)
        //iv[i] = static_cast<uint8_t>(rand()%10) + 48;
        iv[i] = 0;      // use 0 for testing
    // Encrypted data: MAC | IV | AES128(data)
    std::copy(iv, iv + iv_size, output.begin() + mac_size);
    
    // Create a new encryption context using the EVP_CIPHER_CTX_new function
    EVP_CIPHER_CTX* e_ctx = EVP_CIPHER_CTX_new();

    // Perform the encryption of the plaintext
    // Encrypted data: MAC | IV | AES128(data)
    EVP_EncryptInit(e_ctx, EVP_aes_128_ctr(), (const unsigned char*)key, iv);
    EVP_EncryptUpdate(e_ctx, &output[mac_size + iv_size], &actual_size, (const unsigned char*)plain_data, plain_data_size);
    EVP_EncryptFinal(e_ctx, &output[mac_size + iv_size + actual_size], &final_size);
    //printf("actual = %d and final = %d\n", actual_size, final_size);
    
    // Cleanup the encryption context
    EVP_CIPHER_CTX_free(e_ctx);

    uint8_t* data_to_be_hashed = (uint8_t*)malloc(plain_data_size+iv_size);
    memcpy(data_to_be_hashed, plain_data, plain_data_size);
    memcpy(data_to_be_hashed+plain_data_size, &output[mac_size], iv_size);

    // Retrieve the authentication tag using SHA-256 hash of the data
    unsigned char sha256_hash[SHA256_DIGEST_LENGTH];
    SHA256(data_to_be_hashed, plain_data_size+iv_size, sha256_hash); 
    std::copy(sha256_hash, sha256_hash + mac_size, output.begin());

    free(data_to_be_hashed);
    memcpy(enc_data, output.data(), real_size);

    //quick_decrypt_debug(key, enc_data, *enc_data_size);

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
    // Buffer size:           32    16   size(data)
    // MAC reference:         &data       :   &data+32
    // IV reference:          &data+32    :   &data+32+16
    // AES128(data) ref:      &data+32+16 : 
    size_t mac_size = 32;
    size_t iv_size = 16;
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
    //printf("Aqui 1\n");

    // Create a new decryption context using the EVP_CIPHER_CTX_new function
    EVP_CIPHER_CTX *d_ctx = EVP_CIPHER_CTX_new();

    // Perform the decryption of the ciphertext
    EVP_DecryptInit(d_ctx, EVP_aes_128_ctr(), (const unsigned char*)key, iv);
    EVP_DecryptUpdate(d_ctx, &plaintext[0], &actual_size, &enc_data[mac_size+iv_size], real_size);

    // Cleanup the encryption context
    EVP_CIPHER_CTX_free(d_ctx);

    //printf("%s\n",plaintext.data()); ok
    size_t plain_size = plaintext.size();

    uint8_t* data_to_be_hashed = (uint8_t*)malloc(plain_size+iv_size);
    memcpy(data_to_be_hashed, plaintext.data(), plain_size);
    memcpy(data_to_be_hashed+plain_size, iv, iv_size);

    // Retrieve the authentication tag using SHA-256 hash of the data
    unsigned char sha256_hash[SHA256_DIGEST_LENGTH];
    SHA256(data_to_be_hashed, plain_size+iv_size, sha256_hash); 
    free(data_to_be_hashed);

    int different = memcmp(sha256_hash, mac, mac_size);
    if(different != 0)
        return -1;
    //printf("Aqui 2\n");

    memcpy(plain_data, plaintext.data(), real_size);

    return 0;
}


void quick_decrypt_debug (uint8_t* key, uint8_t* enc, uint32_t enc_size) {
    uint32_t plain_size = enc_size - 32 - 16;
    uint8_t plain[plain_size+1];
    decrypt_data(key, enc, enc_size, plain, &plain_size);
    plain[plain_size] = '\0';
    printf("Decrypted: %s\n", (char*)plain);
}