file(REMOVE_RECURSE
  "liblwm2mserver.pdb"
  "liblwm2mserver.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/awa_server_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
