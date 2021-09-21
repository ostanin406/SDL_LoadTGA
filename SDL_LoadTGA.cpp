#include <cstdio>
#include <SDL.h>

#pragma warning(push)
#pragma warning(disable:6386)
#pragma warning(disable:6011)
char* LoadTGA(const char* file, int& width, int& height, int& bpp, int& pitch)
{
	char pics; short picsize; char* data = NULL;
	FILE* f; fopen_s(&f, file, "rb");
	if (!f) return data;

	fseek(f, 12L, SEEK_SET);

	fread(&picsize, sizeof(short), 1, f); width = picsize;
	fread(&picsize, sizeof(short), 1, f); height = picsize;
	fread(&pics, sizeof(char), 1, f); bpp = pics;
	fread(&pics, sizeof(char), 1, f);

	if (width > 0 && height > 0 && (bpp == 24 || bpp == 32))
	{
		int bpp8 = bpp / 8, pixnum = width * height;
		data = (char*)malloc(pixnum * bpp8 * sizeof(char));
		pitch = bpp8 * width;

		for (int i = 0; i < pixnum; i++)
		{
			data[i * bpp8 + 2] = fgetc(f);
			data[i * bpp8 + 1] = fgetc(f);
			data[i * bpp8 + 0] = fgetc(f);

			if (bpp == 32)
				data[i * bpp8 + 3] = fgetc(f);
		}
	}
	fclose(f);
	return data;
}
#pragma warning(pop)

void SDL_RotateSurfaceVertical(SDL_Surface* surface)
{
	if (surface->h < 2) return;
	SDL_LockSurface(surface);

	char* temp = new char[surface->pitch];
	char* pixels = (char*)surface->pixels;

	for (int i = 0; i < surface->h / 2; ++i)
	{
		char* row1 = pixels + i * surface->pitch;
		char* row2 = pixels + (surface->h - i - 1) * surface->pitch;

		SDL_memcpy(temp, row1, surface->pitch);
		SDL_memcpy(row1, row2, surface->pitch);
		SDL_memcpy(row2, temp, surface->pitch);
	}

	delete[] temp;
	SDL_UnlockSurface(surface);
}

SDL_Surface* SDL_LoadTGA(const char* file)
{
	int width, height, bpp, pitch;
	char* data = LoadTGA(file, width, height, bpp, pitch);
	if (data == NULL) return NULL;

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(data, width, height, bpp, pitch,
		0x000000FF, 0x0000FF00, 0x00FF0000, (bpp == 32) ? 0xFF000000 : 0);

	SDL_RotateSurfaceVertical(surface);
	return surface;
}