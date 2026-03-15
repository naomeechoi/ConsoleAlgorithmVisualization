#include "PlaneTexture.h"

void PlaneTexture::UpdateTexture()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int i = y * width + x;

			double d = texture[i];

			char c;
			int color;
			if (d > 0.35)
			{
				c = '&';
				color = 15;
			}
			else if (d > 0.25)
			{
				c = '#';
				color = 14;
			}
			else if (d > 0.185)
			{
				c = '*';
				color = 11;
			}
			else if (d > 0.10)
			{
				c = '.';
				color = 8;
			}
			else
			{
				c = ' ';
				color = 0;
			}

			finalBuffer[i].Char.AsciiChar = c;
			finalBuffer[i].Attributes = color;
		}
	}

	int astarEndIdx = astarEndPosX + astarEndPosY * width;
	finalBuffer[astarEndIdx].Char.AsciiChar = '!';
	finalBuffer[astarEndIdx].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_GREEN;

	int astarIdx = astarPosX + astarPosY * width;
	finalBuffer[astarIdx].Char.AsciiChar = '@';
	finalBuffer[astarIdx].Attributes = FOREGROUND_RED;
}
