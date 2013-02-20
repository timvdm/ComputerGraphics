#ifndef INC_PLUGIN_H
#define INC_PLUGIN_H

#include "utils.h"

#include <list>

namespace CG {

  class Plugin;

  class PluginFactory
  {
    public:
      /**
       * Get the plugin type. This is the 'type' in field in the 'General'
       * section of the ini file.
       */
      virtual std::string type() const = 0;
      virtual Plugin* instance() const = 0;
  };

#define PLUGIN_FACTORY(type_str, klass) \
  class klass##Factory : public PluginFactory \
  { \
    public: \
      klass##Factory() { Plugin::registerPlugin(this); } \
      std::string type() const { return type_str; } \
      Plugin* instance() const { return new klass; } \
  }; \
  klass##Factory the##klass##Factory;

  class Plugin
  {
    public:
      virtual ~Plugin()
      {
      }

      /**
       * Get the generated image.
       */
      virtual img::EasyImage image(const ini::Configuration &conf) = 0;

      /**
       * Get a list of all plugins.
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

      static Plugin* findPlugin(const std::string &type)
      {
        std::list<PluginFactory*> &list = pluginFactories();
        for (std::list<PluginFactory*>::iterator i = list.begin(); i != list.end(); ++i)
          if ((*i)->type() == type)
            return (*i)->instance();
        return 0;
      }

  };

}

#endif
