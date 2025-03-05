#include <playground_base/base/topo/param/FBBarsParam.hpp>

#include <set>
#include <cassert>

using namespace juce;

bool 
FBBarsParamNonRealTime::IsItems() const
{
  return true;
}

bool 
FBBarsParamNonRealTime::IsStepped() const 
{
  return true;
}

int
FBBarsParamNonRealTime::ValueCount() const 
{
  return static_cast<int>(items.size());
}

double 
FBBarsParamNonRealTime::PlainToNormalized(double plain) const 
{
  return std::clamp(plain / (ValueCount() - 1.0), 0.0, 1.0);
}

double
FBBarsParamNonRealTime::NormalizedToPlain(double normalized) const 
{
  return std::clamp(static_cast<int>(normalized * ValueCount()), 0, ValueCount() - 1);
}

std::string
FBBarsParamNonRealTime::PlainToText(FBTextDisplay display, double plain) const 
{
  int discrete = static_cast<int>(std::round(plain));
  return items[discrete].ToString();
}

std::optional<double>
FBBarsParamNonRealTime::TextToPlain(FBTextDisplay display, std::string const& text) const
{
  for (int i = 0; i < items.size(); i++)
    if (text == items[i].ToString())
      return { i };
  return {};
}

PopupMenu
FBBarsParamNonRealTime::MakePopupMenu() const
{
  int k = 0;
  PopupMenu result;
  PopupMenu subMenu;
  for (int i = 0; i < items.size(); i++)
  {
    subMenu.addItem(i + 1, PlainToText(FBTextDisplay::Text, i));
    if (i == items.size() - 1 || items[i].num != items[i + 1].num)
    {
      result.addSubMenu(std::to_string(items[i].num), subMenu);
      subMenu = {};
    }
  }
  return result;
}

std::vector<FBBarsItem>
FBMakeBarsItems(bool withZero, FBBarsItem min, FBBarsItem max)
{
  static std::vector<int> const
    BarSteps({
      1, 2, 3, 4, 5, 6, 7, 8,
      9, 10, 11, 12, 13, 14, 15, 16,
      20, 24, 32, 48, 64, 96, 128 });

  std::set<FBBarsItem> equals;
  std::vector<FBBarsItem> result;

  if (withZero)
    result.push_back({ 0, -1 });
  for (int i = 0; i < BarSteps.size(); i++)
    for (int j = 0; j < BarSteps.size(); j++)
    {
      FBBarsItem item = { BarSteps[i], BarSteps[j] };
      if (min <= item && item <= max)
        if(equals.insert(item).second)
          result.push_back(item);
    }
  return result;
}