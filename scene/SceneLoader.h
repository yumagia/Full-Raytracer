#ifndef SCENELOADER_INCLUDED
#define SCENELOADER_INCLUDED

#include "Scene.h"
#include <string>

// Loader class for scenes
class SceneLoader {
public:
	Scene *ParseSceneFile(const char *fileName);

private:
	void RemoveLeading(std::string &s);
};

#endif