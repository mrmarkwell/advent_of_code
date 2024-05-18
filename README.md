# Advent of Code

## Requirements

- Bazel 7

## How To

- Add a new .cc file for the day you want to work on (e.g. ./2015/day5.cc).
- Make a build rule in `./2015/BUILD.bazel` for your new file.
- Be sure to include any .txt file needed in the `data` attribute of the build
  rule.
- Write some of your code, and rebuild the compilation database with
  `./rebuild_compile_commands.sh`.
- Build your file with `bazel build //2015:day5` (or your target name).
- Run the file directly with `bazel run //2015:day5`.

## Notes

- Building compile-commands.json for bazel is done with [this](https://github.com/hedronvision/bazel-compile-commands-extractor)
  Using this command: `bazel run @hedron_compile_commands//:refresh_all`
- New libraries can be added to `./MODULE.bazel`.
  - See [the registry](https://registry.bazel.build/) to search for libraries.
