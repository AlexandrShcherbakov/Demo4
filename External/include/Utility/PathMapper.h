#ifndef PATHMAPPER_H
#define PATHMAPPER_H

#include <sstream>
#include <string>

#ifndef UTILITY_H_INCLUDED
#include "Overall.h"
#endif // UTILITY_H_INCLUDED

class PathMapper
{
    public:
        void Init(const std::string& sceneName, const uint voxelsConst) {
            SetSceneName(sceneName);
            SetVoxelsConst(voxelsConst);
        }

        ///Getters
        static PathMapper& GetPM() {
            static PathMapper mapper;
            return mapper;
        }
        const std::string GetResource(const std::string& resourceName, const std::string& type="bin") const {
            std::stringstream ss;
            ss << "../Scenes/" << SceneName << "/" << resourceName << VoxelsConst << "." << type;
            return ss.str();
        }
        const std::string GetHydraScene() const {
            std::stringstream ss;
            ss << "../Scenes/" << SceneName << "/sponza_exported/scene.vsgf";
            return ss.str();
        }
        const std::string GetHydraProfile() const {
            std::stringstream ss;
            ss << "../Scenes/" << SceneName << "/sponza_exported/hydra_profile_generated.xml";
            return ss.str();
        }
        const std::string GetSceneName() const {
            return SceneName;
        }
        const uint GetVoxelsConst() const {
            return VoxelsConst;
        }

        ///Setters
        void SetSceneName(const std::string& sceneName) {
            SceneName = sceneName;
        }
        void SetVoxelsConst(const uint voxelsConst) {
            VoxelsConst = voxelsConst;
        }

    private:
        PathMapper(){}

        std::string SceneName;
        uint VoxelsConst;
};

#endif // PATHMAPPER_H
