file(REMOVE_RECURSE
  "liblwm2mserver.pdb"
  "liblwm2mserver.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/awa_server_static.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
