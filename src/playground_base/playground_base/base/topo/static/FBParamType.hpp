#pragma once

enum class FBParamType { Boolean, Discrete, List, Note, Bars, Linear, Log2, FreqOct };

bool
FBParamTypeIsList(FBParamType type);
bool
FBParamTypeIsStepped(FBParamType type);