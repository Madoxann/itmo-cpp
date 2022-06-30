#include "return_codes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ZLIB
	#include <zlib.h>
#elifdef LIBDEFLATE
	#include <libdeflate.h>
#elifdef ISAL
	#include <isa-l.h>
#else
	#error "Library isn't supported"
#endif

typedef unsigned char Byte;
typedef unsigned int uint32_t;

typedef struct
{
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t c_type;
	uint32_t cmp_method;
	uint32_t filter;
	uint32_t interlace;
} IHDR, *pIHDR;

uint32_t getReversedBytes(const Byte* data)
{
	uint32_t size = 0;
	for (int i = 0; i < 4; i++)
		size |= data[i] << 8 * (3 - i);

	return size;
}

int getPaethPredicator(int a, int b, int c)
{
	int p = a + b - c;
	int pa = abs(p - a);
	int pb = abs(p - b);
	int pc = abs(p - c);
	int Pr;
	if (pa <= pb && pa <= pc)
		Pr = a;
	else if (pb <= pc)
		Pr = b;
	else
		Pr = c;
	return Pr;
}

int getIHDR(FILE* stream, pIHDR init)
{
	Byte data[13];
	size_t result = fread(data, 1, 13, stream);
	if (result < 13)
		return ERROR_INVALID_DATA;

	init->width = getReversedBytes(data);
	init->height = getReversedBytes(&data[4]);
	init->depth = data[8];
	init->c_type = data[9];
	init->cmp_method = data[10];
	init->filter = data[11];
	init->interlace = data[12];
	return ERROR_SUCCESS;
}

int fseekFailure(FILE* stream)
{
	fclose(stream);
	fprintf(stderr, "fseek failed to move ptr in stream");
	return ERROR_UNKNOWN;
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "Invalid argument amount");
		return ERROR_INVALID_PARAMETER;
	}

	FILE* png = fopen(argv[1], "rb");
	if (!png)
	{
		fprintf(stderr, "Failed to open file");
		return ERROR_NOT_FOUND;
	}

	Byte signature[8], size[4], type[4] /*, crc[4]*/;
	fread(signature, 1, 8, png);
	Byte validSignature[] = { 137, 'P', 'N', 'G', 13, 10, 26, 10 };
	if (memcmp(signature, validSignature, 8))
	{
		fprintf(stderr, "Format is not compatible, png required");
		fclose(png);
		return ERROR_INVALID_DATA;
	}

	fread(size, 1, 4, png);
	if (getReversedBytes(size) != 13)
	{
		fprintf(stderr, "IHDR chunk should be 13 bytes long");
		fclose(png);
		return ERROR_INVALID_DATA;
	}

	fread(type, 1, 4, png);
	if (memcmp(type, "IHDR", 4))
	{
		fprintf(stderr, "IHDR chunk expected");
		fclose(png);
		return ERROR_INVALID_DATA;
	}

	IHDR data;
	size_t result = getIHDR(png, &data);
	if (result == ERROR_INVALID_DATA)
	{
		fprintf(stderr, "Failed to read IHDR data, unexpected EOF");
		fclose(png);
		return ERROR_INVALID_DATA;
	}
	if (data.depth != 8)
	{
		fprintf(stderr, "Unsupported amount of bit to channel");
		fclose(png);
		return ERROR_INVALID_DATA;
	}
	if (data.c_type != 0 && data.c_type != 2)
	{
		fprintf(stderr, "Unsupported color type");
		fclose(png);
		return ERROR_INVALID_DATA;
	}
	result = fseek(png, 4, 1);
	if (result)
		return fseekFailure(png);

	for (;;)
	{
		fread(size, 1, 4, png);
		result = fread(type, 1, 4, png);
		if (result < 4)
		{
			fprintf(stderr, "Unexpected EOF");
			fclose(png);
			return ERROR_INVALID_DATA;
		}
		if (!memcmp(type, "PLTE", 4) && !data.c_type)
		{
			fprintf(stderr, "Unexpected PLTE chunk ar color type 0!");
			fclose(png);
			return ERROR_INVALID_DATA;
		}
		if (!memcmp(type, "IDAT", 4))
			break;
		result = fseek(png, getReversedBytes(size) + 4 /*crc skipped*/, 1);
		if (result)
			return fseekFailure(png);
	}

	size_t currentSize = 128, actualMemory = 0;
	Byte* encoded = malloc(sizeof(Byte) * currentSize);
	if (!encoded)
	{
		fprintf(stderr, "Failed to allocate memory");
		fclose(png);
		return ERROR_MEMORY;
	}
	while (!memcmp(type, "IDAT", 4))
	{
		size_t sizeData = getReversedBytes(size);
		while (currentSize < actualMemory + sizeData)
		{
			Byte* newEncoded = realloc(encoded, currentSize *= 2);
			if (!newEncoded)
			{
				fprintf(stderr, "Failed to reallocate memory");
				free(encoded);
				fclose(png);
				return ERROR_MEMORY;
			}
			encoded = newEncoded;
		}
		fread(encoded + actualMemory, 1, sizeData, png);
		actualMemory += sizeData;
		result = fseek(png, 4, 1);
		if (result)
			return fseekFailure(png);
		fread(size, 1, 4, png);
		result = fread(type, 1, 4, png);
		if (result < 4)
		{
			fprintf(stderr, "Unexpected EOF");
			fclose(png);
			free(encoded);
			return ERROR_INVALID_DATA;
		}
	}
	while (memcmp(type, "IEND", 4))
	{
		fseek(png, getReversedBytes(size) + 4, 1);
		fread(size, 1, 4, png);
		size_t read = fread(type, 1, 4, png);
		if (read < 4)
		{
			fprintf(stderr, "EOF reached, no IEND found");
			fclose(png);
			free(encoded);
			return ERROR_INVALID_DATA;
		}
	}
	if (getReversedBytes(size) != 0)
	{
		fprintf(stderr, "IEND size must be 0");
		fclose(png);
		free(encoded);
		return ERROR_INVALID_DATA;
	}
	fclose(png);

	size_t length = data.height * ((data.c_type + 1) * data.width + 1);
	Byte* decoded = malloc(sizeof(Byte) * length);
	if (!decoded)
	{
		fprintf(stderr, "Failed to allocate memory");
		free(encoded);
		return ERROR_MEMORY;
	}

#ifdef ZLIB
	result = uncompress(decoded, &length, encoded, actualMemory);
	if (result)
	{
		fprintf(stderr, "Uncompress failure! Zlib failed to uncompress with following error code");
		free(encoded);
		free(decoded);

		return (int)result;
	}
#elifdef LIBDEFLATE
	struct libdeflate_decompressor* uncompress;
	uncompress = libdeflate_alloc_decompressor();
	result = libdeflate_zlib_decompress(uncompress, encoded, actualMemory, decoded, length, NULL);
	if (result != LIBDEFLATE_SUCCESS)
	{
		fprintf(stderr, "Uncompress failure! Libdeflate failed to uncompress with following error code");
		free(encoded);
		free(decoded);
		libdeflate_free_decompressor(uncompress);
		return (int)result;
	}
	libdeflate_free_decompressor(uncompress);
#elifdef ISAL
	struct inflate_state uncompress;
	isal_inflate_init(&uncompress);

	uncompress.next_in = encoded;
	uncompress.avail_in = actualMemory;
	uncompress.next_out = decoded;
	uncompress.avail_out = length;
	uncompress.crc_flag = IGZIP_ZLIB;
	result = isal_inflate(&uncompress);
	if (result != ISAL_DECOMP_OK)
	{
		fprintf(stderr, "Uncompress failure! ISA-L failed to uncompress with following error code");
		free(encoded);
		free(decoded);
		return (int)result;
	}
#endif
	free(encoded);

	size_t block = data.width * (data.c_type + 1) + 1;
	Byte filterType = 0;
	for (size_t i = 0; i < length; i++)
	{
		int hasLeft = ((i % block) > (data.c_type + 1));
		int hasUp = i >= block;
		if (!(i % block))
		{
			filterType = decoded[i];
		}
		else
		{
			switch (filterType)
			{
			case 0:
				break;
			case 1:
				if (hasLeft)
					decoded[i] += decoded[i - data.c_type - 1];
				break;
			case 2:
				if (hasUp)
					decoded[i] += decoded[i - block];
				break;
			case 3:
				if (!hasLeft && hasUp)
				{
					decoded[i] += decoded[i - block] / 2;
					break;
				}
				if (hasLeft && !hasUp)
				{
					decoded[i] += decoded[i - data.c_type - 1] / 2;
					break;
				}
				if (hasLeft)
				{
					decoded[i] += (decoded[i - data.c_type - 1] + decoded[i - block]) / 2;
					break;
				}
				break;
			case 4:
				if (!hasLeft && hasUp)
				{
					decoded[i] += getPaethPredicator(0, decoded[i - block], 0);
					break;
				}
				if (hasLeft && !hasUp)
				{
					decoded[i] += getPaethPredicator(decoded[i - data.c_type - 1], 0, 0);
					break;
				}
				if (hasLeft)
				{
					decoded[i] +=
						getPaethPredicator(decoded[i - data.c_type - 1], decoded[i - block], decoded[i - data.c_type - 1 - block]);
					break;
				}
				break;
			default:
				fprintf(stderr, "Unsupported filter type!");
				free(decoded);
				return ERROR_INVALID_DATA;
			}
		}
	}

	FILE* pnm = fopen(argv[2], "wb");
	if (!pnm)
	{
		fprintf(stderr, "Failed to open file");
		free(decoded);
		return ERROR_NOT_FOUND;
	}

	fprintf(pnm, (!data.c_type) ? "P5\n%u %u\n255\n" : "P6\n%u %u\n255\n", data.width, data.height);
	for (int i = 0; i < data.height; i++)
	{
		result = fwrite(decoded + block * i + 1, 1, (data.c_type + 1) * data.width, pnm);
		if (result < data.width * (data.c_type + 1))
		{
			fprintf(stderr, "File writing error!");
			fclose(pnm);
			free(decoded);
			return ERROR_UNKNOWN;
		}
	}
	fclose(pnm);
	free(decoded);

	return ERROR_SUCCESS;
}
