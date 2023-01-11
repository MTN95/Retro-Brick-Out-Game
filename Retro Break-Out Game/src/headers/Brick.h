#pragma once


class aBrick
{
public:
	Vector2 position = { 0 };
	Vector2 size = { 0 };
	Color color = RED;
	bool isAlive = false;
	
	static Vector2 CalculateSpacing(Vector2 brickSize) 
	{
		// Set the x and y spacing based on the size of the brick
		return { 5 + brickSize.x, 5 + brickSize.y };
	}
};