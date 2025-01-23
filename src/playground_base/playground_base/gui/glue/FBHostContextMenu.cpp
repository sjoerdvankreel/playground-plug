#include <playground_base/gui/glue/FBHostContextMenu.hpp>

#include <stack>
#include <memory>
#include <cassert>

using namespace juce;

struct MenuBuilder
{
  bool checked = false;
  bool enabled = false;
  std::string name = {};
  std::unique_ptr<PopupMenu> menu = {};
};

std::unique_ptr<PopupMenu>
FBMakeHostContextMenu(std::vector<FBHostContextMenuItem> const& items)
{
  std::stack<MenuBuilder> builders = {};
  builders.emplace();
  builders.top().menu = std::make_unique<PopupMenu>();
  for (int i = 0; i < items.size(); i++)
    if (items[i].subMenuStart)
    {
      builders.emplace();
      builders.top().name = items[i].name;
      builders.top().checked = items[i].checked;
      builders.top().enabled = items[i].enabled;
      builders.top().menu = std::make_unique<PopupMenu>();
    } else if (items[i].subMenuEnd)
    {
      auto builder = std::move(builders.top());
      builders.pop();
      builders.top().menu->addSubMenu(builder.name, *builder.menu, builder.enabled, nullptr, builder.checked);
    } else if (items[i].separator)
      builders.top().menu->addSeparator();
    else
      builders.top().menu->addItem(i + 1, items[i].name, items[i].enabled, items[i].checked);
  assert(builders.size() == 1);
  return std::move(builders.top().menu);
}