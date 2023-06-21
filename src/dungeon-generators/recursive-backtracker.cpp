#include "recursive-backtracker.h"

#include "rng.h"
#include "constants.h"

#include <stack>
#include <map>

///
/// \brief Everybody's favourite
///
/// 1. Get random direction.
/// 2. Try to replace wall with empty space.
/// 3. Backtrack to previous cell if failed.
/// 4. Repeat until all cells are visited.
///
void RecursiveBacktracker::Generate(const Position& mapSize,
                                    const Position& startingPoint,
                                    const RemovalParams& removalParams)
{
  std::stack<Position> openCells;

  _mapSize = mapSize;
  _endWallsRemovalParams = removalParams;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int sx, sy = 0;

  if (startingPoint.X == -1 || startingPoint.Y == -1)
  {
    sx = RNG::Instance().RandomRange(1, mapSize.X - 1);
    sy = RNG::Instance().RandomRange(1, mapSize.Y - 1);
  }
  else
  {
    sx = startingPoint.X;
    sy = startingPoint.Y;
  }

  _startingPoint.Set(sx, sy);

  _map[sx][sy].Visited = true;
  _map[sx][sy].Image = '.';

  Position p(sx, sy);
  openCells.push(p);

  while (!openCells.empty())
  {
    auto np = openCells.top();
    auto res = GetRandomCell(np);
    if (res.size() != 0)
    {
      openCells.push(res[0]);
    }
    else
    {
      openCells.pop();
    }
  }

  FillDeadEnds();
  CutProblemCorners();
  RemoveEndWalls();

  FillMapRaw();
}

// =============================================================================

std::vector<Position> RecursiveBacktracker::GetRandomCell(const Position& p)
{
  std::vector<Position> res;

  std::map<RoomEdgeEnum, Position> offsetsBySide =
  {
    { RoomEdgeEnum::NORTH, { -1, 0 } },
    { RoomEdgeEnum::SOUTH, { 1, 0 }  },
    { RoomEdgeEnum::EAST,  { 0, 1 }  },
    { RoomEdgeEnum::WEST,  { 0, -1 } }
  };

  std::vector<Position> candidates;

  for (auto& kvp : offsetsBySide)
  {
    Position newPos;
    newPos.X = p.X + kvp.second.X;
    newPos.Y = p.Y + kvp.second.Y;

    if (IsInsideMap(newPos)
     && IsDeadEnd(newPos)
     && !_map[newPos.X][newPos.Y].Visited)
    {
      candidates.push_back(newPos);
    }
  }

  if (candidates.size() != 0)
  {
    int index = RNG::Instance().RandomRange(0, candidates.size());
    Position cp = candidates[index];
    _map[cp.X][cp.Y].Visited = true;
    _map[cp.X][cp.Y].Image = '.';
    res.push_back(cp);
  }

  return res;
}

