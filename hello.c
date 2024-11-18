#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265358979323846

void write_wav_header(FILE *file, int sample_rate, int channels, int bits_per_sample, int samples) {
    int byte_rate = sample_rate * channels * bits_per_sample / 8;
    int block_align = channels * bits_per_sample / 8;
    int subchunk2_size = samples * channels * bits_per_sample / 8;
    int chunk_size = 36 + subchunk2_size;

    fwrite("RIFF", 1, 4, file);
    fwrite(&chunk_size, 4, 1, file);
    fwrite("WAVE", 1, 4, file);
    fwrite("fmt ", 1, 4, file);
    
    int subchunk1_size = 16;
    short audio_format = 1;
    fwrite(&subchunk1_size, 4, 1, file);
    fwrite(&audio_format, 2, 1, file);
    fwrite(&channels, 2, 1, file);
    fwrite(&sample_rate, 4, 1, file);
    fwrite(&byte_rate, 4, 1, file);
    fwrite(&block_align, 2, 1, file);
    fwrite(&bits_per_sample, 2, 1, file);

    fwrite("data", 1, 4, file);
    fwrite(&subchunk2_size, 4, 1, file);
}

void generate_wave(int fs, int m, int c, const char* wavetype, double f, double A, double T, short *buffer) {
    int samples = fs * T;

    for (int n = 0; n < samples; n++) {
        double t = (double)n / fs;
        double value = 0.0;

        if (strcmp(wavetype, "sine") == 0) {
            value = A * sin(2 * PI * f * t);
        } else if (strcmp(wavetype, "sawtooth") == 0) {
            value = A * (2.0 * (t * f - floor(t * f + 0.5)));
        } else if (strcmp(wavetype, "square") == 0) {
            value = A * (sin(2 * PI * f * t) >= 0 ? 1.0 : -1.0);
        } else if (strcmp(wavetype, "triangle") == 0) {
            value = A * (1.0 - fabs(fmod(t * f + 0.5, 1.0) * 2.0 - 1.0));
        }

        buffer[n * c] = (short)(value * ((1 << (m - 1)) - 1));
    }
}

double calculate_sqnr(short *buffer, int num_samples, int m) {
    double signal_power = 0, noise_power = 0;
    int max_level = (1 << (m - 1)) - 1;

    for (int i = 0; i < num_samples; i++) {
        double quantized = round(buffer[i] * max_level) / max_level;
        signal_power += buffer[i] * buffer[i];
        noise_power += (buffer[i] - quantized) * (buffer[i] - quantized);
    }

    signal_power /= num_samples;
    noise_power /= num_samples;
    
    return 10 * log10(signal_power / noise_power);
}

int main(int argc, char *argv[]) {
    if (argc != 8) {
        fprintf(stderr, "Usage: %s fs m c wavetype f A T\n", argv[0]);
        return 1;
    }

    int fs = atoi(argv[1]);
    int m = atoi(argv[2]);
    int c = atoi(argv[3]);
    const char* wavetype = argv[4];
    double f = atof(argv[5]);
    double A = atof(argv[6]);
    double T = atof(argv[7]);
    int samples = fs * T;
    
    short *buffer = (short *)malloc(samples * c * sizeof(short));
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    generate_wave(fs, m, c, wavetype, f, A, T, buffer);

    // Write WAV header to stdout
    write_wav_header(stdout, fs, c, m, samples);
    fwrite(buffer, sizeof(short), samples * c, stdout);

    double sqnr = calculate_sqnr(buffer, samples, m);
    fprintf(stderr, "%.15f\n", sqnr);

    free(buffer);
    return 0;
}











