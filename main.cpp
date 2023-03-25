#include <iostream>
#include <nix/build-result.hh>
#include <nix/derivations.hh>
#include <nix/derived-path.hh>
#include <nix/globals.hh>
#include <nix/realisation.hh>
#include <nix/shared.hh>
#include <nix/store-api.hh>
#include <nlohmann/json.hpp>
#include <string_view>

using namespace nix;
using json = nlohmann::json;

DerivedPath toDerivedPath(ref<Store> store, StorePath storePath) {
  if (storePath.isDerivation()) {
    auto drv = store->readDerivation(storePath);
    return DerivedPath::Built{
        .drvPath = storePath,
        .outputs = drv.outputNames(),
    };
  } else {
    return DerivedPath::Opaque{
        .path = storePath,
    };
  }
}

int main(int argc, char **argv) {
  nix::initNix();
  nix::initLibStore();
  nix::initPlugins();
  settings.keepGoing = true;
  ref<Store> store = nix::openStore();
  std::vector<std::string> str_paths(argv + 1, argv + argc);
  BuildMode buildMode = bmNormal;
  std::vector<DerivedPath> paths = {};
  for (std::string &s : str_paths) {
    std::string_view v = s.c_str();
    StorePath storePath = store->parseStorePath(v);
    DerivedPath derived = toDerivedPath(store, storePath);
    paths.push_back(derived);
  }
  std::vector<BuildResult> results =
      store->buildPathsWithResults(paths, buildMode, store);
  json outputs;
  for (BuildResult &res : results) {
    DerivedPath derived = res.path;
    std::string path;
    std::visit(overloaded{[&](DerivedPath::Built built) {
                            path = store->printStorePath(built.drvPath);
                          },
                          [&](DerivedPath::Opaque opaque) {
                            path = store->printStorePath(opaque.path);
                          }},
               derived);
    json outPaths = {};
    DrvOutputs outs = res.builtOutputs;
    for (std::pair<DrvOutput, Realisation> const &x : outs) {
      Realisation r = x.second;
      outPaths.push_back(store->printStorePath(r.getPath()));
    }
    outputs[path] = {{"success", res.success()},
                     {"status", res.toString()},
                     {"outputs", outPaths}};
  }
  std::cout << outputs << std::endl;
}
