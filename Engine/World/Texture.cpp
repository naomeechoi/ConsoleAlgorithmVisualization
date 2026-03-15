#include "Texture.h"

namespace RenderEngine
{
	void Texture::SetTexture(const std::vector<double>& texture)
	{
		this->texture = texture;
	}

	void Texture::UpdateTexture()
	{
	}

	std::vector<CHAR_INFO> Texture::GetFinalBuffer()
	{
		return finalBuffer;
	}

	void Texture::SetAStarPos(int x, int y, int endX, int endY)
	{
		astarPosX = x;
		astarPosY = y;
		astarEndPosX = endX;
		astarEndPosY = endY;
	}
}