#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#ifdef TTSLLibBuild
#define TTSLModExport __declspec(dllimport)
#else
#define TTSLModExport __declspec(dllexport)
#endif

#include "pch.h"
#include "Configuration.h"
#include <memory>
#include <string>


/// <summary>
/// Base class for a user defined mod.
/// </summary>
class Mod {
private:
  std::shared_ptr<Configuration> config;

public:
  virtual ~Mod() = default;

  virtual void configureEnvironment(ImGuiContext *context) {
    ImGui::SetCurrentContext(context);
  }

  /// <summary>
  /// Return the lazily loaded configuration for this mod, loaded from a file named <modName>.ini.
  /// </summary>
  std::shared_ptr<Configuration> getConfiguration() {
    if (config) {
      return config;
    } else {
      config = Configuration::getByName(getName() + ".ini", getDefaultConfiguration());
      return config;
    }
  }

  /// <summary>
  /// Return the version this mod was created with.
  /// </summary>
  virtual int getModVersion() { return MOD_VERSION; }

  /// <summary>
  /// Returns the name of this mod. Override to set your mod name.
  /// </summary>
  virtual std::string getName() = 0;

  /// <summary>
  /// Default configuration for this mod.
  /// </summary>
  virtual Configuration::ConfigurationData getDefaultConfiguration() {
    return {};
  }

  /// <summary>
  /// Called as early as possible in the initialization procedure, before any
  /// window is loaded.
  /// </summary>
  virtual void earlyInit() {}

  /// <summary>
  /// Called right before the window is created.
  /// </summary>
  virtual void preWindowInit() {}

  /// <summary>
  /// Called right before exiting the initialization procedure.
  /// This is after all of the global game files have been loaded and right
  /// before control is handed to the user in the main menu
  /// </summary>
  virtual void lateInit() {}

  /// <summary>
  /// Called at the start of the update tick.
  /// </summary>
  /// <param name="delta">Time (in seconds) since last update</param>
  virtual void earlyUpdate(double delta) {}

  /// <summary>
  /// Called at the start of the render loop.
  /// Any modification of rendered engine objects should occur here
  /// </summary>
  virtual void earlyRender() {}

  /// <summary>
  /// Called at the end of the render loop.
  /// Any custom DirectX render calls should occur here
  /// </summary>
  virtual void lateRender() {}

  /// <summary>
  /// Renders the ImGUI UI
  /// </summary>
  virtual void drawUI() {}

  /// <summary>
  /// Parse keyboard input.
  /// </summary>
  virtual void onKeyboardInput(int message, int keyCode) {}
};

extern "C" {
TTSLModExport Mod *getModInstance();
}
