- Create more namespaces
- Reduce amount of code duplication in `celllayer.h`
- Mark getters as `const`
- Const qualifiers for pointers!!!
- Create `frame_` and `background_` member variables in `Cell`
- Tile texture generation algorithm
- Implement zoom
- add destructors

- Change layers in `Cell` from ptr to LayerName(String)?.This will fix the problem with deleted Layers objects. Also this may be useful in future for implementing loading `Cells` from file.