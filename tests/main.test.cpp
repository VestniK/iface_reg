#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <tests/main.test.hpp>

plugins_env global_plugin_env;

const plugins_env& get_plugins_env() {
    return global_plugin_env;
}

int main(int argc, char** argv) {
  Catch::Session session;

  using namespace Catch::clara;
  auto cli = session.cli()
    | Opt(global_plugin_env.plugin_path, "plugin_path")
      ["-p"]["--plugin"]
      ("path to the plugin to be laoded prorgammatically");
  session.cli( cli );
  const int returnCode = session.applyCommandLine( argc, argv );
  if(returnCode != 0)
    return returnCode;

  return session.run();
}
