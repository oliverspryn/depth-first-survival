#include "Maze.h"

Maze::Maze() {}

Maze::~Maze() {
	delete root;
}

void Maze::addWalls(Node *cell) {
//Place the east wall
	if (cell->children.east == NULL && cell->location.x >= 0 && cell->location.x < size.x - 1) {
		int xEast = 2 * (cell->location.x + 1) * mazeNS::CELL_LENGTH;
		int zEast = 2 * (mazeNS::CELL_WIDTH / 2.0f + (cell->location.z * mazeNS::CELL_WIDTH));

		walls[wallsConstructed].setPosition(D3DXVECTOR3(xEast, 0, zEast));
		++wallsConstructed;
	}
	
//Place the south wall
	if (cell->children.south == NULL && cell->location.z > 0 && cell->location.z < size.z) {
		int xSouth = 2 * (mazeNS::CELL_LENGTH / 2.0f + (cell->location.x * mazeNS::CELL_LENGTH));
		int zSouth = 2 * cell->location.z * mazeNS::CELL_WIDTH;

		walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_LENGTH));
		walls[wallsConstructed].setRotation(D3DXVECTOR3(0, ToRadian(90), 0));
		walls[wallsConstructed].setPosition(D3DXVECTOR3(xSouth, 0, zSouth));
		++wallsConstructed;
	}
}

void Maze::build() {
	Node *currentCell = root;
	Node *lastCell = NULL;
	Node *nextCell = NULL;

	#ifdef DEBUG_ENABLED
	//Debug output
		wchar_t visit[10], xCur[10], zCur[10], xNext[10], zNext[10];
		_itow_s(currentCell->location.x, xCur, 10);
		_itow_s(currentCell->location.z, zCur, 10);
	
		OutputDebugString(L"Start: (");
		OutputDebugString(xCur);
		OutputDebugString(L", ");
		OutputDebugString(zCur);
		OutputDebugString(L")\n");
		OutputDebugString(L"Visited: 1 cell\n");
	#endif

	do {
		nextCell = inaccessableSiblingCell(currentCell);

		if (nextCell != NULL) {
			#ifdef DEBUG_ENABLED
			//Debug ouputput
				_itow_s(currentCell->location.x, xCur, 10);
				_itow_s(currentCell->location.z, zCur, 10);
				_itow_s(nextCell->location.x, xNext, 10);
				_itow_s(nextCell->location.z, zNext, 10);

				OutputDebugString(L"Advance: (");
				OutputDebugString(xCur);
				OutputDebugString(L", ");
				OutputDebugString(zCur);
				OutputDebugString(L") binding ");
			#endif

		//Assume this is part of the solution to the maze
			currentCell->toEnd = true;

		//What is the direction of this new cell?
			if (currentCell->location.x > nextCell->location.x) { // WEST
				#ifdef DEBUG_ENABLED
					OutputDebugString(L"WEST");
				#endif

				currentCell->children.west = nextCell;
				nextCell->children.east = currentCell;
			}

			if (currentCell->location.x < nextCell->location.x) { // EAST
				#ifdef DEBUG_ENABLED
					OutputDebugString(L"EAST");
				#endif

				currentCell->children.east = nextCell;
				nextCell->children.west = currentCell;
			}

			if (currentCell->location.z > nextCell->location.z) { // SOUTH
				#ifdef DEBUG_ENABLED
					OutputDebugString(L"SOUTH");
				#endif

				currentCell->children.south = nextCell;
				nextCell->children.north = currentCell;
			}

			if (currentCell->location.z < nextCell->location.z) { // NORTH
				#ifdef DEBUG_ENABLED
					OutputDebugString(L"NORTH");
				#endif

				currentCell->children.north = nextCell;
				nextCell->children.south = currentCell;
			}
			
		//Log this transaction
			locations.push(currentCell);
			currentCell = nextCell;
			lastCell = currentCell;
			++visited;

			#ifdef DEBUG_ENABLED
			//Debug output
				_itow_s(visited, visit, 10);
			
				OutputDebugString(L" with (");
				OutputDebugString(xNext);
				OutputDebugString(L", ");
				OutputDebugString(zNext);
				OutputDebugString(L")\n");
				OutputDebugString(L"Visited: ");
				OutputDebugString(visit);
				OutputDebugString(L" cells\n");
			#endif
		} else {
			#ifdef DEBUG_ENABLED
			//Debug ouputput
				_itow_s(currentCell->location.x, xCur, 10);
				_itow_s(currentCell->location.z, zCur, 10);

				OutputDebugString(L"   Retreat: (");
				OutputDebugString(xCur);
				OutputDebugString(L", ");
				OutputDebugString(zCur);
				OutputDebugString(L") falling back to ");
			#endif

		//Since we are backtracking, this is not part of the solution
			currentCell->toEnd = false;

		//Go back one step
			currentCell = locations.top();
			locations.pop();

			#ifdef DEBUG_ENABLED
			//Debug output
				_itow_s(currentCell->location.x, xCur, 10);
				_itow_s(currentCell->location.z, zCur, 10);

				OutputDebugString(L"(");
				OutputDebugString(xCur);
				OutputDebugString(L", ");
				OutputDebugString(zCur);
				OutputDebugString(L")\n");
			#endif
		}
	} while (visited < totalCells);

//The last cell is the solution
	lastCell->toEnd = true;

//Save the end cell
	end = lastCell->location;

	#ifdef DEBUG_ENABLED
	//Debug output
		_itow_s(lastCell->location.x, xCur, 10);
		_itow_s(lastCell->location.z, zCur, 10);

		OutputDebugString(L"End: (");
		OutputDebugString(xCur);
		OutputDebugString(L", ");
		OutputDebugString(zCur);
		OutputDebugString(L")\n\n");

	//Display all the connections!
		OutputDebugString(L"Connected edges:\n");

		for(int i = 0; i < size.x; ++i) {
			for(int j = 0; j < size.z; ++j) {
				_itow_s(i, xCur, 10);
				_itow_s(j, zCur, 10);

				OutputDebugString(L"(");
				OutputDebugString(xCur);
				OutputDebugString(L", ");
				OutputDebugString(zCur);
				OutputDebugString(L") is connected with: ");

			//East connection
				if (grid[i][j]->children.east != NULL) {
					_itow_s(i + 1, xNext, 10);
					_itow_s(j, zNext, 10);

					OutputDebugString(L"[East] (");
					OutputDebugString(xNext);
					OutputDebugString(L", ");
					OutputDebugString(zNext);
					OutputDebugString(L") ");
				}

			//West connection
				if (grid[i][j]->children.west != NULL) {
					_itow_s(i - 1, xNext, 10);
					_itow_s(j, zNext, 10);

					OutputDebugString(L"[West] (");
					OutputDebugString(xNext);
					OutputDebugString(L", ");
					OutputDebugString(zNext);
					OutputDebugString(L") ");
				}

			//North connection
				if (grid[i][j]->children.north != NULL) {
					_itow_s(i, xNext, 10);
					_itow_s(j + 1, zNext, 10);

					OutputDebugString(L"[North] (");
					OutputDebugString(xNext);
					OutputDebugString(L", ");
					OutputDebugString(zNext);
					OutputDebugString(L") ");
				}

			//South connection
				if (grid[i][j]->children.south != NULL) {
					_itow_s(i, xNext, 10);
					_itow_s(j - 1, zNext, 10);

					OutputDebugString(L"[South] (");
					OutputDebugString(xNext);
					OutputDebugString(L", ");
					OutputDebugString(zNext);
					OutputDebugString(L") ");
				}

				OutputDebugString(L"\n");
			}
		}
	#endif

//Position the walls
	for(int i = 0; i < size.x; ++i) {
		for(int j = 0; j < size.z; ++j) {
			addWalls(grid[i][j]);
		}
	}

//Add the north bounding wall
	for(int i = 0; i < size.z; i++)
	{
		walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_LENGTH));
		walls[wallsConstructed].setRotation(D3DXVECTOR3(0, ToRadian(90), 0));
		walls[wallsConstructed].setPosition(D3DXVECTOR3((i*mazeNS::CELL_WIDTH)+mazeNS::CELL_LENGTH * size.x, 0, 2 * mazeNS::CELL_WIDTH * size.z));
		++wallsConstructed;
	}

//Add the east bounding wall
	walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_WIDTH * size.z));
	walls[wallsConstructed].setPosition(D3DXVECTOR3(0, 0, mazeNS::CELL_WIDTH * size.z));
	++wallsConstructed;

//Add the south bounding wall
	walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_LENGTH * size.x));
	walls[wallsConstructed].setRotation(D3DXVECTOR3(0, ToRadian(90), 0));
	walls[wallsConstructed].setPosition(D3DXVECTOR3(mazeNS::CELL_LENGTH * size.x, 0, 0));
	++wallsConstructed;

//Add the west bounding wall
	walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_WIDTH * size.z));
	walls[wallsConstructed].setPosition(D3DXVECTOR3(2 * mazeNS::CELL_LENGTH * size.x, 0, mazeNS::CELL_WIDTH * size.z));
	++wallsConstructed;

//Add the floor
	walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::CELL_LENGTH * size.x, mazeNS::CELL_WIDTH * size.z));
	walls[wallsConstructed].setRotation(D3DXVECTOR3(0, 0, ToRadian(90)));
	walls[wallsConstructed].setPosition(D3DXVECTOR3(mazeNS::CELL_LENGTH * size.x, -mazeNS::WALL_HEIGHT, mazeNS::CELL_WIDTH * size.z));
	++wallsConstructed;

//Add the ceiling
	//walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::CELL_LENGTH * size.x, mazeNS::CELL_WIDTH * size.z));
	//walls[wallsConstructed].setRotation(D3DXVECTOR3(0, 0, ToRadian(90)));
	//walls[wallsConstructed].setPosition(D3DXVECTOR3(mazeNS::CELL_LENGTH * size.x, mazeNS::WALL_HEIGHT, mazeNS::CELL_WIDTH * size.z));
	//++wallsConstructed;
}

Location Maze::cellToPx(Location cell) {
//Ensure this request is not out of bounds
	if (cell.x >= size.x || cell.z >= size.z) {
		Location loc = {-1, -1};
		return loc;
	}

//Calculate the grid location
	Location loc;
	loc.x = (mazeNS::CELL_LENGTH / 2.0f) + (cell.x * mazeNS::CELL_LENGTH);
	loc.z = (mazeNS::CELL_WIDTH / 2.0f) + (cell.z * mazeNS::CELL_WIDTH);
	return loc;
}

bool Maze::collided(Location px) {
//Get the cell grid location
	Location cell = pxToCell(px);

//Get the cell center
	Location center = cellToPx(cell);

//Is this location valid?
	if (cell.x == -1) return true;

//Get the cell data
	Borders borders;
	Node *location = grid[cell.x][cell.z];

//North wall boundaries
	if (location->children.north == NULL) {
		borders.north->farDim = center.z + (mazeNS::WALL_THICK / 2.0f + mazeNS::CELL_WIDTH / 2.0f);
		borders.north->nearDim = center.z + (mazeNS::CELL_WIDTH / 2.0f - mazeNS::CELL_PAD);

		if (px.z <= borders.north->farDim & px.z >= borders.north->nearDim) return true;
	}

//East wall boundaries
	if (location->children.east == NULL) {
		borders.north->farDim = center.x + (mazeNS::WALL_THICK / 2.0f + mazeNS::CELL_LENGTH / 2.0f);
		borders.north->nearDim = center.x + (mazeNS::CELL_LENGTH / 2.0f - mazeNS::CELL_PAD);

		if (px.x <= borders.north->farDim & px.x >= borders.north->nearDim) return true;
	}

//South wall boundaries
	if (location->children.south == NULL) {
		borders.south->farDim = center.z - (mazeNS::WALL_THICK / 2.0f + mazeNS::CELL_WIDTH / 2.0f);
		borders.south->nearDim = center.z - (mazeNS::CELL_WIDTH / 2.0f - mazeNS::CELL_PAD);

		if (px.z >= borders.north->farDim & px.z <= borders.north->nearDim) return true;
	}

//West wall boundaries
	if (location->children.east == NULL) {
		borders.north->farDim = center.x - (mazeNS::WALL_THICK / 2.0f + mazeNS::CELL_LENGTH / 2.0f);
		borders.north->nearDim = center.x - (mazeNS::CELL_LENGTH / 2.0f - mazeNS::CELL_PAD);

		if (px.x >= borders.north->farDim & px.x <= borders.north->nearDim) return true;
	}

	return false;
}

void Maze::draw(ID3D10EffectTechnique *technique, D3DXMATRIX viewMTX, D3DXMATRIX projMTX) {
	D3DXMATRIX worldViewProj;
	D3DXMatrixIdentity(&worldViewProj);

	for(int i = 0; i < wallsConstructed; ++i) {
		walls[i].draw(viewMTX, projMTX, technique);
	}
}

Location Maze::getEndPosition() {
	return end;
}

Location Maze::getStartPosition() {
	return start;
}

Node *Maze::inaccessableSiblingCell(Node *currentCell) {
//Choose a random direction
	Direction direction = static_cast<Direction>(rand() % 4);

//Find a cell with no knocked down walls
	Location loc;
	Node *operable = NULL;

	for(int i = 0; i <= 4; ++i) {
		direction = static_cast<Direction>((direction + 1) % 4);

	//Map the chosen direction to a node and stay within the matrix bounds
		switch(direction) {
			case NORTH : 
				loc.x = currentCell->location.x;
				loc.z = currentCell->location.z - 1;

				if (loc.z >= 0) {
					operable = grid[loc.x][loc.z];
				}

				break;

			case EAST : 
				loc.x = currentCell->location.x + 1;
				loc.z = currentCell->location.z;
				
				if (loc.x < size.x) {
					operable = grid[loc.x][loc.z];
				}
				
				break;

			case SOUTH : 
				loc.x = currentCell->location.x;
				loc.z = currentCell->location.z + 1;

				if (loc.z < size.z) {
					operable = grid[loc.x][loc.z];
				}
				
				break;

			case WEST : 
				loc.x = currentCell->location.x - 1;
				loc.z = currentCell->location.z;
				
				if (loc.x >= 0) {
					operable = grid[loc.x][loc.z];
				}
				
				break;
		}

	//Are all the walls intact?
		if (operable != NULL &&
			operable->children.north == NULL &&
			operable->children.east  == NULL &&
			operable->children.south == NULL &&
			operable->children.west  == NULL) {
				return operable;
		} else {
			operable = NULL;
		}
	}

	return NULL;
}

Location Maze::pxToCell(Location px) {
	Dimension dim;
	dim.x = size.x * mazeNS::CELL_LENGTH;
	dim.z = size.z * mazeNS::CELL_WIDTH;

//Ensure this request is not out of bounds
	if (px.x >= dim.x || px.z >= dim.z) {
		Location loc = {-1, -1};
		return loc;
	}

//Calculate the cell position
	Location loc;
	loc.x = floor((double)((px.x * size.x) / dim.x));
	loc.z = floor((double)((px.z * size.z) / dim.z));

	return loc;
}

void Maze::setStartPosition(Location location) {
	start = location;
	root = grid[location.x][location.z];
}

void Maze::setTex(ID3D10EffectShaderResourceVariable* diffuseLoc, ID3D10EffectShaderResourceVariable* specLoc, wchar_t* diffuseMap, wchar_t* specMap)
{
	for(int i = 0; i < wallsConstructed; i++)
	{
		walls[i].setTex(diffuseLoc,specLoc,diffuseMap,specMap);
	}
}

void Maze::init(Dimension dim, ID3D10EffectMatrixVariable *mfxWVPVar, ID3D10EffectMatrixVariable* fx2,ID3D10Device *md3dDevice)
{
	//Set up the random generator
	srand(time(NULL));

//Set up the maze size and DFS algorithm
	size = dim;
	totalCells = dim.x * dim.z;
	visited = 1;

//Create the matrix
	Location loc;

	for(int i = 0; i < dim.x; ++i) {
		vector<Node*> row;

		for(int j = 0; j < dim.z; ++j) {
			loc.x = i;
			loc.z = j;
			
			row.push_back(new Node(loc));
		}

		grid.push_back(row);
	}

//Starting position
	loc.x = rand() % dim.x;
	loc.z = rand() % dim.z;
	
	root = grid[loc.x][loc.z];

//Determine the number of walls needed for the maze
	wallCount = (2 * dim.x * dim.z) - dim.x - dim.z; //Number of inner walls, 2xy - x - y
	walls.resize(wallCount);
	wallsConstructed = 0;
	
	#ifdef DEBUG_ENABLED
	//Debug output
		wchar_t buffer[10];
		_itow_s(wallCount, buffer, 10);

		OutputDebugString(L"\n*******************************************\n");
		OutputDebugString(L"MAZE CLASS OUTPUT\n");
		OutputDebugString(L"*******************************************\n");
		OutputDebugString(L"Generated: ");
		OutputDebugString(buffer);
		OutputDebugString(L" walls\n\n");
	#endif

//Initialize the box object for the walls
	box.init(md3dDevice, 1.0f);

	for(int i = 0; i < wallCount; ++i) {
		walls[i].init(&box, mfxWVPVar,fx2, sqrt(2.0f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 0), 0,
				D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_WIDTH));
	}
}

void Maze::update(float dt) {
	for(int i = 0; i < wallsConstructed; ++i) {
		walls[i].update(dt);
	}
}
