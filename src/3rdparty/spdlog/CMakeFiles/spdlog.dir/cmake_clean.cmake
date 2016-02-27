file(REMOVE_RECURSE
  "libspdlog.pdb"
  "libspdlog.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang)
  include(CMakeFiles/spdlog.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
