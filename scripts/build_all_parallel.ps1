$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

$scripts = @(
    ".\build_msvc2005.bat",
    ".\build_msvc2022.bat",
    ".\build_msvc2026.bat",
    ".\build_mingw.bat",
    ".\build_cygwin.bat"
)

$jobs = @()
foreach ($script in $scripts) {
    Write-Host "Starting $script in background..."
    $jobs += Start-Job -ScriptBlock {
        param($s, $dir)
        Set-Location $dir
        & cmd.exe /c $s
    } -ArgumentList $script, $scriptDir
}

Write-Host "Waiting for jobs to complete..."
Wait-Job -Job $jobs | Out-Null

Write-Host "Job Outputs:"
foreach ($job in $jobs) {
    Receive-Job -Job $job
}

Write-Host "Generating report..."
& cmd.exe /c ".\generate_report.bat"
Write-Host "Done."
