#include <iostream>
#include <nix/build-result.hh>
#include <nix/path-with-outputs.hh>
#include <nix/store-api.hh>
#include <string_view>

using namespace nix;

int main(int argc, char **argv) {
  auto store = nix::openStore();
  // auto state = std::make_unique<EvalState>("", store, store);
  std::vector<std::string> str_paths(argv + 1, argv + argc);
  // std::vector<std::string> str_paths = {
  //     "/nix/store/9xfvq3mdbmmx40d220zpgqflwqrhn8dv-nix-build-results.drv"};
  BuildMode buildMode = bmNormal;
  std::vector<DerivedPath> paths = {};
  for (std::string &s : str_paths) {
    std::string_view v = s.c_str();
    StorePathWithOutputs storePath = parsePathWithOutputs(*store, v);
    paths.push_back(storePath.toDerivedPath());
    std::cout << s << std::endl;
  }
  std::vector<BuildResult> results =
      store->buildPathsWithResults(paths, buildMode, store);
  for (BuildResult &res : results) {
    std::cout << res.path.to_string(*store) << ": "
              << (res.success() ? "success" : "failure") << " - "
              << res.toString() << std::endl;
  }
  std::cout << "Compiled" << std::endl;
}
