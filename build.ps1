param(
    [string]$Target = "all",
    [string]$Std = "c++11"
)

$ErrorActionPreference = "Stop"

if (Get-Command mingw32-make -ErrorAction SilentlyContinue) {
    & mingw32-make STD=$Std $Target
} elseif (Get-Command make -ErrorAction SilentlyContinue) {
    & make STD=$Std $Target
} else {
    Write-Error "Neither mingw32-make nor make is available in PATH."
}
