#include "Mesh.h"

namespace RenderEngine
{
	void Mesh::SetTexture(const std::vector<double>& texture)
	{
		this->texture = texture;
	}

	void Mesh::UpdateTexture()
	{
	}

	std::vector<CHAR_INFO> Mesh::GetFinalBuffer()
	{
		return finalBuffer;
	}
}