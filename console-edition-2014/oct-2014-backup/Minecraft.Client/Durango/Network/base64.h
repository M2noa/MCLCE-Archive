#pragma once

#include <string>

Platform::String^ base64_encode(unsigned char* chars_to_encode, unsigned int in_len);
void base64_decode(Platform::String ^encoded_string, unsigned char *output, unsigned int out_len);