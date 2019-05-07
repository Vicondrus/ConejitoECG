#pragma once
#include "vec3.h"
#include <vector>

namespace egc {
	//Cyrus-Beck clipping algorithm
	//clipWindow specifies the vertices that define the clipping area in conterclockwise order
	//and can represent any convex polygon
	//function returns -1 if the line segment cannot be clipped
	//-2 if the line segment is actually a point and it is outside of the window
	int lineClip_CyrusBeck(std::vector<vec3> clipWindow, vec3& p1, vec3& p2)
	{
		vec3 D = p2 - p1;
		float tLeaving = 1.0f, tEntering = 0.0f;
		vec3 planeNormal = crossProduct(clipWindow[0]-clipWindow[1], clipWindow[2]-clipWindow[1]);
		if (p1 == p2) 
		{
			for (int i = 0; i < clipWindow.size(); i++)
			{
				vec3 normal = crossProduct(p1-clipWindow[i%clipWindow.size()],clipWindow[(i + 1) % clipWindow.size()] - clipWindow[i%clipWindow.size()]);
				if (normal.z > 0) {
					return -2;
				}
			}
			return 1;
		}
		else
		{
			for (int i = 0; i < clipWindow.size(); i++)
			{
				vec3 normal = crossProduct(planeNormal, clipWindow[(i + 1) % clipWindow.size()] - clipWindow[i%clipWindow.size()]);
				float t = dotProduct(normal, (p1 - clipWindow[i])) / dotProduct(-normal, D);
				if (dotProduct(normal, D) != 0)
				{

					if (dotProduct(normal, D) > 0 && t < tLeaving)
					{
						tLeaving = t;
					}
					else if (dotProduct(normal, D) < 0 && t > tEntering)
					{
						tEntering = t;
					}
				}
			}
		}
		if (tEntering > tLeaving)
		{
			return -1;
		}
		else
		{
			vec3 pTe = p1 + (p2 - p1)*tEntering;
			vec3 pTl = p1 + (p2 - p1)*tLeaving;
			p1 = pTe;
			p2 = pTl;
		}
	}

	//Cohen-Sutherland clipping algorithm
	//clipWindow specifies the vertices that define the clipping area in conterclockwise order
	//and must be a rectangular window
	std::vector<int> computeCSCode(std::vector<vec3> clipWindow, const vec3 p)
	{
		vec3 min, max;
		min.x = 99999;
		min.y = 99999;
		max.x = -1;
		max.y = -1;
		for (int i = 0; i < 4; i++) {
			if (clipWindow[i].x > max.x) {
				max.x = clipWindow[i].x;
			}
			else if (clipWindow[i].x < min.x) {
				min.x = clipWindow[i].x;
			}
			if (clipWindow[i].y > max.y) {
				max.y = clipWindow[i].y;
			}
			else if (clipWindow[i].y < min.y) {
				min.y = clipWindow[i].y;
			}
		}
		std::vector<int> codes;
		if (min.y > p.y) 
		{
			codes.push_back(1);
		}
		else
		{
			codes.push_back(0);
		}
		if (max.y < p.y)
		{
			codes.push_back(1);
		}
		else
		{
			codes.push_back(0);
		}
		if (max.x >= p.x)
		{
			codes.push_back(0);
		}
		else
		{
			codes.push_back(1);
		}
		if (min.x <= p.x)
		{
			codes.push_back(0);
		}
		else
		{
			codes.push_back(1);
		}
		return codes;
	}
	bool simpleRejection(std::vector<int> cod1, std::vector<int> cod2)
	{
		for (int i = 0; i < 4; i++)
		{
			if (cod1[i] == 1 && cod2[i] == 1)
			{
				return true;
			}
		}
		return false;
	}
	bool simpleAcceptance(std::vector<int> cod1, std::vector<int> cod2)
	{
		for (int i = 0; i < 4; i++)
		{
			if (cod1[i] == 1 || cod2[i] == 1)
			{
				return false;
			}
		}
		return true;
	}
	//function returns -1 if the line segment cannot be clipped
	int lineClip_CohenSutherland(std::vector<vec3> clipWindow, vec3& p1, vec3& p2)
	{
		vec3 max, min;
		max.x = -1;
		max.y = -1;
		min.x = 99999;
		min.y = 99999;
		for (int i = 0; i < 4; i++)
		{
			if (max.x < clipWindow[i].x)
			{
				max.x = clipWindow[i].x;
			}
			if (min.x > clipWindow[i].x)
			{
				min.x = clipWindow[i].x;
			}
			if (max.y < clipWindow[i].y)
			{
				max.y = clipWindow[i].y;
			}
			if (min.y > clipWindow[i].y)
			{
				min.y = clipWindow[i].y;
			}
		}
		bool finished = false;
		while (!finished)
		{
			std::vector<int> cod1 = computeCSCode(clipWindow, p1);
			std::vector<int> cod2 = computeCSCode(clipWindow, p2);
			bool reject = simpleRejection(cod1, cod2);
			if (reject)
			{
				finished = true;
			}
			else
			{
				bool display = simpleAcceptance(cod1, cod2);
				if (display)
				{
					finished = true;
				}
				else
				{
					bool ok = false;
					for (int i = 0; i < 4; i++)
					{
						if (cod1[i] == 1)
						{
							ok = true;
						}
					}
					if (!ok)
					{
						vec3 aux = p1;
						p1 = p2;
						p2 = aux;
						std::vector<int> auy = cod1;
						cod1 = cod2;
						cod2 = auy;
					}

					if (cod1[1] == 1 && p1.y != p2.y)
					{
						p1.x = p1.x + (p2.x - p1.x)*(max.y - p1.y) / (p2.y - p1.y);
						p1.y = max.y;
					}

					else if (cod1[0] == 1 && p1.y != p2.y)
					{
						p1.x = p1.x + (p2.x - p1.x)*(min.y - p1.y) / (p2.y - p1.y);
						p1.y = min.y;
					}

					else if (cod1[2] == 1 && p1.x != p2.x)
					{
						p1.y = p1.y + (p2.y - p1.y)*(max.x - p1.x) / (p2.x - p1.x);
						p1.x = max.x;
					}

					else if (cod1[3] == 1 && p1.x != p2.x)
					{
						p1.y = p1.y + (p2.y - p1.y)*(min.x - p1.x) / (p2.x - p1.x);
						p1.x = min.x;
					}
				}
			}
		}
		return 1;
	}
}
