#ifndef SCENELOADER_INCLUDED
#define SCENELOADER_INCLUDED

#include "Scene.h"
#include <iostream>
#include <fstream>

// Loader class for scenes
class SceneLoader {
public:
	Scene ParseSceneFile(std::string fileName);

private:
	void removeLeading(std::string &s);
};

#endif