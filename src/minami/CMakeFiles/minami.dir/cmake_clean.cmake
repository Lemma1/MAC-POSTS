file(REMOVE_RECURSE
  "libminami.pdb"
  "libminami.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang)
  include(CMakeFiles/minami.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
