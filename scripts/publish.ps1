[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [string]$OutputDir,

    [string]$Preset = "ninja-release"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$RepoRootPath = $RepoRoot.Path

function Invoke-CheckedCommand {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Command,

        [Parameter(Mandatory = $true)]
        [string[]]$Arguments
    )

    & $Command @Arguments
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}

Push-Location $RepoRootPath
try {
    if ([string]::IsNullOrWhiteSpace($OutputDir)) {
        $OutputDir = $RepoRootPath
    }

    if (-not [System.IO.Path]::IsPathRooted($OutputDir)) {
        $OutputDir = Join-Path $RepoRootPath $OutputDir
    }

    $OutputDir = [System.IO.Path]::GetFullPath($OutputDir)

    Invoke-CheckedCommand -Command "cmake" -Arguments @("--preset", $Preset)
    Invoke-CheckedCommand -Command "cmake" -Arguments @(
        "--build",
        "--preset",
        $Preset,
        "--target",
        "codelinecalculator"
    )

    $buildDir = Join-Path (Join-Path $RepoRootPath "build") $Preset
    $exePath = Join-Path (Join-Path $buildDir "bin") "codelinecalculator.exe"
    if (-not (Test-Path $exePath)) {
        throw "Built executable was not found: $exePath"
    }

    New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null

    $destination = Join-Path $OutputDir "codelinecalculator.exe"
    Copy-Item -LiteralPath $exePath -Destination $destination -Force

    Write-Host "Published: $destination"
} finally {
    Pop-Location
}
