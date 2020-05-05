# Getting the most out of the modern C++ language and standard libraries
## OxRSE 2020

## Instructions for attendees

- Some
- Instructions

## Slides

- A PDF version of the slides can be found in `slides/ModernCppCourse.pdf`
- To re-build the slides,
  ```
  cd build
  cmake -DSLIDES=ON ..
  make slides
  ```
- Building slides requires `pandoc`, `LaTeX` and the [Metropolis](https://github.com/matze/mtheme) `Beamer` theme

## Testing the slides (on Linux)

Each code block in `slides/RSEConUK2019CppWorkshop.md` can be compiled by making the `test_slides` target defined in `CMakeLists.txt`:

```
make test_slides
```

Code blocks beginning with `~~~cpp` will be tested in this way.
To explicitly write a C++ code block that is not tested, add whitespace into the code block fence: `~~~ cpp`.