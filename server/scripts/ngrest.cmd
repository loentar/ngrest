@IF "%NGR_TERM%" == "1" (
  SET NGR_TERM=
  SET "PATH=%PATH%;c:\msys64\usr\bin;c:\msys64\mingw64\bin"
  bash "%APPDATA%/ngrest/bin/ngrest" %*
) ELSE (
  SET NGR_TERM=1
  CALL %0 %* <NUL
)

