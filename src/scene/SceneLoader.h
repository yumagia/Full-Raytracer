#ifndef SCENELOADER_INCLUDED
#define SCENELOADER_INCLUDED

#include "Scene.h"
#include <string>

#define MAX_ARGS 15

// Loader class for scenes
class SceneLoader {
public:
	Scene *ParseSceneFile(const char *fileName);

private:
	std::vector<std::string> ParseArgsFromLine(std::string line);
	void RemoveLeading(std::string &s);
};

#endif