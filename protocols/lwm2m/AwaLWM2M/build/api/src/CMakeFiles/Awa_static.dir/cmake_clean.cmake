file(REMOVE_RECURSE
  "libawa.pdb"
  "libawa.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/Awa_static.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
