#pragma once

#include <OverEngine.h>
using namespace OverEngine;

Ref<Scene> CreateSceneOnDisk(const String& path);
Ref<Scene> LoadSceneFromFile(const String& path);
void SaveSceneToFile(const String& path, Ref<Scene> scene);