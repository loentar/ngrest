@IF "%NGR_TERM%" == "1" (
  SET NGR_TERM=
  bash "%APPDATA%/ngrest/bin/ngrest" %*
) ELSE (
  SET NGR_TERM=1
  CALL %0 %* <NUL
)

