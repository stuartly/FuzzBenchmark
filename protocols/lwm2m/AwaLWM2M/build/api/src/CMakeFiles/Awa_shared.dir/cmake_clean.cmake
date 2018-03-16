file(REMOVE_RECURSE
  "libawa.pdb"
  "libawa.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/Awa_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
