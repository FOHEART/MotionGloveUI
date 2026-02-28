# skipAutoMoc.cmake
# Helper to prevent Qt AUTOMOC from parsing headers in transitive dependency source trees.

# AUTO* (AUTOMOC / AUTOUIC / AUTORCC) are all OFF globally.
# This function is retained for compatibility but has nothing to do.
function(skip_automoc_in_deps)
endfunction()
