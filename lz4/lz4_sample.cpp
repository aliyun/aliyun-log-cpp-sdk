#include "lz4/lz4.h"
#include "string"
#include "iostream"
using namespace std;

int main(void)
{
    string sample_string = "aaaaaaaaaaaabbbbbbbbbbbbbbbbbbbccccccccccccccccdddddddddddddddddd";
    int original_size = sample_string.size();
    int bound = LZ4_compressBound(original_size);
    string compressed;
    compressed.resize(bound);
    int compressed_size = LZ4_compress(sample_string.c_str(), (char*)compressed.c_str(), original_size); 
    if (compressed_size <= 0) 
    {
        cout << "Failed to compress data" << endl;
        return -1;
    }
    compressed.resize(compressed_size);
    string decompressed;
    decompressed.resize(original_size);
    int decompressed_size = LZ4_decompress_safe(compressed.c_str(), (char*)decompressed.c_str(), compressed.size(), original_size);
    if (decompressed_size != original_size) 
    {
        cout << "Failed to de-compress data" << endl;
        return -1;
    }
    cout << "source size:" << original_size << "\tdata:" << sample_string << endl;
    cout << "compressed size:" << compressed_size << "\tdata:" << compressed << endl;
    cout << "decompressed size:" << decompressed_size << "\tdata:" << decompressed << endl;
    return 0;
}
