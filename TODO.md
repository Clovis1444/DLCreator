- Create more namespaces
- Mark getters as `const`
- Const qualifiers for pointers
- Tile texture generation algorithm
- add destructors

- Make all keyboard shortcuts `MainWindow` wide?
- Fix bug when user can use `Tool` associated with deleted `CellLayer`
- Consider implement History tracking when add/remove `Cell` to/from `CellCollection`

# IMPORTANT

- check all classes: if there is a member variable that allocates memory - dealocate it in destructor
- cache clang,qt in workflows

- QGraphicsItems does not have proper destructor: they should be deleted manually
