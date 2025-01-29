#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <cassert>

using namespace juce;

bool
FBGridCell::operator<(FBGridCell const& rhs) const
{
  if (row < rhs.row) return true;
  if (row > rhs.row) return false;
  return col < rhs.col;
}

FBGridComponent::
FBGridComponent(int rows, std::vector<int> const& cols):
FBGridComponent(std::vector<int>(rows, 1), cols) {}

FBGridComponent::
FBGridComponent(std::vector<int> const& rows, int cols):
FBGridComponent(rows, std::vector<int>(cols, 1)) {}

FBGridComponent::
FBGridComponent(int rows, int cols):
FBGridComponent(std::vector<int>(rows, 1), std::vector<int>(cols, 1)) {}

FBGridComponent::
FBGridComponent(std::vector<int> const& rows, std::vector<int> const& cols):
Component(),
_rows(rows),
_cols(cols),
_cells() {}

void
FBGridComponent::Add(int row, int col, Component* child)
{
  Add(row, col, 1, 1, child);
}

void
FBGridComponent::Add(int row, int col, int rowSpan, int colSpan, Component* child)
{
  addAndMakeVisible(child);
  FBGridCell cell = { row, col };
  assert(_cells.find(cell) == _cells.end());
  _cells[cell].child = child;
  _cells[cell].span = { rowSpan, colSpan };
}

int
FBGridComponent::FixedWidth(int height) const
{
  // TODO all rows
  int result = 0;
  for (int i = 0; i < _cols.size(); i++)
    result += HorizontalAutoSizeAt(0, i)->FixedWidth(height);
  return result;
}

IFBHorizontalAutoSize*
FBGridComponent::HorizontalAutoSizeAt(int row, int col) const
{
  auto component = _cells.at({ row, col }).child;
  return FBAsHorizontalAutoSize(component);
}

int 
FBGridComponent::FixedColWidth(int col, int height) const
{
  int result = 0;
  int totalRelativeHeight = 0;
  for (int i = 0; i < _rows.size(); i++)
    totalRelativeHeight += _rows[i];
  for (int r = 0; r < _rows.size(); r++)
  {
    int rowHeight = (int)std::round(_rows[r] / (float)totalRelativeHeight * height);
    int fixedWidth = HorizontalAutoSizeAt(r, col)->FixedWidth(rowHeight);
    result = std::max(result, fixedWidth);
  }
  return result;
}

void
FBGridComponent::resized()
{
  Grid grid;
  for (auto const& e : _cells)
  {
    GridItem item(e.second.child);
    item.row.start = e.first.row + 1;
    item.column.start = e.first.col + 1;
    item.row.end = e.first.row + e.second.span.row + 1;
    item.column.end = e.first.col + e.second.span.col + 1;
    grid.items.add(item);
  }

  for (int i = 0; i < _rows.size(); i++)
    grid.templateRows.add(Grid::TrackInfo(Grid::Fr(_rows[i])));
  for (int i = 0; i < _cols.size(); i++)
    if (_cols[i] != 0)
      grid.templateColumns.add(Grid::TrackInfo(Grid::Fr(_cols[i])));
    else
      grid.templateColumns.add(Grid::TrackInfo(Grid::Px(FixedColWidth(i, getHeight()))));

  grid.performLayout(getLocalBounds());
}