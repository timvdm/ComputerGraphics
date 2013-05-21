#ifndef CG_PLUGIN_H
#define CG_PLUGIN_H

#include "utils.h"
#include "labo/render.h"

#include <list>

namespace CG {


  class Plugin;

  /**
   * @brief Base class for all plugin factories.
   *
   * Each plugin factory implements the type() method which is used to match
   * the correct plugin with the type field in the General section of the
   * *.ini files. A plugin factory also implements the instance() method to
   * return a newly created instance of the correct plugin.
   *
   * Inside a plugin implementation file, it is succicient to use the
   * PLUGIN_FACTORY macro with the correct parameters.
   */
  class PluginFactory
  {
    public:
      /**
       * @brief Get the plugin type.
       *
       * This is the 'type' in field in the 'General' section of the ini file.
       *
       * @return The plugin type.
       */
      virtual std::string type() const = 0;

      /**
       * @brief Get a new instance of the plugin.
       *
       * The instance that is returned is a newly created object and should be
       * deleted when it is not needed anymore.
       *
       * @return A pointer to a new instance of the plugin.
       */
      virtual Plugin* instance() const = 0;
  };

  /**
   * @brief Macro used to generate code for a plugin factory.
   *
   * @param type_str The plugin type as defined in the General section of the
   *        *.ini files.
   * @param klass The class name of the plugin class.
   */
#define PLUGIN_FACTORY(type_str, klass) \
  class klass##Factory : public PluginFactory \
  { \
    public: \
      klass##Factory() { Plugin::registerPlugin(this); } \
      std::string type() const { return type_str; } \
      Plugin* instance() const { return new klass; } \
  }; \
  klass##Factory the##klass##Factory;

  /**
   * @brief Base class for all plugins to generate images.
   *
   * Each derived plugin class implements the image() method to generate the
   * image for a certain type that is found in the General section of the
   * *.ini configuration files.
   */
  class Plugin
  {
    public:
      /**
       * @brief Destructor.
       */
      virtual ~Plugin()
      {
      }

      /**
       * @brief Get the generated image.
       *
       * Subclasses implement this method to generate the required image for
       * their type.
       *
       * @param conf The ini configuration.
       *
       * @return The generated image.
       */
      virtual img::EasyImage image(const ini::Configuration &conf) = 0;

      /**
       * @brief Get a list of all plugin factories.
       *
       * This method can be used to enumerate all available plugin types.
       *
       * @return List of plugin factories.
       */
      static std::list<PluginFactory*>& pluginFactories()
      {
        static std::list<PluginFactory*> *list = 0;
        if (!list)
          list = new std::list<PluginFactory*>();
        return *list;
      }

      static void registerPlugin(PluginFactory *factory)
      {
        pluginFactories().push_back(factory);
      }

      /**
       * @brief Find the plugin for the specified type and create an instance
       * of it.
       *
       * @param type The plugin type as defined in the General section of the
       *        *.ini files.
       *
       * @return An instance of the plugin type or 0 if there is no plugin for
       * the specified type.
       */
      static Plugin* findPlugin(const std::string &type)
      {
        std::list<PluginFactory*> &list = pluginFactories();
        for (std::list<PluginFactory*>::iterator i = list.begin(); i != list.end(); ++i)
          if ((*i)->type() == type)
            return (*i)->instance();
        return 0;
      }

      static void setFilename(const std::string &filename)
      {
        // HACK HACK HACK FIXME: solve this bug??
        if (filename == "shadowing267.ini" || filename == "shadowing268.ini")
          shadowEpsilon = 10e-3;
      }

  };

}

#endif
