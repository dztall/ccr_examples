//Original source code found at : https://gist.github.com/ironpinguin/7627623#file-freetype2_mono-c

/* First test with FreeType2 library       */
/* Output to console of mono rendered font */
/*
* FreeType2_Mono.c
*
* Created by Michele Catalano <michele@catalano.de>.
*
* Copyright (c) 2013 Michele Catalano
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* Neither the name of the project's author nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define FONT_FILENAME "SourceSansPro-Bold.otf"

#define WIDTH   128
#define HEIGHT  64

typedef struct Text_Dimensions_
{
	int            width;
	int            height;
	int            baseline;
	unsigned char *outpuffer;
} Text_Dimension;

typedef struct Mono_Glyph_
{
	int            top;
	int            height;
	int            width;
	int            descent;
	int            ascent;
	int            advance_width;
	unsigned char *pixelmap;
} Mono_Glyph;

/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];

/* Get Kerning offset if needed. */
int kerning_offset(char c, char pc, FT_Face face)
{
	FT_Vector kerning;
	int error;

	FT_Get_Kerning(face, c, pc, FT_KERNING_DEFAULT, &kerning);

	return kerning.x / 64;
}

/* convert FreeType glyph bitmap to a simple 0|1 bitmap */
unsigned char* unpack_mono_bitmap(FT_Bitmap bitmap)
{
	unsigned char* result;
	int y, x, byte_index, num_bits_done, rowstart, bits, bit_index;
	unsigned char byte_value;

	result = (unsigned char*)malloc(bitmap.rows * bitmap.width);

	for (y = 0; y < bitmap.rows; y++)
	{
		for (byte_index = 0; byte_index < bitmap.pitch; byte_index++)
		{
			byte_value = bitmap.buffer[y * bitmap.pitch + byte_index];

			num_bits_done = byte_index * 8;

			rowstart = y * bitmap.width + byte_index * 8;

			bits = 8;
			if (((int)bitmap.width - num_bits_done) < 8)
				bits = bitmap.width - num_bits_done;

			for (bit_index = 0; bit_index < bits; bit_index++)
			{
				int bit;
				bit = byte_value & (1 << (7 - bit_index));

				result[rowstart + bit_index] = bit;
			}
		}
	}

	return result;
}

/* Convert monochrome glyph slot information to simple structure. */
Mono_Glyph convert_glyph(FT_GlyphSlot slot)
{
	Mono_Glyph glyph_data;
	int descent = 0, ascent = 0, ascent_calc;
	unsigned char *pixelmap;

	pixelmap = unpack_mono_bitmap(slot->bitmap);
	glyph_data.pixelmap = pixelmap;
	glyph_data.width = slot->bitmap.width;
	glyph_data.height = slot->bitmap.rows;
	glyph_data.top = slot->bitmap_top;
	glyph_data.advance_width = slot->advance.x / 64;
	if (descent < (glyph_data.height - glyph_data.top))
		descent = glyph_data.height - glyph_data.top;
	glyph_data.descent = descent;

	if (glyph_data.top < glyph_data.height)
		ascent_calc = glyph_data.height;
	else
		ascent_calc = glyph_data.top;

	if (ascent < (ascent_calc - descent))
		ascent = ascent_calc - descent;
	glyph_data.ascent = ascent;

	return glyph_data;
}

/* Get simple monochrome structure for given char. */
Mono_Glyph get_glyph(char character, FT_Face face)
{
	int error;

	/* load glyph image into the slot (erase previous one) */
	error = FT_Load_Char(face, character, FT_LOAD_RENDER | FT_LOAD_TARGET_MONO);
	if (error)
	{
		// TODO error handling.
	}

	return convert_glyph(face->glyph);
}

/* Get dimensions information of the given Test. */
Text_Dimension text_dimension(char* text, FT_Face face)
{
	int width, max_ascent, max_descent, kerning_x, n, num_chars, error;
	char character, prev_character;
	Text_Dimension result;
	Mono_Glyph glyph;
	FT_Vector kerning;

	width = 0;
	max_ascent = 0;
	max_descent = 0;
	prev_character = 0;

	num_chars = strlen(text);

	for (n = 0; n < num_chars; n++)
	{
		character = text[n];
		glyph = get_glyph(character, face);
		if (max_ascent < glyph.ascent)
			max_ascent = glyph.ascent;
		if (max_descent < glyph.descent)
			max_descent = glyph.descent;

		kerning_x = kerning_offset(character, prev_character, face);

		if ((glyph.advance_width + kerning_x) < (glyph.width + kerning_x))
			width += (glyph.width + kerning_x);
		else
			width += (glyph.advance_width + kerning_x);

		free(glyph.pixelmap);

		prev_character = character;
	}
	result.height = max_ascent + max_descent;
	result.width = width;
	result.baseline = max_descent;
	result.outpuffer = (unsigned char*)calloc(
		result.height * result.width, 1);

	return result;
}

/* Draw char to puffer on button. */
void draw_puffer(Mono_Glyph* glyph, int x, int y, Text_Dimension text_info)
{
	int srcpixel, dstpixel, row_offset, sx, sy;

	srcpixel = 0;
	dstpixel = y * text_info.width + x;
	row_offset = text_info.width - glyph->width;

	for (sy = 0; sy < glyph->height; sy++)
	{
		for (sx = 0; sx < glyph->width; sx++)
		{
			text_info.outpuffer[dstpixel] |= glyph->pixelmap[srcpixel];
			srcpixel++;
			dstpixel++;
		}
		dstpixel += row_offset;
	}
}

void show_image(Text_Dimension text_dimensions)
{
	int  i, j;

	for (i = 0; i < text_dimensions.height; i++)
	{
		for (j = 0; j < text_dimensions.width; j++)
		{
			_Bool blackPixel = text_dimensions.outpuffer[text_dimensions.width * i + j] != 0;
			printf("%s", blackPixel ? "■" : "□");
		}
		putchar('\n');
	}
}

const char* getErrorMessage(FT_Error err)
{
    #undef __FTERRORS_H__
    #define FT_ERRORDEF( e, v, s )  case e: return s;
    #define FT_ERROR_START_LIST     switch (err) {
    #define FT_ERROR_END_LIST       }
    #include FT_ERRORS_H
    return "(Unknown error)";
}

int main()
{
	FT_Library     library;
	FT_Face        face;

	FT_GlyphSlot   slot;
	/* untransformed origin  */
	FT_Vector      pen;
	FT_Error       error;

	char*          filename;
	char*          text;

	int            target_height;
	int            n, num_chars, pixel;
	Text_Dimension text_dimensions;
	Mono_Glyph     glyph;
	char           c, pc;

	c = 0;
	pc = 0;

	filename = FONT_FILENAME;	/* first argument     */
	pixel = 16;	/* second argument    */
	text = "C++";	/* third argument     */
	num_chars = strlen(text);
	target_height = HEIGHT;

	/* initialize library */
	error = FT_Init_FreeType(&library);
	if(error != FT_Err_Ok)
	{
		printf("Error on FT_Init_FreeType(...);\n'%s'\n", getErrorMessage(error));
		return -1;
	}

	/* create face object */
	error = FT_New_Face(library, filename, 0, &face);
	if(error != FT_Err_Ok)
	{
		FT_Done_FreeType(library);
		printf("Error on FT_New_Face(...);\n'%s'\n", getErrorMessage(error));
		return -1;
	}

	error = FT_Set_Pixel_Sizes(face, 0, pixel);
	if(error != FT_Err_Ok)
	{
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		printf("Error on FT_Set_Pixel_Sizes(...);\n'%s'\n", getErrorMessage(error));
		return -1;
	}

	slot = face->glyph;

	text_dimensions = text_dimension(text, face);

	pen.x = 0;

	for (n = 0; n < num_chars; n++)
	{
		c = text[n];

		glyph = get_glyph(c, face);

		pen.x += kerning_offset(c, pc, face);
		pen.y = text_dimensions.height - glyph.ascent -
			text_dimensions.baseline;

		draw_puffer(&glyph, pen.x, pen.y, text_dimensions);

		pen.x += glyph.advance_width;

		free(glyph.pixelmap);

		pc = c;
	}

	show_image(text_dimensions);

	free(text_dimensions.outpuffer);

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	return 0;
}