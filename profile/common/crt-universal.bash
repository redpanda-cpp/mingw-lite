export _CRT="ucrt"

# override arch
if [[ "$_ARCH" == "32" ]]; then
  export _WIN32_WINNT="0x0501"
fi
