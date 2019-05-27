//
//  main.cpp
//
//  Copyright ? 2016 CGIS. All rights reserved.
//

#include <iostream>
#include "include/SDL.h"

#include "mat4.h"
#include "vec4.h"
#include "transform.h"
#include "camera.h"
#include "projection.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

//define window dimensions
const int WINDOW_WIDTH = 512;
const int WINDOW_HEIGHT = 512;

//The window
SDL_Window *window = NULL;
//The window renderer
SDL_Renderer* renderer = NULL;
SDL_Event currentEvent;

bool quit = false;

int mouseX, mouseY;
float rotationAngle = 0.0f;
float cameraZ = 3.0f;

egc::mat4 viewTransformMatrix;
egc::mat4 cameraMatrix;
egc::mat4 perspectiveMatrix;
egc::mat4 modelMatrix;
egc::Camera myCamera;

const unsigned int VIEWPORT_WIDTH = 400;
const unsigned int VIEWPORT_HEIGHT = 400;

egc::vec2 viewportTopLeftCorner(30, 30);
egc::vec2 viewportDimensions(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

float depthBuffer[VIEWPORT_WIDTH][VIEWPORT_HEIGHT];

bool backFaceCulling = true;
bool displayNormals = false;
bool doNotDepth = false;

void initDepthBuffer()
{
	for (int i = 0; i < VIEWPORT_WIDTH; i++)
		for (int j = 0; j < VIEWPORT_HEIGHT; j++)
			depthBuffer[i][j] = -1;

}

void drawWireframeTriangle(SDL_Renderer *renderer, egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3)
{
	int dy = 2 * viewportTopLeftCorner.y + viewportDimensions.y;

	SDL_RenderDrawLine(renderer, vertex1.x, -(vertex1.y - dy), vertex2.x, -(vertex2.y - dy));
	SDL_RenderDrawLine(renderer, vertex1.x, -(vertex1.y - dy), vertex3.x, -(vertex3.y - dy));
	SDL_RenderDrawLine(renderer, vertex2.x, -(vertex2.y - dy), vertex3.x, -(vertex3.y - dy));
}

float absolute(float x)
{
	if (x >= 0)
		return x;
	else
		return -x;
}

bool clipPointInHomogeneousCoordinate(egc::vec4 vertex)
{
	if (vertex.x<-absolute(vertex.w) || vertex.x>absolute(vertex.w))
		return false;
	if (vertex.y<-absolute(vertex.w) || vertex.y>absolute(vertex.w))
		return false;
	if (vertex.z<-absolute(vertex.w) || vertex.z>absolute(vertex.w))
		return false;
	return true;
}

bool clipTriangleInHomegeneousCoordinates(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3)
{
	if (!clipPointInHomogeneousCoordinate(vertex1) || !clipPointInHomogeneousCoordinate(vertex2) || !clipPointInHomogeneousCoordinate(vertex3))
		return false;
	return true;
}

egc::vec3 findNormalVectorToTriangle(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3)
{
	egc::vec3 n;
	n = egc::crossProduct(vertex2 - vertex1, vertex3 - vertex1).normalize();
	return n;
}

egc::vec4 findCenterPointOfTriangle(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3)
{
	egc::vec4 triangleCenter;
	triangleCenter = (vertex1 + vertex2 + vertex3) *(1 / 3.0f);
	return triangleCenter;
}

bool isTriangleVisible(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3, egc::vec3 normalVector)
{
	egc::vec3 norm = egc::vec3(vertex1);//findNormalVectorToTriangle(vertex1, vertex2, vertex3);
	if (egc::dotProduct(norm, normalVector) >= 0)
		return false;
	return true;
}

void displayNormalVectors(egc::vec3 normalVector, egc::vec4 triangleCenter)
{
	int dy = 2 * viewportTopLeftCorner.y + viewportDimensions.y;
	egc::vec4 normAux;

	normAux.x = normalVector.x;
	normAux.y = normalVector.y;
	normAux.z = normalVector.z;

	egc::vec4 start, end;
	start = triangleCenter;
	float scaleFactor = 0.1;
	end = triangleCenter + normAux*scaleFactor;

	start = perspectiveMatrix * start;
	end = perspectiveMatrix * end;

	egc::perspectiveDivide(start);
	egc::perspectiveDivide(end);

	start = viewTransformMatrix * start;
	end = viewTransformMatrix * end;



	SDL_SetRenderDrawColor(renderer,0,128,0,0);
	SDL_RenderDrawLine(renderer, start.x, -(start.y - dy), end.x, -(end.y - dy));
}

float lineEquation(egc::vec4 vertex1, egc::vec4 vertex2, float x, float y)
{
	return (vertex1.y - vertex2.y)*x + (vertex2.x - vertex1.x)*y + vertex1.x*vertex2.y - vertex2.x*vertex1.y;
}

void triangleRasterization(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3, egc::vec3 colorA, egc::vec3 colorB, egc::vec3 colorC)
{
	egc::vec4 leftUp;
	egc::vec4 rightDown;
	egc::vec3 color;
	leftUp.x = std::min(vertex1.x, std::min(vertex2.x, vertex3.x));
	rightDown.x = std::max(vertex1.x, std::max(vertex2.x, vertex3.x));
	leftUp.y = std::min(vertex1.y, std::min(vertex2.y, vertex3.y));
	rightDown.y = std::max(vertex1.y, std::max(vertex2.y, vertex3.y));
	for (int i = leftUp.x; i <= rightDown.x; i++)
	{
		for (int j = leftUp.y; j <= rightDown.y; j++)
		{
			float a = lineEquation(vertex2, vertex3, i, j) / lineEquation(vertex2, vertex3, vertex1.x, vertex1.y);
			float b = lineEquation(vertex1, vertex3, i, j) / lineEquation(vertex1, vertex3, vertex2.x, vertex2.y);
			float c = 1 - a - b;
			if (a > 0 && a < 1 && b > 0 && b < 1 && c > 0 && c < 1)
			{
				float zPixel = a * vertex1.z + b * vertex2.z + c * vertex3.z;
				if ((zPixel > depthBuffer[i][j]) || doNotDepth)
				{
					depthBuffer[i][j] = zPixel;
					color = colorA * a + colorB * b + colorC * c;
					SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 0);
					int dy = 2 * viewportTopLeftCorner.y + viewportDimensions.y;
					SDL_RenderDrawPoint(renderer, i, -(j - dy));
				}
			}
		}
	}
}

float zMin, zMax;

void findMinMax(std::vector<tinyobj::shape_t> shapes)
{
	zMin = 10.0f;
	zMax = -10.0f;
	for (size_t i = 0; i < shapes.size(); i++) {

		for (int f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
			int vertexId1 = shapes[i].mesh.indices[3 * f + 0];
			int vertexId2 = shapes[i].mesh.indices[3 * f + 1];
			int vertexId3 = shapes[i].mesh.indices[3 * f + 2];

			egc::vec4 tempVertex1 = egc::vec4(shapes[i].mesh.positions[3 * vertexId1 + 0], shapes[i].mesh.positions[3 * vertexId1 + 1], shapes[i].mesh.positions[3 * vertexId1 + 2], 1);
			egc::vec4 tempVertex2 = egc::vec4(shapes[i].mesh.positions[3 * vertexId2 + 0], shapes[i].mesh.positions[3 * vertexId2 + 1], shapes[i].mesh.positions[3 * vertexId2 + 2], 1);
			egc::vec4 tempVertex3 = egc::vec4(shapes[i].mesh.positions[3 * vertexId3 + 0], shapes[i].mesh.positions[3 * vertexId3 + 1], shapes[i].mesh.positions[3 * vertexId3 + 2], 1);

			zMin = std::min(zMin, std::min(tempVertex1.z, std::min(tempVertex2.z, tempVertex3.z)));
			zMax = std::max(zMax, std::max(tempVertex1.z, std::max(tempVertex2.z, tempVertex3.z)));
		}
	}
}

void renderMesh(SDL_Renderer *renderer, std::vector<tinyobj::shape_t> shapes)
{
	egc::vec3 color1, color2, color3;
	float depthCoeff;
	//for each mesh in the 3d model representation
	for (size_t i = 0; i < shapes.size(); i++) {
		//for each triangle

//#pragma omp parallel for  //uncomment for disco bun

		for (int f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
			int vertexId1 = shapes[i].mesh.indices[3 * f + 0];
			int vertexId2 = shapes[i].mesh.indices[3 * f + 1];
			int vertexId3 = shapes[i].mesh.indices[3 * f + 2];

			//update each of the three vertices that form a triangle with the ModelView matrix
			egc::vec4 tempVertex1 = egc::vec4(shapes[i].mesh.positions[3 * vertexId1 + 0], shapes[i].mesh.positions[3 * vertexId1 + 1], shapes[i].mesh.positions[3 * vertexId1 + 2], 1);
			egc::vec4 tempVertex2 = egc::vec4(shapes[i].mesh.positions[3 * vertexId2 + 0], shapes[i].mesh.positions[3 * vertexId2 + 1], shapes[i].mesh.positions[3 * vertexId2 + 2], 1);
			egc::vec4 tempVertex3 = egc::vec4(shapes[i].mesh.positions[3 * vertexId3 + 0], shapes[i].mesh.positions[3 * vertexId3 + 1], shapes[i].mesh.positions[3 * vertexId3 + 2], 1);

			depthCoeff = 255.0*(tempVertex1.z - zMin) / (zMax - zMin);
			color1 = egc::vec3(depthCoeff, depthCoeff, depthCoeff);
			depthCoeff = 255.0*(tempVertex2.z - zMin) / (zMax - zMin);
			color2 = egc::vec3(depthCoeff, depthCoeff, depthCoeff);
			depthCoeff = 255.0*(tempVertex3.z - zMin) / (zMax - zMin);
			color3 = egc::vec3(depthCoeff, depthCoeff, depthCoeff);

			tempVertex1 = modelMatrix * tempVertex1;
			tempVertex2 = modelMatrix * tempVertex2;
			tempVertex3 = modelMatrix * tempVertex3;

			tempVertex1 = cameraMatrix * tempVertex1;
			tempVertex2 = cameraMatrix * tempVertex2;
			tempVertex3 = cameraMatrix * tempVertex3;

			if (backFaceCulling && !isTriangleVisible(tempVertex1, tempVertex2, tempVertex3, findNormalVectorToTriangle(tempVertex1, tempVertex2, tempVertex3)))
				continue;

			egc::vec3 normal = findNormalVectorToTriangle(tempVertex1, tempVertex2, tempVertex3);
			egc::vec4 center = findCenterPointOfTriangle(tempVertex1, tempVertex2, tempVertex3);

			tempVertex1 = perspectiveMatrix * tempVertex1;
			tempVertex2 = perspectiveMatrix * tempVertex2;
			tempVertex3 = perspectiveMatrix * tempVertex3;

			if (!clipTriangleInHomegeneousCoordinates(tempVertex1, tempVertex2, tempVertex3))
				continue;

			if (displayNormals)
				displayNormalVectors(normal, center);

			egc::perspectiveDivide(tempVertex1);
			tempVertex1 = viewTransformMatrix * tempVertex1;
			
			egc::perspectiveDivide(tempVertex2);
			tempVertex2 = viewTransformMatrix * tempVertex2;
			
			egc::perspectiveDivide(tempVertex3);
			tempVertex3 = viewTransformMatrix * tempVertex3;

			SDL_SetRenderDrawColor(renderer, 96, 96, 96, 0);
			//drawWireframeTriangle(renderer, tempVertex1, tempVertex2, tempVertex3);
			triangleRasterization(tempVertex1, tempVertex2, tempVertex3, color1, color2, color3);
		}
	}
}

std::vector<tinyobj::shape_t> readOBJ()
{
	std::string inputfile = "bunny.obj";
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

#ifdef PRINTINFO
	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	for (size_t i = 0; i < shapes.size(); i++) {
		printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
		printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
		printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());

		assert((shapes[i].mesh.indices.size() % 3) == 0);
		for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
			printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3 * f + 0], shapes[i].mesh.indices[3 * f + 1], shapes[i].mesh.indices[3 * f + 2], shapes[i].mesh.material_ids[f]);
		}

		printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
		assert((shapes[i].mesh.positions.size() % 3) == 0);
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			printf("  v[%ld] = (%f, %f, %f)\n", v,
				shapes[i].mesh.positions[3 * v + 0],
				shapes[i].mesh.positions[3 * v + 1],
				shapes[i].mesh.positions[3 * v + 2]);
		}
	}
#endif
	return shapes;
}

bool initWindow()
{
	bool success = true;

	//Try to initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL initialization failed" << std::endl;
		success = false;
	}
	else {
		//Try to create the window
		window = SDL_CreateWindow("SDL Hello World Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

		if (window == NULL)
		{
			std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
			success = false;
		}
		else
		{
			//Create renderer for window
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
			if (renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}

	return success;
}

void destroyWindow()
{
	//Destroy window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	//Quit SDL
	SDL_Quit();
}

void handleMouseEvents()
{
	//Mouse event -> pressed button
	if (currentEvent.type == SDL_MOUSEBUTTONDOWN)
	{
		if (currentEvent.button.button == SDL_BUTTON_LEFT)
		{
			SDL_GetMouseState(&mouseX, &mouseY);

		}

		if (currentEvent.button.button == SDL_BUTTON_RIGHT)
		{
			SDL_GetMouseState(&mouseX, &mouseY);

		}
	}
	/*
	//Mouse event -> mouse movement
	if (currentEvent.type == SDL_MOUSEMOTION)
	{
		if (currentEvent.button.button & SDL_BUTTON_LMASK)
		{
			SDL_GetMouseState(&mouseX, &mouseY);
		}

		if (currentEvent.button.button & SDL_BUTTON_RMASK)
		{
			SDL_GetMouseState(&mouseX, &mouseY);

		}
	}
	 */
}

void handleKeyboardEvents()
{
	//Keyboard event
	if (currentEvent.type == SDL_KEYDOWN)
	{
		switch (currentEvent.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			quit = true;
			break;

		case SDLK_a:
			rotationAngle += 10.0f;
			break;

		case SDLK_d:
			rotationAngle -= 10.0f;
			break;

		case SDLK_w:
			cameraZ -= 0.5f;
			break;

		case SDLK_s:
			cameraZ += 0.5f;
			break;

		case SDLK_z:
			backFaceCulling = true;
			break;

		case SDLK_c:
			backFaceCulling = false;
			break;

		case SDLK_q:
			displayNormals = true;
			break;

		case SDLK_e:
			displayNormals = false;
			break;

		case SDLK_f:
			doNotDepth = false;
			break;

		case SDLK_r:
			doNotDepth = true;
			break;

		default:
			break;
		}
	}
}

int main(int argc, char * argv[]) {
	if (!initWindow())
	{
		std::cout << "Failed to initialize" << std::endl;
		return -1;
	}

	myCamera = egc::Camera(egc::vec3(-0.3f, 1.5f, cameraZ), egc::vec3(-0.3f, 1.5f, -10.0f), egc::vec3(0.0f, 1.0f, 0.0f));
	viewTransformMatrix = egc::defineViewTransformMatrix(viewportTopLeftCorner.x, viewportTopLeftCorner.y, viewportDimensions.x, viewportDimensions.y);
	cameraMatrix = egc::defineCameraMatrix(myCamera);
	perspectiveMatrix = egc::definePerspectiveProjectionMatrix(45.0f, 1.0, -0.1f, -10.0f);

	std::vector<tinyobj::shape_t> shapes = readOBJ();
	
	findMinMax(shapes);

	SDL_Rect viewportRectangle = { static_cast<int>(viewportTopLeftCorner.x), static_cast<int>(viewportTopLeftCorner.y), static_cast<int>(viewportDimensions.x), static_cast<int>(viewportDimensions.y) };

	while (!quit) {
		//Handle events on queue
		while (SDL_PollEvent(&currentEvent) != 0)
		{
			//User requests quit
			if (currentEvent.type == SDL_QUIT)
			{
				quit = true;
			}

			handleMouseEvents();
			handleKeyboardEvents();

			//Clear screen
			initDepthBuffer();

			SDL_SetRenderDrawColor(renderer, 224, 224, 224, 0);
			SDL_RenderClear(renderer);

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
			SDL_RenderDrawRect(renderer, &viewportRectangle);

			modelMatrix = egc::rotateY(rotationAngle) * egc::scale(15.0f, 15.0f, 15.0f);
			myCamera.cameraPosition.z = cameraZ;
			cameraMatrix = egc::defineCameraMatrix(myCamera);

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
			renderMesh(renderer, shapes);

			//Update screen
			SDL_RenderPresent(renderer);
		}
	}

	destroyWindow();
	return 0;
}
