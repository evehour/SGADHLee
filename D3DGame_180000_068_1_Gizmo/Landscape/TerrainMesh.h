#pragma once

class TerrainMesh
{
public:
	TerrainMesh(ExecuteValues* values);
	~TerrainMesh();

	UINT VertexCount() { return vertexCount; }
	void CopyVertices(void* vertices);

private:
	void ReadHeightMap(wstring file);
	void CalcNormalVector();
	void AlignVertexData();

private:
	ExecuteValues* values;

	UINT width, height;

	VertexTextureNormal* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;
};