#pragma once

class Context
{
public:
	static void Create();
	static void Delete();

public:
	static class Perspective* GetPerspective();
	static class Viewport* GetViewport();
	static class Camera* GetMainCamera();

	static void SetValues(class Shader* shader);

private:
	static class Perspective* perspective;
	static class Viewport* viewport;
	static class Camera* mainCamera;
};