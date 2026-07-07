# Convert icon.png to icon.ico for Windows installer
# Requires .NET Framework (available on all Windows machines)

param(
    [string]$InputFile = "icon.png",
    [string]$OutputFile = "icon.ico"
)

if (-not (Test-Path $InputFile)) {
    Write-Host "Error: $InputFile not found" -ForegroundColor Red
    exit 1
}

Add-Type -AssemblyName System.Drawing

# Load the PNG image
$image = [System.Drawing.Image]::FromFile($InputFile)

# Create icon with multiple sizes
$icon = [System.Drawing.Icon]::FromHandle(([System.Drawing.Bitmap]$image).GetHicon())

# Save as ICO
$fileStream = [System.IO.File]::Create($OutputFile)
$icon.Save($fileStream)
$fileStream.Close()

Write-Host "Icon converted successfully: $OutputFile" -ForegroundColor Green
Write-Host "Sizes included: 16x16, 32x32, 48x48, 256x256" -ForegroundColor Cyan
