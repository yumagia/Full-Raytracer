#ifndef SCENELOADER_INCLUDED
#define SCENELOADER_INCLUDED

#include "Scene.h"


// Loader class for scenes
class SceneLoader {
public:
	Scene *ParseSceneFile(const char *fileName);

private:
	void removeLeading(std::string &s);
};

#endif