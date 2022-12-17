#pragma once

#ifdef TTSLLibBuild
#define TTSLModExport __declspec(dllimport)
#else
#define TTSLModExport __declspec(dllexport)
#endif
#include <memory>
#include <string>

constexpr int MOD_VERSION = 0;
/// <summary>
/// Base class for a user defined mod.
/// </summary>
class BaseMod {
public:
  virtual ~BaseMod() = default;

  /// <summary>
  /// Returns the name of this mod. Override to set your mod name.
  /// </summary>
  virtual std::string getName() = 0;

  virtual int getModVersion() { return MOD_VERSION; }

  /// <summary>
  /// Called as early as possible in the initialization procedure, before any
  /// window is loaded.
  /// </summary>
  virtual void earlyInit() {}

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

  virtual void onKeyboardInput(int message, int keyCode) {}
};

extern "C" {
TTSLModExport BaseMod *getModInstance();
}
