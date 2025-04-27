
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define STATE_SIZE 288

uint8_t state[STATE_SIZE];

void initialize(const uint8_t key[80], const uint8_t iv[80]) {
    memset(state, 0, sizeof(state));

    for (int i = 0; i < 80; i++) {
        state[i] = key[i];
    }

    for (int i = 0; i < 80; i++) {
        state[i + 93] = iv[i];
    }

    state[285] = 1;
    state[286] = 1;
    state[287] = 1;

    for (int i = 0; i < 1152; i++) {
        uint8_t t1 = state[65] ^ (state[90] & state[91]) ^ state[92] ^ state[170];
        uint8_t t2 = state[161] ^ (state[174] & state[175]) ^ state[176] ^ state[263];
        uint8_t t3 = state[242] ^ (state[285] & state[286]) ^ state[287] ^ state[68];

        memmove(&state[1], &state[0], 92);
        memmove(&state[94], &state[93], 83);
        memmove(&state[178], &state[177], 110);

        state[0] = t3;
        state[93] = t1;
        state[177] = t2;
    }
}

uint8_t trivium_gen() {
    uint8_t z = state[65] ^ state[92] ^ state[161] ^ state[176] ^ state[242] ^ state[287];

    uint8_t t1 = state[65] ^ (state[90] & state[91]) ^ state[92] ^ state[170];
    uint8_t t2 = state[161] ^ (state[174] & state[175]) ^ state[176] ^ state[263];
    uint8_t t3 = state[242] ^ (state[285] & state[286]) ^ state[287] ^ state[68];

    memmove(&state[1], &state[0], 92);
    memmove(&state[94], &state[93], 83);
    memmove(&state[178], &state[177], 110);

    state[0] = t3;
    state[93] = t1;
    state[177] = t2;

    return z;
}

int main() {
    uint8_t key[80], iv[80];
    FILE *f;

    srand((unsigned int)time(NULL));

    f = fopen("trivium_output.bin", "wb");
    if (!f) {
        printf("Failed to open file trivium_output.bin!\n");
        return 1;
    }

    for (int stream = 1; stream <= 100; stream++) {
        for (int i = 0; i < 80; i++) {
            key[i] = rand() % 2;
            iv[i] = rand() % 2;
        }

        initialize(key, iv);

        for (int i = 0; i < 125000; i++) {
            uint8_t byte = 0;
            for (int j = 0; j < 8; j++) {
                byte = (byte << 1) | trivium_gen();
            }
            fputc(byte, f);
        }

        printf("Stream %d generated and written to trivium_output.bin\n", stream);
    }

    fclose(f);

    return 0;
}
